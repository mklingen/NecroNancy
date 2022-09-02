// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintHelpers.generated.h"

UCLASS()
class ZOMBIETOWN_API UBlueprintHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// Silly function needed to determine if all the zombies have been spawned (this is used for the graveyard).
	UFUNCTION(BlueprintCallable)
	static bool AreAllZombieSpawnersDone(ULevel* level);
};
