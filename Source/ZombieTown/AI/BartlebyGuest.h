// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BartlebyGuest.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API UBartlebyGuest : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBartlebyGuest();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
