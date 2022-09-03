// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEntrance.h"
#include "TownCharacter.h"
#include "ZombieAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ALevelEntrance::ALevelEntrance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

// Called when the game starts or when spawned
void ALevelEntrance::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelEntrance::EnterLevel()
{
	if (TeleportPlayer)
	{
		AActor* player = UGameplayStatics::GetPlayerPawn(this, 0);
		if (player)
		{
			player->TeleportTo(GetActorLocation(), player->GetActorRotation(), false, true);
		}
	}

	if (TeleportZombies)
	{
		TArray<AActor*> zombies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieAIController::StaticClass(), zombies);
		int idx = 0;
		double teleportRadius = 200;
		int numZombies = zombies.Num();
		double dTheta = 0;
		if (numZombies > 0)
		{
			dTheta = 2.0f * PI / numZombies;
		}
		for (AActor* zombie : zombies)
		{
			AZombieAIController* zombieAI = Cast<AZombieAIController>(zombie);
			if (!zombieAI)
			{
				continue;
			}
			ATownCharacter* character = zombieAI->GetTownCharacter();
			if (zombieAI->GetIsDead())
			{
				if (character)
				{
					character->Destroy();
				}
				zombieAI->Destroy();
				continue;
			}
			if (!character)
			{
				continue;
			}
			FVector offset(sin(idx * dTheta + PI) * teleportRadius, cos(idx * dTheta + PI) * teleportRadius, 0);
			character->TeleportTo(GetActorLocation() + offset, character->GetActorRotation(), false, true);

			AActor* player = UGameplayStatics::GetPlayerPawn(this, 0);
			if (player)
			{
				zombieAI->SummonTowardThisFrame(player);
			}
			idx++;
		}
	}
}
