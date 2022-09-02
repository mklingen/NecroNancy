// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSpawnpoint.h"

#include "TownCharacter.h"
#include "PrintHelper.h"

// Sets default values
AZombieSpawnpoint::AZombieSpawnpoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AZombieSpawnpoint::BeginPlay()
{
	Super::BeginPlay();

}

ATownCharacter* AZombieSpawnpoint::SpawnZombie()
{
	if (!this->IsActive)
	{
		return nullptr;
	}
	FActorSpawnParameters spawnParams;
	spawnParams.bNoFail = true;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FTransform& transform = GetActorTransform();
	ATownCharacter* townCharacter = GetWorld()->SpawnActor<ATownCharacter>(CharacterPrefabClass, transform, spawnParams);
	if (!townCharacter)
	{
		LOGE("Couldn't convert to town character %s", *GetActorNameOrLabel());
		return nullptr;
	}
	HasEverSpawned = true;
	spawnCallback.ExecuteIfBound(townCharacter);
	if (DestroyOnSpawn)
	{
		Destroy();
	}
	return townCharacter;
}

void AZombieSpawnpoint::BindSpawnCallback(FSpawnedEvent callback)
{
	spawnCallback = callback;
}
