// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "ZombieTown/AI/TownieAIController.h"
#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZombieTown/Helpers/PrintHelper.h"
#include "ZombieTown/Helpers/DebugDrawHelpers.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ZombieTown/ZombieTownGameModeBase.h"
#include "ZombieTown/Actors/ZombieInteractionPoint.h"
#include "ZombieTown/Helpers/ActorHelpers.h"
#include "ZombieTown/Interfaces/TargetableInterface.h"

AZombieAIController::AZombieAIController()
{
	using namespace seqcomp;
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return TickIsAttachedDomain(0.0f); },
			[this](float _dt) { TickOnAttached(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return summonTargetThisFrame != nullptr; },
			[this](float _dt) { TickOnSummoning(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return isSendingThisFrame; },
			[this](float _dt) { TickOnSending(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return targetedActor != nullptr; },
			[this](float _dt) { TickOnTargeting(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return nearestEnemyActor != nullptr; },
			[this](float _dt) { TickOnFollowingEnemy(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return nearestEnemyActor != nullptr; },
			[this](float _dt) { TickOnFollowingEnemy(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return followingActor != nullptr; },
			[this](float _dt) { TickOnFollowingAlly(_dt); }
		)
	);
	seqComp.AddElement(
		SeqCompElement(
			[this]() { return true; },
			[this](float _dt)
			{
					StopMovement();
					MaybeAvoidPlayer();
			}
		)
	);
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();
	townCharacter = Cast<ATownCharacter>(GetCharacter());
	GetWorld()->GetTimerManager().SetTimer(searchEnemiesHandle, this, &AZombieAIController::SearchNearestEnemyActor, SearchForEnemiesEverySeconds + FMath::FRand(), true);
	PrimaryActorTick.bCanEverTick = true;
	player = Cast<ATownCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void AZombieAIController::DetachFromInteractionPoint()
{
	SAY_W("Detached");
	StopTargeting();
	attachedInteractionPoint->Detach();
	attachedInteractionPoint = nullptr;
}

void AZombieAIController::TickOnDead(float dt)
{
	if (attachedInteractionPoint &&
		attachedInteractionPoint->GetAttachedZombieOrNull() == GetCharacter()) {
		DetachFromInteractionPoint();
	}
}

void AZombieAIController::TickOnPaused(float dt)
{
	StopMovement();
}

bool AZombieAIController::TickIsAttachedDomain(float dt)
{
	if (attachedInteractionPoint &&
		(attachedInteractionPoint->GetAttachedZombieOrNull() != GetCharacter() ||
			!attachedInteractionPoint->IsActive)) {
		DetachFromInteractionPoint();
	}

	if (attachedInteractionPoint != nullptr && !summonTargetThisFrame)
	{
		GetCharacter()->SetActorTransform(attachedInteractionPoint->GetActorTransform());
		return true;
	}
	return false;
}

void AZombieAIController::TickOnAttached(float dt)
{
	GetCharacter()->SetActorTransform(attachedInteractionPoint->GetActorTransform());
}

void AZombieAIController::TickOnSummoning(float dt)
{
	MoveToActor(summonTargetThisFrame);
	if (followingActor != summonTargetThisFrame)
	{
		followingActor = summonTargetThisFrame;
	}
	summonTargetThisFrame = nullptr;
	if (attachedInteractionPoint != nullptr) {
		DetachFromInteractionPoint();
	}
	StopTargeting();
}

void AZombieAIController::TickOnSending(float dt)
{
	if (targetedActor) 
	{
		MoveTowardTargetThisFrame(targetedActor);
	}
	else 
	{
		MoveToLocation(sendLocation);
	}
	isSendingThisFrame = false;
}

void AZombieAIController::TickOnTargeting(float dt)
{
	UHealthComponent* health = targetedActor->FindComponentByClass<UHealthComponent>();
	if (health && health->IsDead())
	{
		StopTargeting();
	}
	else
	{
		SetFocus(targetedActor);
		MoveTowardTargetThisFrame(targetedActor);
		FVector targetPoint;
		float dist = FVector::Dist(ptOnTargetedActor, townCharacter->GetActorLocation());

		if (townCharacter)
		{
			AZombieInteractionPoint* pt =
				Cast<AZombieInteractionPoint>(targetedActor);
			if (pt && dist < 150)
			{
				if (pt->GetAttachedZombieOrNull())
				{
					SAY_W("Tried to attach, but something is already attached?");
					StopTargeting();
				}
				else
				{
					SAY_W("Attached");
					pt->Attach(GetCharacter());
					attachedInteractionPoint = pt;
					StopTargeting();
				}
			}
			else if (dist < townCharacter->MeleeAttackRange &&
				townCharacter->AttackWithCooldown(AttackCooldownSeconds))
			{
				DamageActor(targetedActor);
			}
		}
	}
}

void AZombieAIController::TickOnFollowingEnemy(float dt)
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

void  AZombieAIController::TickOnFollowingAlly(float dt)
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

void AZombieAIController::TickSeqComp(float dt)
{
	seqComp.Tick(dt);
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
		TickOnDead(dt);
		return;
	}
	if (IsGamePaused())
	{
		TickOnPaused(dt);
		return;
	}

	TickSeqComp(dt);
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
	TargetActor(actor);
	MoveTo(ptOnTargetedActor);
}

void AZombieAIController::StopTargeting()
{
	if (interfaceTargeted && targetedActor)
	{
		interfaceTargeted->OnTargetReleased(GetCharacter(), targetedActor);
	}
	interfaceTargeted = nullptr;
	targetedActor = nullptr;
}


void AZombieAIController::TargetActor(AActor* target)
{
	StopTargeting();
	followingActor = nullptr;
	nearestEnemyActor = nullptr;
	targetedActor = target;
	UActorHelpers::DistanceToActor(targetedActor,
		townCharacter->GetActorLocation(), ECollisionChannel::ECC_WorldDynamic, ptOnTargetedActor);
}

void AZombieAIController::TargetInterface(AActor* target, TScriptInterface<ITargetableInterface> targetableInterface)
{
	TargetActor(target);
	interfaceTargeted = targetableInterface;
	interfaceTargeted->OnTargeted(GetCharacter(), target);
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