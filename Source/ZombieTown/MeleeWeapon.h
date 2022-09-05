// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeWeapon.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API UMeleeWeapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeleeWeapon();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Damage = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AttackRate = 2.0f;

	UFUNCTION()
		void DoAttack(AActor* enemy);

	UFUNCTION()
		void OnAttack(const FVector& hitPoint);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
