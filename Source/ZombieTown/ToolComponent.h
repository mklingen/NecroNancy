// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ToolComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UToolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UToolComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AActor* spawnedTool = nullptr;

public:

	bool AttachToMesh(USkeletalMeshComponent* mesh);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> ToolActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName ToolSocketName;

	UFUNCTION()
		AActor* GetSpawnedTool() const;
};
