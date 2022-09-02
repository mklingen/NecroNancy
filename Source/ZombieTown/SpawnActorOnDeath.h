// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnActorOnDeath.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API USpawnActorOnDeath : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnActorOnDeath();


	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> SpawnActorClass = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
