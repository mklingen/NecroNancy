// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunComponent.generated.h"

class UNiagaraSystem;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UStaticMeshComponent* Mesh = nullptr;

	float timeLastShot = 0;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Shoot(const FVector& hitPoint, const FRotator& hitRotation);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName MuzzleSocketName = "muzzleSocket";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Damage = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UNiagaraSystem* MuzzleFlashParticles = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UNiagaraSystem* HitParticles = nullptr;
};
