// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "TownieAIController.h"
#include "TownCharacter.h"
#include "GameFramework/Character.h"
#include "PrintHelper.h"
#include "ZombieTownGameModeBase.h"

AZombieAIController::AZombieAIController()
{
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
	townCharacter = Cast<ATownCharacter>(GetCharacter());
	GetWorld()->GetTimerManager().SetTimer(searchEnemiesHandle, this, &AZombieAIController::SearchNearestEnemyActor, SearchForEnemiesEverySeconds + FMath::FRand(), true);
	PrimaryActorTick.bCanEverTick = true;
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
		MoveToLocation(sendLocation);
		isSendingThisFrame = false;
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
	}
	else
	{
		SetFocus(followingActor);
		MoveToActor(followingActor, FollowSummonerRadius);

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