// Fill out your copyright notice in the Description page of Project Settings.


#include "TownieAIController.h"

#include "ZombieTown/AI/ZombieAIController.h"
#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/Components/GunComponent.h"
#include "ZombieTown/Helpers/PrintHelper.h"
#include "ZombieTown/ZombieTownGameModeBase.h"
#include "ZombieTown/Components/MeleeWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"

ATownCharacter* ATownieAIController::GetTownCharacter() const
{
	return Cast<ATownCharacter>(GetCharacter());;
}

void ATownieAIController::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(searchEnemiesHandle, this, &ATownieAIController::SearchNearestEnemyActor, SearchForEnemiesEverySeconds + FMath::FRand(), true);
	PrimaryActorTick.bCanEverTick = true;
	lastPatrolTime = -RandomPatrolTime;
	PanicTime = PanicTime + FMath::FRand();
}

bool ATownieAIController::IsReadyToAttack() const
{
	return aggression >= AggressionToCauseAttack;
}

bool ATownieAIController::IsReadyToAttackHysteresis(float timeout) const
{
	return IsReadyToAttack() || GetWorld()->GetTimeSeconds() - timeLastReadyToAttack < timeout;
}


void ATownieAIController::RespondToEnemy(float time, float dt)
{
	HandleAggression(dt);
	SetFocus(nearestEnemyActor);
	FVector pos = GetCharacter()->GetActorLocation();
	FVector enemyPos = nearestEnemyActor->GetActorLocation();
	float dist = (pos - enemyPos).Length();
	if (dist > EnemyVisibilityRadius)
	{
		RespondToDistantEnemey();
		ReduceFright(dt);
		ReduceAggression(dt);
		IdleBehavior(time);
		GetTownCharacter()->IsAiming = false;
	}
	else
	{
		ManagePanicWhenNearEnemy(dt);
		if (IsPanicking())
		{
			return;
		}
		if (IsReadyToAttack())
		{
			if (HasGun())
			{
				GetTownCharacter()->IsAiming = true;
			}

			MaybeAttackEnemy(dist);
			bool isAiming = GetTownCharacter()->IsAiming;
			if (!isAiming)
			{
				if (!HasMeleeWeapon())
				{
					FleeEnemy(pos, enemyPos);
				}
				else
				{
					StopMovement();
					SetFocus(nearestEnemyActor);
				}
			}
			else
			{
				AimAtEnemy(isAiming, dist, enemyPos, pos);
			}
		}
		else
		{
			FleeEnemy(pos, enemyPos);
			GetTownCharacter()->IsAiming = false;
		}
	}
}

void ATownieAIController::AimAtEnemy(bool isAiming, float dist, FVector& enemyPos, FVector& pos)
{
	SetFocus(nearestEnemyActor);
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = false;
	if (isAiming && dist < FleeWhileAimingDist)
	{
		// Move sorta away, lol
		GetCharacter()->GetCharacterMovement()->AddInputVector((enemyPos - pos) * 1000);
	}
	else
	{
		StopMovement();
	}
}

void ATownieAIController::FleeEnemy(FVector& pos, FVector& enemyPos)
{
	ClearFocus(EAIFocusPriority::Gameplay);
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
	EPathFollowingRequestResult::Type result = MoveToLocation(pos - (enemyPos - pos).GetSafeNormal() * FleeRadius);
	if (result == EPathFollowingRequestResult::Failed)
	{
		LOGI("Path planning failed, so doing a random patrol.");
		// Move sorta away, lol
		DoRandomPatrol(GetWorld()->GetTimeSeconds());
	}
	else
	{
		LOGI("Path planning success.");
	}
}

bool ATownieAIController::MaybeAttackEnemy(float dist)
{
	if (IsPanicking())
	{
		return false;
	}
	bool hasGun = HasGun();
	if (hasGun && dist < ShootingRange)
	{
		ShootAtEnemy();
		timeLastAttacked = GetWorld()->GetTimeSeconds();
		return true;
	}
	else if (hasGun && dist < ShootingRange * 2)
	{
		GetTownCharacter()->IsAiming = true;
		return false;
	}
	else if (HasMeleeWeapon() &&
		dist < GetTownCharacter()->MeleeAttackRange)
	{
		MeleeAttackEnemy();
		timeLastAttacked = GetWorld()->GetTimeSeconds();
		return true;
	}
	return false;
}

