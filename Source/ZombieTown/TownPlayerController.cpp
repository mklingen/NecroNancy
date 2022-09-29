// Fill out your copyright notice in the Description page of Project Settings.


#include "TownPlayerController.h"

#include "ActorHelpers.h"
#include "TargetableInterface.h"
#include "TownCharacter.h"
#include "TownieAIController.h"
#include "ZombieAIController.h"
#include "ZombieTownGameModeBase.h"
#include "ZombieSpawnpoint.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UsableObject.h"
#include "PrintHelper.h"
#include "LevelExit.h"

ATownPlayerController::ATownPlayerController()
{

}

void ATownPlayerController::BeginPlay()
{
	Super::BeginPlay();
	townCharacter = Cast<ATownCharacter>(GetCharacter());
}

bool ATownPlayerController::IsGamePaused() const
{
	if (!townCharacter)
	{
		return false;
	}
	if (!townCharacter->GameMode)
	{
		return false;
	}
	return townCharacter->GameMode->IsPausedForScriptedEvent;
}

void ATownPlayerController::Tick(float dt)
{
	Super::Tick(dt);

	if (IsGamePaused())
	{
		return;
	}

	if ((FMath::Abs(lastLeftRightInput) > 1e-3 || FMath::Abs(lastUpDownInput) > 1e-3) &&
		!IsSummoning && !IsSending)
	{
		FRotator rotator = PlayerCameraManager->GetCameraRotation();
		FVector camRight = rotator.RotateVector(FVector::RightVector);
		camRight.Z = 0;
		camRight.Normalize();
		FVector camFwd = rotator.RotateVector(FVector::ForwardVector);
		camFwd.Z = 0;
		camFwd.Normalize();
		FVector input = (camRight * lastLeftRightInput + camFwd * lastUpDownInput).GetSafeNormal() * FMath::Max(FMath::Abs(lastLeftRightInput), FMath::Abs(lastUpDownInput));
		this->GetCharacter()->AddMovementInput(input);
	}
}

void ATownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	this->InputComponent->BindAxis(TEXT("UpDown"), this, &ATownPlayerController::OnUpDown);
	this->InputComponent->BindAxis(TEXT("RightLeft"), this, &ATownPlayerController::OnLeftRight);
	this->InputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &ATownPlayerController::OnAttack);
	this->InputComponent->BindAxis(TEXT("Summon"), this, &ATownPlayerController::OnSummon);
	this->InputComponent->BindAxis(TEXT("Send"), this, &ATownPlayerController::OnSending);
	this->InputComponent->BindAction(TEXT("Use"), IE_Pressed, this, &ATownPlayerController::OnUse);
	this->InputComponent->BindAction(TEXT("DebugNextLevel"), IE_Pressed, this, &ATownPlayerController::OnDebugNextLevel);
}

void ATownPlayerController::OnLeftRight(float value)
{
	if (IsGamePaused())
	{
		return;
	}
	lastLeftRightInput = value;
}

void ATownPlayerController::OnUpDown(float value)
{
	if (IsGamePaused())
	{
		return;
	}
	lastUpDownInput = value;
}

void ATownPlayerController::OnAttack()
{
	if (IsGamePaused())
	{
		return;
	}
	if (townCharacter)
	{
		townCharacter->Attack();
	}
}

void ATownPlayerController::OnSummon(float value)
{
	if (IsGamePaused())
	{
		return;
	}
	if (!CanSummon)
	{
		return;
	}
	IsSummoning = FMath::Abs(value) > 1e-3 && !IsSending;

	if (IsSummoning)
	{
		SummonZombies();
	}
}

void ATownPlayerController::OnSending(float value)
{
	if (IsGamePaused())
	{
		return;
	}
	if (!CanSend)
	{
		return;
	}
	IsSending = FMath::Abs(value) > 1e-3 && !IsSummoning;

	if (IsSending)
	{
		SendZombies();
	}
}

void ATownPlayerController::SummonZombies()
{
	if (IsGamePaused())
	{
		return;
	}
	if (SummonParticles)
	{
		FTransform transform = GetCharacter()->GetActorTransform();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SummonParticles, transform.GetLocation(), transform.GetRotation().Rotator(), transform.GetScale3D(), true);
	}
	TArray<AZombieSpawnpoint*> spawnPoints;
	townCharacter->GetActorsInRadius<AZombieSpawnpoint>(SummonDistance, spawnPoints);

	for (AZombieSpawnpoint* zombieSpawnPoint : spawnPoints)
	{
		zombieSpawnPoint->SpawnZombie();
	}

	//DrawDebugSphere(GetWorld(), GetCharacter()->GetActorLocation(), SummonDistance, 16, FColor::Green);
	TArray<AZombieAIController*> zombies;
	if (!GetZombiesInRadius(SummonDistance, zombies))
	{
		return;
	}

	for (AZombieAIController* zombie : zombies)
	{
		zombie->SummonTowardThisFrame(GetCharacter());
	}

	TArray<ATownieAIController*> deadTownies;
	if (GetTowniesInRadius(SummonDistance, deadTownies, LivingStatus::DeadOnly))
	{
		for (ATownieAIController* townie : deadTownies)
		{
			townie->GetTownCharacter()->BecomeZombie();
		}
	}
}

