// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UsableObject.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UUsableObject : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUsableObject();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnUse(AActor* user);
};
