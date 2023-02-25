// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEntrance.generated.h"

UCLASS()
class ZOMBIETOWN_API ALevelEntrance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelEntrance();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool TeleportPlayer = true;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool TeleportZombies = true;

	// This function is called when a player enters a level through a level entrance
	UFUNCTION(BlueprintCallable)
		void EnterLevel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
