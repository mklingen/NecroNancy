// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlasherComponent.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API UFlasherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlasherComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* staticMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* skeletalMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<UMaterialInterface*> origMaterial;

	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic* flashMaterial = nullptr;

	void StopFlashing();
	void TurnOn();
	void TurnOff();

	float timeStartedFlashing = -1.0f;
	bool isFlashing = false;

	UFUNCTION()
		void OnTakeAnyDamage(AActor* DamagedActor,
			float Damage,
			const class UDamageType* DamageType,
			class AController* InstigatedBy,
			AActor* DamageCauser);

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor FlashColor = FLinearColor(FVector4f(0.99f, 0.3f, 0.2f, 1.0));
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FlashTime = 0.25f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FlashRateHz = 8.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool FlashOnDamage = true;

	UFUNCTION(BlueprintCallable)
		void Flash();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInterface* FlashMaterialBase = nullptr;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
