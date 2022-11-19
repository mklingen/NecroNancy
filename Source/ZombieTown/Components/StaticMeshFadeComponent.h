// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaticMeshFadeComponent.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UCurveFloat;

enum class EFadeState : uint8
{
	FadingIn,
	FadingOut,
	Invisible,
	Visible
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UStaticMeshFadeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStaticMeshFadeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	EFadeState State;

	float timeStateChanged = 0;
	bool stateChangeComplete = false;

	void StartFadingOut();
	void StartFadingIn();
	void BecomeInvisible();
	void BecomeVisible();

	UStaticMeshComponent* mesh = nullptr;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TArray<UMaterialInterface*> origMaterial;

	UPROPERTY(VisibleAnywhere)
		UMaterialInstanceDynamic* fadeMaterial = nullptr;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool ShouldBeVisible = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCurveFloat* FadeCurve = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FadeTime = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMaterialInterface* FadeMaterialBase = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float CurrentAlpha = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float CurrentT = 0;


};
