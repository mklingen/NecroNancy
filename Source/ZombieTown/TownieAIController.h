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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fright")
		float PanicCooldown = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Visibility")
		float EnemyVisibilityRadius = 600.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fright")
		float FleeRadius = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fright")
		float RemainScaredFor = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float ShootingRange = 550.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float FleeWhileAimingDist = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooting")
		float ShootSpeed = 1.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Melee")
		bool isPreparingToMelee = false;

	ATownCharacter* GetTownCharacter() const;
	void SearchNearestEnemyActor();

protected:
	float timeLastPanicked = -100;
	FTimerHandle searchEnemiesHandle;
	ATownCharacter* nearestEnemyActor = nullptr;

	bool CastShootingRay(const FVector& target, FHitResult& outHit);
};
