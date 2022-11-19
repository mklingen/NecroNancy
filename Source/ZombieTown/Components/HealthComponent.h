// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();
	
	DECLARE_DYNAMIC_DELEGATE(FDamagedCallback);
	DECLARE_DYNAMIC_DELEGATE(FHealedCallback);
	DECLARE_DYNAMIC_DELEGATE(FDiedCallback);
	DECLARE_MULTICAST_DELEGATE(FDiedBroadcastCallback);
	DECLARE_MULTICAST_DELEGATE(FHealedBroadcastCallback);
	DECLARE_MULTICAST_DELEGATE(FDamagedBroadcastCallback);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		float Health = 100.0f;


	FDamagedCallback damagedCallback;
	FHealedCallback healedCallback;
	FDiedCallback diedCallback;
	FDiedBroadcastCallback diedBroadcast;
	FDamagedBroadcastCallback damagedBroadcast;
	FHealedBroadcastCallback healedBroadcast;

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigatedBy,
		AActor* DamageCauser);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool DamageChildActors = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool DestroyActorOnDeath = false;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "callback"))
	void SetDamagedCallback(const FDamagedCallback& callback) {
		damagedCallback = callback;
	}

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "callback"))
	void SetHealedCallback(const FHealedCallback& callback) {
		healedCallback = callback;
	}

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "callback"))
	void SetDiedCallback(const FDiedCallback& callback) {
		diedCallback = callback;
	}

	bool IsDead() const
	{
		return Health <= 0;
	}

	void Damage(float amount)
	{
		Health -= amount;
		Health = FMath::Max(Health, 0);
		damagedCallback.ExecuteIfBound();
		damagedBroadcast.Broadcast();
		if (IsDead())
		{
			diedCallback.ExecuteIfBound();
			diedBroadcast.Broadcast();
			if (DestroyActorOnDeath)
			{
				GetOwner()->Destroy();
			}
		}
	}

	void Heal(float amount)
	{
		Health += amount;
		Health = FMath::Min(Health, MaxHealth);
		healedCallback.ExecuteIfBound();
		healedBroadcast.Broadcast();
	}

	FDiedBroadcastCallback& GetDiedBroadcast() {
		return diedBroadcast;
	}

	FHealedBroadcastCallback& GetHealedBroadcast() {
		return healedBroadcast;
	}

	FDamagedBroadcastCallback& GetDamagedBroadcast() {
		return damagedBroadcast;
	}

};