void ATownieAIController::ManagePanicWhenNearEnemy(float dt)
{
	if (fright > FrightToCausePanic && !IsPanicking())
	{
		Panic();
	}
	else
	{
		fright += FrightGrowthNearEnemy * dt;
	}
}

void ATownieAIController::ReduceFright(float dt)
{
	if (fright > 0)
	{
		fright -= FrightShrinkWhenFarFromEnemy * dt;
	}
}

void ATownieAIController::RespondToDistantEnemey()
{
	ClearFocus(EAIFocusPriority::Gameplay);
	GetTownCharacter()->IsPanicking = false;
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
	GetTownCharacter()->IsAiming = false;
}

void ATownieAIController::HandleAggression(float dt)
{
	aggression += dt * AggressionGrowthPerSecond;
	if (aggression > AggressionToCauseAttack)
	{
		aggression = AggressionToCauseAttack;
	}
}

void ATownieAIController::ReduceAggression(float dt)
{
	aggression -= dt * AggressionGrowthPerSecond * 0.5f;
	if (aggression < 0.0f)
	{
		aggression = 0.0f;
	}
}

void ATownieAIController::Tick(float dt)
{
	Super::Tick(dt);

	if (!GetTownCharacter())
	{
		LOGE("Not attached to town character.");
		return;
	}

	if (GetTownCharacter()->GetIsDead())
	{
		return;
	}

	if (GetTownCharacter()->GameMode)
	{
		if (GetTownCharacter()->GameMode->IsPausedForScriptedEvent)
		{
			StopMovement();
			return;
		}
	}


	float time = GetWorld()->GetTimeSeconds();
	timeSinceLastPanicked = time - timeLastPanicked;
	GetTownCharacter()->IsScared = timeLastPanicked > 0 && (timeSinceLastPanicked < RemainScaredFor);

	if (IsReadyToAttack())
	{
		timeLastReadyToAttack = time;
	}
	GetTownCharacter()->IsReadyToAttack = IsReadyToAttackHysteresis(AttackFollowthrough);

	if (time - timeLastAttacked < AttackFollowthrough)
	{
		// Pause after each attack.
		IdleBehavior(time);
	}
	else if (nearestEnemyActor && !nearestEnemyActor->GetIsDead())
	{
		// An enemy is near.
		RespondToEnemy(time, dt);
	}
	else
	{
		GetTownCharacter()->IsAiming = IsReadyToAttackHysteresis(AttackFollowthrough);
		// Idle Behavior.
		IdleBehavior(time);
		fright = 0;
	}
}


bool ATownieAIController::IsPanicking() const
{
	return panicTimer > 0.0;
}

bool ATownieAIController::HasGun() const
{
	return GetTownCharacter()->GetHasGun();
}

bool ATownieAIController::HasMeleeWeapon() const
{
	return GetTownCharacter()->GetHasMeleeWeapon();
}

void ATownieAIController::DoRandomPatrol(float time)
{
	lastPatrolTime = time;
	randomPatrolTarget = FVector(FMath::FRandRange(-RandomPatrolRadius, RandomPatrolRadius), 
								 FMath::FRandRange(-RandomPatrolRadius, RandomPatrolRadius), 
								 0) + GetCharacter()->GetActorLocation();
	if ((HasGun() || HasMeleeWeapon()) && 
		(nearestEnemyActor != nullptr && !nearestEnemyActor->GetIsDead()))
	{
		randomPatrolTarget = nearestEnemyActor->GetActorLocation();
	}
	MoveToLocation(randomPatrolTarget);
	DrawDebugLine(GetWorld(), GetCharacter()->GetActorLocation(), randomPatrolTarget, FColor::Yellow, false, 5.0f);
}

void ATownieAIController::IdleBehavior(float time)
{
	ClearFocus(EAIFocusPriority::Gameplay);
	GetTownCharacter()->IsPanicking = false;
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;

	if (EnableRandomPatrol && (time - lastPatrolTime > RandomPatrolTime))
	{
		DoRandomPatrol(time);
	}
}

bool ATownieAIController::GetIsDead() const
{
	ATownCharacter* character = GetTownCharacter();
	if (!character)
	{
		return false;
	}
	return character->GetIsDead();
}

