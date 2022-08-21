// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawnpoint.generated.h"

class ATownCharacter;

UCLASS()
class ZOMBIETOWN_API AZombieSpawnpoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AZombieSpawnpoint();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool DestroyOnSpawn = true;

	DECLARE_DYNAMIC_DELEGATE_OneParam(FSpawnedEvent, ATownCharacter*, SpawnedZombie);

	UFUNCTION(BlueprintCallable)
		ATownCharacter* SpawnZombie();

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "spawnCallback"))
		void BindSpawnCallback(FSpawnedEvent spawnCallback);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsActive = true;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ATownCharacter> CharacterPrefabClass = nullptr;

	FSpawnedEvent spawnCallback;

};
