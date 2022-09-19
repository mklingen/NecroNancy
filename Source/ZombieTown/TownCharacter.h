// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

#include "TownCharacter.generated.h"

class UHealthComponent;
class AController;
class UZombifier;
class UToolComponent;
class UGunComponent;
class AZombieTownGameModeBase;
class UFlasherComponent;
class UMeleeWeapon;

UCLASS()
class ZOMBIETOWN_API ATownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATownCharacter();
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float timeOfLastAttack = 0;
	float timeOfLastShot = 0;

	UPROPERTY(VisibleAnywhere)
		UHealthComponent* health = nullptr;

	UPROPERTY(VisibleAnywhere)
		UZombifier* zombifier = nullptr;

	UPROPERTY(VisibleAnywhere)
		UToolComponent* toolComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UFlasherComponent* flasherComponent = nullptr;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ShouldAttachToolOnStartup = true;

	UFUNCTION(BlueprintCallable)
		void AttachTool();

	UFUNCTION(BlueprintCallable)
		bool ShootWithCoolDown(float cooldown, const FVector& hitPoint, const FRotator& hitRotation);
	UFUNCTION(BlueprintCallable)
		bool AttackWithCooldown(float cooldown);
	UFUNCTION(BlueprintCallable)
		void Attack();

	UFUNCTION(BlueprintCallable)
		void Shoot(const FVector& hitPoint, const FRotator& hitRotation);

	UFUNCTION(BlueprintCallable)
		bool GetIsDead() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AttackedRecently = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ShotRecently = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MeleeAttackRange = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsPanicking = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsScared = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsAiming = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsReadyToAttack = false;

	UFUNCTION(BlueprintCallable)
		ATownCharacter* BecomeZombie();

	UFUNCTION(BlueprintCallable)
		bool GetHasGun() const;

	UFUNCTION(BlueprintCallable)
		bool GetHasMeleeWeapon() const;

	UFUNCTION(BlueprintCallable)
		UMeleeWeapon* GetMeleeWeaponOrNull() const;

	UFUNCTION(BlueprintCallable)
		bool GetHasAttachedTool() const;

	UFUNCTION(BlueprintCallable)
		UGunComponent* GetGunOrNull() const;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AZombieTownGameModeBase* GameMode = nullptr;

	template <typename TActor>
	bool GetActorsInRadius(float radius, TArray<TActor*>& actorsOut)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> summonTypes;
		UClass* summonClassFilter = TActor::StaticClass();
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(this);
		TArray<AActor*> overlapActors;
		if (!UKismetSystemLibrary::SphereOverlapActors(this,
			GetActorLocation(), radius,
			summonTypes, summonClassFilter, ignoreActors, overlapActors))
		{
			return false;
		}
		for (AActor* actor : overlapActors)
		{
			TActor* castActor = Cast<TActor>(actor);
			if (castActor)
			{
				actorsOut.Add(castActor);
			}
		}
		return actorsOut.Num() > 0;
	}

	template <typename TComponent>
	bool GetComponentsInRadius(float radius, TArray<TComponent*>& componentsOut)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> summonTypes;
		UClass* summonClassFilter = AActor::StaticClass();
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(this);
		TArray<AActor*> overlapActors;

		if (!UKismetSystemLibrary::SphereOverlapActors(this,
			GetActorLocation(), radius,
			summonTypes, summonClassFilter, ignoreActors, overlapActors))
		{
			return false;
		}

		for (AActor* actor : overlapActors)
		{
			TComponent* component = actor->FindComponentByClass<TComponent>();
			if (!component)
			{
				continue;
			}
			componentsOut.Add(component);
		}
		return componentsOut.Num() > 0;
	}

	template <typename TController>
	bool GetControllersInRadius(float radius, TArray<TController*>& controllersOut)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> summonTypes;
		UClass* summonClassFilter = ATownCharacter::StaticClass();
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(this);
		TArray<AActor*> overlapActors;

		if (!UKismetSystemLibrary::SphereOverlapActors(this,
			GetActorLocation(), radius,
			summonTypes, summonClassFilter, ignoreActors, overlapActors))
		{
			return false;
		}

		for (AActor* actor : overlapActors)
		{
			ATownCharacter* townCharacter = Cast<ATownCharacter>(actor);
			if (!townCharacter)
			{
				continue;
			}

			TController* controller = Cast<TController>(townCharacter->GetController());
			if (!controller)
			{
				continue;
			}
			controllersOut.Add(controller);
		}
		return controllersOut.Num() > 0;
	}
};
