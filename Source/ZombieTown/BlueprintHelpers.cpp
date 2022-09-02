// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintHelpers.h"

#include "ZombieSpawnpoint.h"

bool UBlueprintHelpers::AreAllZombieSpawnersDone(ULevel* level) {
	if (!level) {
		return false;
	}
	for (const AActor* actor : level->Actors)
	{
		const AZombieSpawnpoint* spawn = Cast<AZombieSpawnpoint>(actor);
		if (spawn && (spawn->IsActive && !spawn->HasEverSpawned))
		{
			return false;
		}
	}
	return true;
}