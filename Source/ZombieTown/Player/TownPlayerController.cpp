// Fill out your copyright notice in the Description page of Project Settings.


#include "TownPlayerController.h"

#include "ZombieTown/Helpers/ActorHelpers.h"
#include "ZombieTown/Interfaces/TargetableInterface.h"
#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/AI/TownieAIController.h"
#include "ZombieTown/AI/ZombieAIController.h"
#include "ZombieTown/ZombieTownGameModeBase.h"
#include "ZombieTown/Actors/ZombieSpawnpoint.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ZombieTown/Components/UsableObject.h"
#include "ZombieTown/Helpers/PrintHelper.h"
#include "ZombieTown/Components/LevelExit.h"

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

	// Control player walking input.
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
	// Set up all the silly input component stuff.
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

// This function is responsible for summoning zombies near the player.
void ATownPlayerController::SummonZombies()
{
	// If the game is paused, don't do anything.
	if (IsGamePaused())
	{
		return;
	}

	// If there are particles to summon, spawn them at the player's location.
	if (SummonParticles)
	{
		FTransform transform = GetCharacter()->GetActorTransform();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SummonParticles, transform.GetLocation(), transform.GetRotation().Rotator(), transform.GetScale3D(), true);
	}

	// Get all the zombie spawnpoints within the summon distance.
	TArray<AZombieSpawnpoint*> spawnPoints;
	townCharacter->GetActorsInRadius<AZombieSpawnpoint>(SummonDistance, spawnPoints);

	// Spawn zombies at each of the spawnpoints.
	for (AZombieSpawnpoint* zombieSpawnPoint : spawnPoints)
	{
		zombieSpawnPoint->SpawnZombie();
	}

	// Get all the zombies within the summon distance.
	TArray<AZombieAIController*> zombies;
	if (!GetZombiesInRadius(SummonDistance, zombies))
	{
		return;
	}

	// Make each of the zombies move toward the player.
	for (AZombieAIController* zombie : zombies)
	{
		zombie->SummonTowardThisFrame(GetCharacter());
	}

	// Get all the dead townies within the summon distance.
	TArray<ATownieAIController*> deadTownies;
	if (GetTowniesInRadius(SummonDistance, deadTownies, LivingStatus::DeadOnly))
	{
		// Turn each of the dead townies into a zombie.
		for (ATownieAIController* townie : deadTownies)
		{
			townie->GetTownCharacter()->BecomeZombie();
		}
	}
}

// This function traces a sphere along the forward direction of the player's character to detect the first blocking hit 
// within the specified distance, and returns the hit result.
// If there is a blocking hit, the function returns true and the hit result is returned in the `result` parameter.
// If there is no blocking hit, the function returns false.
// The `dist` parameter specifies the maximum distance to trace the sphere.
// The `actorsToIgnore` parameter is an optional array of actors to ignore during the trace.
bool ATownPlayerController::GetPointingAt(float dist, FHitResult& result, const TArray<AActor*> actorsToIgnore) const
{
	// Calculate the start and end points of the trace, based on the player's location and forward direction.
	FVector origin = GetCharacter()->GetActorLocation() + FVector::UpVector * SphereTraceSendRadius;
	FVector start = origin + GetCharacter()->GetActorForwardVector() * SphereTraceSendRadius;
	FVector end = start + GetCharacter()->GetActorForwardVector() * dist;

	// Set up collision query parameters, including any actors to ignore.
	FCollisionQueryParams params;
	for (const AActor* actor : actorsToIgnore)
	{
		params.AddIgnoredActor(actor);
	}

	// Perform the sphere trace and return the result.
	// The trace is performed using the SphereTraceSingle function from the KismetSystemLibrary.
	// The trace is a sphere shape, with radius `SphereTraceSendRadius`, and the trace type is `TraceTypeQuery1`.
	// Debug visualization of the trace is disabled, and `true` is passed for `bTraceComplex`.
	// If there is a blocking hit, the hit result is returned in the `result` parameter.
	return UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
		start, end, SphereTraceSendRadius,
		ETraceTypeQuery::TraceTypeQuery1, false,
		actorsToIgnore,
		EDrawDebugTrace::None, result, true);
}