bool ATownPlayerController::GetPointingAt(float dist, FHitResult& result, const TArray<AActor*> actorsToIgnore) const
{
	FVector origin = GetCharacter()->GetActorLocation() + FVector::UpVector * SphereTraceSendRadius;
	FVector start = origin + GetCharacter()->GetActorForwardVector() * SphereTraceSendRadius;
	FVector end = start + GetCharacter()->GetActorForwardVector() * dist;
	FCollisionQueryParams params;
	for (const AActor* actor : actorsToIgnore)
	{
		params.AddIgnoredActor(actor);
	}

	return UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
		start, end, SphereTraceSendRadius,
		ETraceTypeQuery::TraceTypeQuery1, false,
		actorsToIgnore,
		EDrawDebugTrace::None, result, true);
}


void ATownPlayerController::SendZombies()
{
	TArray<AZombieAIController*> zombies;
	if (!GetZombiesInRadius(SummonDistance, zombies))
	{
		// Still point.
	}

	TArray<AActor*> zombieActors;
	zombieActors.Add(GetCharacter());
	for (const AZombieAIController* zombie : zombies)
	{
		zombieActors.Add(zombie->GetCharacter());
	}

	FHitResult hitResult;
	FVector pointLocation;
	FVector myLocation = GetCharacter()->GetActorLocation();
	AActor* targetableActor = nullptr;
	if (!GetPointingAt(SendDistance, hitResult, zombieActors))
	{
		pointLocation = GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * SendDistance;
	}
	else
	{
		pointLocation = hitResult.ImpactPoint;
		if (hitResult.GetActor())
		{
			LOGI("Hit %s", *(hitResult.GetActor()->GetActorNameOrLabel()));
			ITargetableInterface* targetable = UActorHelpers::FindActorOrComponentInterface<ITargetableInterface>(UTargetableInterface::StaticClass(), hitResult.GetActor());
			if (!targetable && hitResult.GetActor()->GetParentActor())
			{
				targetable = UActorHelpers::FindActorOrComponentInterface<ITargetableInterface>(UTargetableInterface::StaticClass(), hitResult.GetActor()->GetParentActor());
			}

			if (targetable)
			{
				auto targetResult = targetable->GetTargetInfo(myLocation);
				targetableActor = targetResult.targetActor;
				LOGI("Nancy is targeting %s", *(targetableActor->GetActorNameOrLabel()));
			}
			else
			{
				LOGI("Item wasn't targetable.");
			}
		}
	}
	if (SendParticles)
	{
		FVector origin = GetCharacter()->GetActorLocation() + FVector::UpVector * 50;
		FVector normal = (pointLocation - origin);
		FVector randPos = origin + FMath::FRand() * normal;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SendParticles, randPos, FRotator(), FVector(1, 1, 1), true);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SendParticles, pointLocation, FRotator(), FVector(1, 1, 1), true);
	}
	for (AZombieAIController* zombie : zombies)
	{
		zombie->SendTowardThisFrame(pointLocation);
		if (targetableActor)
		{
			zombie->TargetActor(targetableActor);
		}
	}
}

bool ATownPlayerController::GetZombiesInRadius(float radius, TArray<AZombieAIController*>& zombies) const
{
	if (!townCharacter)
	{
		return false;
	}

	return townCharacter->GetControllersInRadius<AZombieAIController>(radius, zombies);
}

bool ATownPlayerController::GetTowniesInRadius(float radius, TArray<ATownieAIController*>& towniesOut,
	ATownPlayerController::LivingStatus livingStatus) const
{
	if (!townCharacter)
	{
		return false;
	}

	TArray<ATownieAIController*> townies;
	if (!townCharacter->GetControllersInRadius<ATownieAIController>(radius, townies))
	{
		return false;
	}

	for (ATownieAIController* townie : townies)
	{
		switch (livingStatus)
		{
			case ATownPlayerController::LivingStatus::AliveOnly:
			{
				if (townie->GetIsDead())
				{
					continue;
				}
				break;
			}
			case ATownPlayerController::LivingStatus::AliveOrDead:
			{
				break;
			}
			case ATownPlayerController::LivingStatus::DeadOnly:
			{
				if (!townie->GetIsDead())
				{
					continue;
				}
				break;
			}
		}
		towniesOut.Add(townie);
	}
	return towniesOut.Num() > 0;
}

void ATownPlayerController::OnUse()
{
	if (!townCharacter)
	{
		return;
	}
	if (!townCharacter->GameMode)
	{
		return;
	}
	if (townCharacter->GameMode->AllowRequestNextScriptedEvent &&
		townCharacter->GameMode->IsPausedForScriptedEvent)
	{
		townCharacter->GameMode->RequestNextScriptedEvent = true;
		townCharacter->GameMode->AllowRequestNextScriptedEvent = false;
	}
	else
	{
		LOGI("Using...");
		TArray<UUsableObject*> usableObjects;
		bool foundAny = townCharacter->GetComponentsInRadius<UUsableObject>(UseRadius, usableObjects);
		LOGI("Found %d usable objects", usableObjects.Num());
		for (UUsableObject* obj : usableObjects)
		{
			obj->OnUse(townCharacter);
		}
	}
}

void ATownPlayerController::OnDebugNextLevel()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		ULevelExit* exitComponent = actor->FindComponentByClass<ULevelExit>();
		if (exitComponent)
		{
			exitComponent->DebugGoToNextLevel(ZombieClass, DefaultNumZombies, ToolClass, true);
			break;
		}
	}
}