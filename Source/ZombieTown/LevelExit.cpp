// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelExit.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "ZombieTownGameModeBase.h"
#include "LevelEntrance.h"
#include "PrintHelper.h"
#include "TownCharacter.h"
#include "TownPlayerController.h"
#include "ZombieAIController.h"
#include "ToolComponent.h"
#include "OnLevelExitedInterface.h"

// Sets default values for this component's properties
ULevelExit::ULevelExit()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void ULevelExit::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate scriptDelegate;
	scriptDelegate.BindUFunction(this, "OnBeginOverlap");
	GetOwner()->OnActorBeginOverlap.Add(scriptDelegate);
	
	player = UGameplayStatics::GetPlayerCharacter(this, 0);
	gameMode = Cast<AZombieTownGameModeBase>(GetWorld()->GetAuthGameMode());
	
}

void ULevelExit::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)  
{
	if (!IsEnabled)
	{
		return;
	}

	if (Other != player)
	{
		return;
	}

	Exit();
}

void ULevelExit::Exit()
{
	if (NextLevel == "")
	{
		LOGE("Next level is empty. Cannot exit.");
		return;
	}
	if (CurrentLevel == "")
	{
		LOGE("Current level is empty. Cannot exit.");
		return;
	}
	ULevel* level = GetOwner()->GetLevel();
	if (!level)
	{
		LOGE("No current level?");
		return;
	}
	ULevel* persistentLevel = GetWorld()->PersistentLevel;
	if (!persistentLevel)
	{
		LOGE("Failed to get persistent level.");
		return;
	}
	if (!persistentLevel->IsPersistentLevel())
	{
		LOGE("Outer level isn't the persistent level.");
		return;
	}

	if (gameMode)
	{
		gameMode->IsPausedForScriptedEvent = true;
	}
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (cameraManager)
	{
		cameraManager->StartCameraFade(0.0f, 1.0f, 0.5f, FLinearColor::Black, false, true);
	}

	TArray<UActorComponent*> exitCallbacks;
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		exitCallbacks.Empty();
		exitCallbacks = actor->GetComponentsByInterface(UOnLevelExitedInterface::StaticClass());
		for (UActorComponent* component : exitCallbacks)
		{
			IOnLevelExitedInterface* interface = Cast<IOnLevelExitedInterface>(component);
			if (interface)
			{
				interface->OnLevelExited();
			}
		}
	}

	// Finishes the camera fade before we load the next level.
	FTimerDynamicDelegate delegateTimer;
	delegateTimer.BindUFunction(this, "LoadNextLevel");

	GetWorld()->GetTimerManager().SetTimer(fadeTimer, delegateTimer, 0.75f, false);
}

void ULevelExit::LoadNextLevel()
{
	// Loads the level asynchronously, and then calls UnloadPreviousLevel.
	FLatentActionInfo latentInfo;
	latentInfo.CallbackTarget = this;
	latentInfo.Linkage = 0;
	latentInfo.UUID = 1;
	latentInfo.ExecutionFunction = "UnloadPreviousLevel";
	UGameplayStatics::LoadStreamLevel(this, NextLevel, true, true, latentInfo);
}




void ULevelExit::UnloadPreviousLevel()
{
	OnNextLevelLoaded();
	FLatentActionInfo latentInfo;
	UGameplayStatics::UnloadStreamLevel(this, CurrentLevel, latentInfo, true);
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (cameraManager)
	{
		cameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor::Black);
	}
	if (gameMode)
	{
		gameMode->IsPausedForScriptedEvent = false;
	}
}

void ULevelExit::OnNextLevelLoaded()
{
	ALevelEntrance* level = Cast<ALevelEntrance>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelEntrance::StaticClass()));
	if (!level)
	{
		LOGE("Next level has no entrance!");
		return;
	}
	level->EnterLevel();
}

void ULevelExit::DebugGoToNextLevel(TSubclassOf<ATownCharacter> zombieClass, int numZombies, TSubclassOf<AActor> nancyTool, bool nancyCanSummon)
{
	TArray<AActor*> existingZombies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieAIController::StaticClass(), existingZombies);
	for (AActor* zombie : existingZombies)
	{
		zombie->Destroy();
	}

	FActorSpawnParameters params;
	params.bNoFail = true;
	for (int i = 0; i < numZombies; i++)
	{
		GetWorld()->SpawnActor<AActor>(zombieClass, params);
	}

	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (playerPawn)
	{
		ATownPlayerController* controller = Cast<ATownPlayerController>(playerPawn->GetController());
		if (controller)
		{
			controller->CanSummon = nancyCanSummon;
			controller->CanSend = nancyCanSummon;
		}
		UToolComponent* tool = player->FindComponentByClass<UToolComponent>();
		if (tool)
		{
			tool->Attach(nancyTool);
		}
	}
	Exit();
}