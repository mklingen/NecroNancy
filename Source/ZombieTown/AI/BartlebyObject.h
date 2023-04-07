// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZombieTown/AI/BartlebySystem.h"
#include "ZombieTown/AI/BartlebyRoom.h"
#include "BartlebyObject.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API UBartlebyObject : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBartlebyObject();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Id;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description;
	UPROPERTY()
		class ABartlebySystem* System = nullptr;

};