ATownCharacter* ATownieAIController::BecomeZombie()
{
	ATownCharacter* character = GetTownCharacter();
	if (!character)
	{
		return nullptr;
	}
	return character->BecomeZombie();
}

void ATownieAIController::Panic()
{
	ATownCharacter* character = GetTownCharacter();
	if (!character)
	{
		return;
	}
	character->IsPanicking = true;
	timeLastPanicked = GetWorld()->GetTimeSeconds();
	panicTimer = 0;
	timeSinceLastPanicked = 0;
	// Reset fright -- it will grow if an enemy comes near again.
	fright = 0;
}

void ATownieAIController::SearchNearestEnemyActor()
{
	ATownCharacter* character = GetTownCharacter();
	if (!character)
	{
		return;
	}
	if (!character)
	{
		LOGW("No Character...");
		return;
	}
	TArray<AZombieAIController*> zombies;
	character->GetControllersInRadius<AZombieAIController>(EnemyVisibilityRadius, zombies);
	float closestDist = std::numeric_limits<float>::max();
	if (nearestEnemyActor)
	{
		closestDist = FVector::DistSquared(nearestEnemyActor->GetActorLocation(), character->GetActorLocation());
	}
	for (const auto* zombie : zombies)
	{
		if (zombie->GetIsDead())
		{
			continue;
		}

		float dist = FVector::DistSquared(zombie->GetCharacter()->GetActorLocation(), character->GetActorLocation());
		if (dist < closestDist)
		{
			nearestEnemyActor = zombie->GetTownCharacter();
			closestDist = dist;
		}
	}
}

bool ATownieAIController::CastShootingRay(const FVector& target, FHitResult& outHit)
{
	FVector origin = GetCharacter()->GetActorLocation() + FVector::UpVector * 50;

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetCharacter());
	return GetWorld()->LineTraceSingleByChannel(outHit, origin, target, ECollisionChannel::ECC_Visibility, params);
}

void ATownieAIController::MeleeAttackEnemy()
{
	if (!GetTownCharacter())
	{
		return;
	}
	if (!nearestEnemyActor)
	{
		return;
	}
	aggression = 0.0f;
	UMeleeWeapon* melee = GetTownCharacter()->GetMeleeWeaponOrNull();

	if (melee && nearestEnemyActor)
	{
		if (GetTownCharacter()->AttackWithCooldown(melee->AttackRate))
		{
			melee->DoAttack(nearestEnemyActor);
			melee->OnAttack(nearestEnemyActor->GetActorLocation());
		}
	}

}

void ATownieAIController::ShootAtEnemy()
{
	if (!GetTownCharacter())
	{
		return;
	}
	GetTownCharacter()->IsAiming = true;

	FVector hitPoint;
	FRotator hitRotation;
	FHitResult hitResult;
	FVector rayEnd;
	if (!nearestEnemyActor)
	{
		rayEnd = GetCharacter()->GetActorLocation() + GetCharacter()->GetActorRotation().RotateVector(FVector3d::ForwardVector);

	}
	else
	{
		rayEnd = nearestEnemyActor->GetActorLocation() + FVector::UpVector * 50;
	}
	bool hit = CastShootingRay(rayEnd, hitResult);
	if (hit)
	{
		hitPoint = hitResult.ImpactPoint;
		hitRotation = (hitResult.ImpactNormal).Rotation();
	}
	else
	{
		hitPoint = rayEnd;
		hitRotation = GetCharacter()->GetActorRotation();
	}
	float time = GetWorld()->GetTimeSeconds();

	if (time - timeStartedShooting > ShootSpeed * 2)
	{
		timeStartedShooting = time;
	}
	if (time - timeStartedShooting < ShootSpeed)
	{
		return;
	}
	if (!GetTownCharacter()->ShootWithCoolDown(ShootSpeed, hitPoint, hitRotation))
	{
		return;
	}
	aggression = 0.0f;
	UGunComponent* gun = GetTownCharacter()->GetGunOrNull();

	if (gun && nearestEnemyActor)
	{
		FDamageEvent damage;
		nearestEnemyActor->TakeDamage(gun->Damage, damage, this, GetCharacter());
	}

}