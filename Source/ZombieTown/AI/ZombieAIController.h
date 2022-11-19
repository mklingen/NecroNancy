// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SeqComp.h"
#include "ZombieAIController.generated.h"

class ATownCharacter;
class AZombieInteractionPoint;
class ITargetableInterface;

UCLASS()
class ZOMBIETOWN_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
public:
	AZombieAIController();
	virtual void Tick(float dt) override;
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FollowSummonerRadius = 150.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float StopFollowSummonerRadius = 2500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float EnemyVisibilityRadius = 1500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float SearchForEnemiesEverySeconds = 2.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackCooldownSeconds = 1.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackDamage = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Knockback = 200.0f;

	UFUNCTION(BlueprintCallable)
		bool GetIsDead() const;

	void SendTowardThisFrame(const FVector& location);
	void SummonTowardThisFrame(AActor* actor);
	void MoveTowardEnemyThisFrame(ATownCharacter* actor);
	void MoveTowardTargetThisFrame(AActor* actor);
	void SearchNearestEnemyActor();

	void DamageActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
		ATownCharacter* GetTownCharacter() const
	{
		return townCharacter;
	}

	UFUNCTION(BlueprintCallable)
		bool IsGamePaused() const;

	UFUNCTION(BlueprintCallable)
		void TargetActor(AActor* target);

	UFUNCTION(BlueprintCallable)
		void TargetInterface(AActor* target, TScriptInterface<ITargetableInterface> targetableInterface);

private:
	UPROPERTY()
		AActor* summonTargetThisFrame = nullptr;
	UPROPERTY()
		AActor* followingActor = nullptr;
	UPROPERTY()
		ATownCharacter* nearestEnemyActor = nullptr;
	UPROPERTY()
		AActor* targetedActor = nullptr;
	UPROPERTY()
		FVector ptOnTargetedActor;
	UPROPERTY()
		TScriptInterface<ITargetableInterface> interfaceTargeted;
	UPROPERTY()
		ATownCharacter* townCharacter = nullptr;
	UPROPERTY()
		ATownCharacter* player = nullptr;
	UPROPERTY()
		AZombieInteractionPoint* attachedInteractionPoint = nullptr;
	
	void TickSeqComp(float dt);
	void TickOnDead(float dt);
	void TickOnPaused(float dt);
	bool TickIsAttachedDomain(float dt);
	void TickOnAttached(float dt);
	void TickOnSummoning(float dt);
	void TickOnSending(float dt);
	void TickOnTargeting(float dt);
	void TickOnFollowingEnemy(float dt);
	void TickOnFollowingAlly(float dt);
	void MaybeAvoidPlayer();
	void StopTargeting();
	void DetachFromInteractionPoint();

	seqcomp::SeqComp seqComp;
	FVector sendLocation;
	bool isSendingThisFrame = false;
	FTimerHandle searchEnemiesHandle;
};
