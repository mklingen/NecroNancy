// Fill out your copyright notice in the Description page of Project Settings.


#include "TownieAIController.h"

#include "ZombieAIController.h"
#include "TownCharacter.h"
#include "GunComponent.h"
#include "PrintHelper.h"
#include "ZombieTownGameModeBase.h"
#include "MeleeWeapon.h"
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
	PanicTime = PanicTime + FMath::FRand();
}

void ATownieAIController::RespondToEnemy(float time, float dt)
{
	SetFocus(nearestEnemyActor);
	FVector pos = GetCharacter()->GetActorLocation();
	FVector enemyPos = nearestEnemyActor->GetActorLocation();
	float dist = (pos - enemyPos).Length();
	if (dist > EnemyVisibilityRadius)
	{
		RespondToDistantEnemey();
		ReduceFright(dt);
	}
	else
	{
		ManagePanicWhenNearEnemy(dt);
		if (IsPanicking())
		{
			return;
		}
		MaybeAttackEnemy(dist);
		bool isAiming = GetTownCharacter()->IsAiming;
		if (!isAiming)
		{
			FleeEnemy(pos, enemyPos);
		}
		else
		{
			AimAtEnemy(isAiming, dist, enemyPos, pos);
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
		MoveToLocation(pos - (enemyPos - pos).GetSafeNormal() * FleeRadius);
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
		// Move sorta away, lol
		GetCharacter()->GetCharacterMovement()->AddInputVector((enemyPos - pos) * 1000);
	}
}

bool ATownieAIController::MaybeAttackEnemy(float dist)
{
	if (IsPanicking())
	{
		return false;
	}

	if (HasGun() &&
		dist < ShootingRange)
	{
		ShootAtEnemy();
		return true;
	}
	else if (HasMeleeWeapon() &&
		dist < GetTownCharacter()->MeleeAttackRange)
	{
		MeleeAttackEnemy();
		return true;
	}

	GetTownCharacter()->IsAiming = false;
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
	GetTownCharacter()->IsScared = timeSinceLastPanicked < RemainScaredFor;

	if (nearestEnemyActor && !nearestEnemyActor->GetIsDead())
	{
		// An enemy is near.
		RespondToEnemy(time, dt);
	}
	else
	{
		// Idle Behavior.
		IdleBehavior();
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

void ATownieAIController::IdleBehavior()
{
	GetTownCharacter()->IsAiming = false;
	ClearFocus(EAIFocusPriority::Gameplay);
	GetTownCharacter()->IsPanicking = false;
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
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
	DrawDebugLine(GetWorld(), origin, target, FColor::Yellow, false);
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
		DrawDebugLine(GetWorld(), GetTownCharacter()->GetActorLocation(), hitPoint, FColor::Green);
	}
	else
	{
		hitPoint = rayEnd;
		hitRotation = GetCharacter()->GetActorRotation();
		DrawDebugLine(GetWorld(), GetTownCharacter()->GetActorLocation(), hitPoint, FColor::Red);
	}
	if (!GetTownCharacter()->ShootWithCoolDown(ShootSpeed, hitPoint, hitRotation))
	{
		return;
	}
	UGunComponent* gun = GetTownCharacter()->GetGunOrNull();

	if (gun && nearestEnemyActor)
	{
		FDamageEvent damage;
		nearestEnemyActor->TakeDamage(gun->Damage, damage, this, GetCharacter());
	}

}