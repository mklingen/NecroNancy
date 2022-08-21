// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Zombifier.generated.h"

class ATownCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UZombifier : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UZombifier();

	UFUNCTION(BlueprintCallable)
		ATownCharacter* SpawnZombie(ATownCharacter* current);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ATownCharacter> CharacterPrefabClass = nullptr;
};
