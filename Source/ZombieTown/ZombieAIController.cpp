// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "TownieAIController.h"
#include "TownCharacter.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PrintHelper.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ZombieTownGameModeBase.h"
#include "ActorHelpers.h"

AZombieAIController::AZombieAIController()
{
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
	townCharacter = Cast<ATownCharacter>(GetCharacter());
	GetWorld()->GetTimerManager().SetTimer(searchEnemiesHandle, this, &AZombieAIController::SearchNearestEnemyActor, SearchForEnemiesEverySeconds + FMath::FRand(), true);
	PrimaryActorTick.bCanEverTick = true;
	player = Cast<ATownCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void AZombieAIController::Tick(float dt)
{
	Super::Tick(dt);
	if (!townCharacter)
	{
		townCharacter = Cast<ATownCharacter>(GetCharacter());
	}
	else if (townCharacter->GetIsDead())
	{
		return;
	}
	if (IsGamePaused())
	{
		StopMovement();
		return;
	}

	if (summonTargetThisFrame)
	{
		MoveToActor(summonTargetThisFrame);
		if (followingActor != summonTargetThisFrame)
		{
			followingActor = summonTargetThisFrame;
		}
		summonTargetThisFrame = nullptr;
	}
	else if (isSendingThisFrame)
	{
		if (targetedActor) {
			MoveTowardTargetThisFrame(targetedActor);
		}
		else {
			MoveToLocation(sendLocation);
		}
		isSendingThisFrame = false;
	}
	else if (targetedActor != nullptr)
	{
		UHealthComponent* health = targetedActor->FindComponentByClass<UHealthComponent>();
		if (health && health->IsDead())
		{
			targetedActor = nullptr;
		}
		else
		{
			SetFocus(targetedActor);
			MoveTowardTargetThisFrame(targetedActor);
			FVector targetPoint;
			float dist = FVector::Dist(ptOnTargetedActor, townCharacter->GetActorLocation());

			if (townCharacter)
			{
				if (dist < townCharacter->MeleeAttackRange && townCharacter->AttackWithCooldown(AttackCooldownSeconds))
				{
					DamageActor(targetedActor);
				}
			}
		}
	}
	else if (nearestEnemyActor)
	{
		float dist = FVector::Dist(nearestEnemyActor->GetActorLocation(), townCharacter->GetActorLocation());
		if (dist > EnemyVisibilityRadius || nearestEnemyActor->GetIsDead())
		{
			nearestEnemyActor = nullptr;
		}
		else
		{
			MoveTowardEnemyThisFrame(nearestEnemyActor);
			if (townCharacter && dist < townCharacter->MeleeAttackRange)
			{
				if (townCharacter->AttackWithCooldown(AttackCooldownSeconds))
				{
					DamageActor(nearestEnemyActor);
				}
			}
		}
	}
	else if (!followingActor)
	{
		StopMovement();
		MaybeAvoidPlayer();
	}
	else
	{
		SetFocus(followingActor);
		MoveToActor(followingActor, FollowSummonerRadius);
		MaybeAvoidPlayer();
		float dist = (followingActor->GetActorLocation() - GetCharacter()->GetActorLocation()).Length();
		if (dist > StopFollowSummonerRadius)
		{
			followingActor = nullptr;
		}
	}
}

void AZombieAIController::DamageActor(AActor* actor)
{
	FDamageEvent damageEvent;
	actor->TakeDamage(AttackDamage, damageEvent, this, townCharacter);

	if (Knockback > 0)
	{
		ACharacter* character = Cast<ACharacter>(actor);
		if (character)
		{
			UCharacterMovementComponent* movement = character->GetCharacterMovement();
			if (movement)
			{
				FVector dir = character->GetActorLocation() - GetCharacter()->GetActorLocation();
				dir.Z = 0;
				dir.Normalize();
				movement->AddImpulse(dir * Knockback, true);
			}
		}
	}
}

void AZombieAIController::SendTowardThisFrame(const FVector& location)
{
	sendLocation = location;
	followingActor = nullptr;
	isSendingThisFrame = true;
}

void AZombieAIController::MoveTowardEnemyThisFrame(ATownCharacter* actor)
{
	followingActor = nullptr;
	nearestEnemyActor = actor;
	MoveToActor(actor);
}

void AZombieAIController::MoveTowardTargetThisFrame(AActor* actor)
{
	nearestEnemyActor = nullptr;
	targetedActor = actor;
	MoveTo(ptOnTargetedActor);
}

void AZombieAIController::TargetActor(AActor* target)
{
	followingActor = nullptr;
	nearestEnemyActor = nullptr;
	targetedActor = target;
	UActorHelpers::DistanceToActor(targetedActor,
		townCharacter->GetActorLocation(), ECollisionChannel::ECC_WorldDynamic, ptOnTargetedActor);
}

void AZombieAIController::SearchNearestEnemyActor()
{
	if (!townCharacter)
	{
		LOGW("No Character...");
		return;
	}
	TArray<ATownieAIController*> townies;
	townCharacter->GetControllersInRadius<ATownieAIController>(EnemyVisibilityRadius, townies);
	float closestDist = std::numeric_limits<float>::max();
	if (nearestEnemyActor)
	{
		closestDist = FVector::DistSquared(nearestEnemyActor->GetActorLocation(), townCharacter->GetActorLocation());
	}
	for (const auto* townie : townies)
	{
		if (townie->GetIsDead())
		{
			continue;
		}
		float dist = FVector::DistSquared(townie->GetCharacter()->GetActorLocation(), townCharacter->GetActorLocation());
		if (dist < closestDist)
		{
			nearestEnemyActor = townie->GetTownCharacter();
			closestDist = dist;
		}
	}
}

bool AZombieAIController::GetIsDead() const
{
	if (!townCharacter)
	{
		return false;
	}
	return townCharacter->GetIsDead();
}

void AZombieAIController::SummonTowardThisFrame(AActor* actor)
{
	summonTargetThisFrame = actor;
}

bool AZombieAIController::IsGamePaused() const
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

void AZombieAIController::MaybeAvoidPlayer()
{
	if (!player)
	{
		return;
	}
	if (!townCharacter)
	{
		return;
	}
	UPawnMovementComponent* movement = townCharacter->GetMovementComponent();
	if (!movement)
	{
		return;
	}
	UCapsuleComponent* capsule = player->GetCapsuleComponent();
	if (!capsule)
	{
		return;
	}
	UCapsuleComponent* myCapsule = townCharacter->GetCapsuleComponent();
	if (!myCapsule)
	{
		return;
	}
	if (capsule->OverlapComponent(myCapsule->GetComponentLocation(), 
		myCapsule->GetComponentRotation().Quaternion(), myCapsule->GetCollisionShape(10.0f)))
	{
		movement->AddInputVector((townCharacter->GetActorLocation() - player->GetActorLocation()) * 1000);
	}
}