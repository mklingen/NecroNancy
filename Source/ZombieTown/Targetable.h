// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetableInterface.h"
#include "Targetable.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API UTargetable : public UActorComponent, public ITargetableInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetable();

	virtual FTargetInfo GetTargetInfo(const FVector& targetedFrom) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};
