// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		float Health = 100.0f;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxHealth = 100.0f;

	bool IsDead() const
	{
		return Health <= 0;
	}

	void Damage(float amount)
	{
		Health -= amount;
		Health = FMath::Max(Health, 0);
	}

	void Heal(float amount)
	{
		Health += amount;
		Health = FMath::Min(Health, MaxHealth);
	}

};
