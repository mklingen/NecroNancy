// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TownieAIController.generated.h"

class ATownCharacter;

UCLASS()
class ZOMBIETOWN_API ATownieAIController : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		bool GetIsDead() const;
	virtual void Tick(float dt) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		ATownCharacter* BecomeZombie();

	UFUNCTION(BlueprintCallable)
		void Panic();


	UFUNCTION(BlueprintCallable)
		void MeleeAttackEnemy();


	UFUNCTION(BlueprintCallable)
		void ShootAtEnemy();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visibility")
		float SearchForEnemiesEverySeconds = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fright")
		float PanicTime = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visibility")
		float EnemyVisibilityRadius = 600.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Fright")
		float FleeRadius = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Fright")
		float RemainScaredFor = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Fright")
		float FrightGrowthNearEnemy = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Fright")
		float FrightToCausePanic = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Fright")
		float FrightShrinkWhenFarFromEnemy = 0.25f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float ShootingRange = 550.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float FleeWhileAimingDist = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float ShootSpeed = 1.5f;

	ATownCharacter* GetTownCharacter() const;
	void SearchNearestEnemyActor();

	UFUNCTION(BlueprintCallable)
		bool IsPanicking() const;
	UFUNCTION(BlueprintCallable)
		bool HasGun() const;
	UFUNCTION(BlueprintCallable)
		bool HasMeleeWeapon() const;

protected:
	FTimerHandle searchEnemiesHandle;
	ATownCharacter* nearestEnemyActor = nullptr;


	void RespondToEnemy(float time, float dt);
	void AimAtEnemy(bool isAiming, float dist, FVector& enemyPos, FVector& pos);
	void FleeEnemy(FVector& pos, FVector& enemyPos);
	bool MaybeAttackEnemy(float dist);
	void ManagePanicWhenNearEnemy(float dt);
	void RespondToDistantEnemey();
	void IdleBehavior();
	bool CastShootingRay(const FVector& target, FHitResult& outHit);
	void ReduceFright(float dt);

	// Controls how likely the character is to panic. Panicking
	// reduces fright, as does fleeing. Being near an enemy increases fright.
	float fright = 0;
	// Timer that gets set to PanicTime when we start panicking.
	float panicTimer = 0;
	float timeLastPanicked = 0;
	float timeSinceLastPanicked = 0;

};