// Send zombies to attack or move towards a target.
void ATownPlayerController::SendZombies()
{
	// Retrieve a list of zombies within a certain radius.
	TArray<AZombieAIController*> zombies;
	GetZombiesInRadius(SummonDistance, zombies);

	// Add the player and all zombies to an array of actors to ignore for sphere tracing.
	TArray<AActor*> zombieActors;
	zombieActors.Add(GetCharacter());
	for (const AZombieAIController* zombie : zombies)
	{
		zombieActors.Add(zombie->GetCharacter());
	}

	// Initialize variables for tracing and targeting.
	FHitResult hitResult;
	FVector pointLocation;
	FVector myLocation = GetCharacter()->GetActorLocation();

	// Determine whether the player is pointing at an object or not.
	bool pointingAtObject = !GetPointingAt(SendDistance, hitResult, zombieActors);

	// For each zombie, send them towards the target object or point.
	for (AZombieAIController* zombie : zombies)
	{
		AActor* targetableActor = nullptr;
		TScriptInterface<ITargetableInterface> targetableInterface = nullptr;

		if (pointingAtObject)
		{
			// If the player is pointing at an object, send the zombies to the point.
			pointLocation = GetCharacter()->GetActorLocation() + GetCharacter()->GetActorForwardVector() * SendDistance;
		}
		else
		{
			// If the player is pointing at a targetable object, send the zombie to attack the object.
			pointLocation = hitResult.ImpactPoint;
			if (hitResult.GetActor())
			{
				// Find the targetable interface on the actor or its parent actor.
				targetableInterface = UActorHelpers::FindActorOrComponentInterface<ITargetableInterface>(UTargetableInterface::StaticClass(), hitResult.GetActor());
				if (!targetableInterface && hitResult.GetActor()->GetParentActor())
				{
					targetableInterface = UActorHelpers::FindActorOrComponentInterface<ITargetableInterface>(UTargetableInterface::StaticClass(), hitResult.GetActor()->GetParentActor());
				}

				// If the targetable interface exists and can be targeted, send the zombie to attack the target.
				if (targetableInterface && targetableInterface->CanTarget())
				{
					auto targetResult = targetableInterface->GetTargetInfo(myLocation);
					targetableActor = targetResult.targetActor;
				}
			}
		}

		// Send the zombie towards the target object or point.
		zombie->SendTowardThisFrame(pointLocation);

		// If the zombie has a targetable actor and interface, set the zombie's target to the actor and interface.
		if (targetableActor && targetableInterface)
		{
			zombie->TargetInterface(targetableActor, targetableInterface);
		}
	}

	// If SendParticles is set, spawn particle effects at the zombie's current location and the target location.
	if (SendParticles)
	{
		FVector origin = GetCharacter()->GetActorLocation() + FVector::UpVector * 50;
		FVector normal = (pointLocation - origin);
		FVector randPos = origin + FMath::FRand() * normal;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SendParticles, randPos, FRotator(), FVector(1, 1, 1), true);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SendParticles, pointLocation, FRotator(), FVector(1, 1, 1), true);
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

// Called when using objects. Technically, just when the "use" button is pressed.
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
	//Skip the current scripted event.
	if (townCharacter->GameMode->AllowRequestNextScriptedEvent &&
		townCharacter->GameMode->IsPausedForScriptedEvent)
	{
		townCharacter->GameMode->RequestNextScriptedEvent = true;
		townCharacter->GameMode->AllowRequestNextScriptedEvent = false;
	}
	else
	{
		// Use all useable objects.
		TArray<UUsableObject*> usableObjects;
		bool foundAny = townCharacter->GetComponentsInRadius<UUsableObject>(UseRadius, usableObjects);
		for (UUsableObject* obj : usableObjects)
		{
			obj->OnUse(townCharacter);
		}
	}
}

// Called to force us to go to the next level.
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