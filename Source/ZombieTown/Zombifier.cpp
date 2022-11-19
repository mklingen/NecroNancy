// Fill out your copyright notice in the Description page of Project Settings.


#include "Zombifier.h"

#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/Helpers/PrintHelper.h"

UZombifier::UZombifier()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


void UZombifier::BeginPlay()
{
	Super::BeginPlay();
}

ATownCharacter* UZombifier::SpawnZombie(ATownCharacter* current)
{
	FActorSpawnParameters spawnParams;
	spawnParams.bNoFail = true;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FTransform& transform = GetOwner()->GetActorTransform();
	ATownCharacter* townCharacter = GetWorld()->SpawnActor<ATownCharacter>(CharacterPrefabClass, transform, spawnParams);
	if (!townCharacter)
	{
		LOGE("Couldn't convert to town character %s", *GetOwner()->GetActorNameOrLabel());
		return nullptr;
	}
	USkeletalMeshComponent* currentMesh = current->GetMesh();
	USkeletalMeshComponent* spawnMesh = townCharacter->GetMesh();
	if (currentMesh && spawnMesh)
	{
		int num_materials_spawned = spawnMesh->SkeletalMesh->GetMaterials().Num();
		TArray<FSkeletalMaterial> prev_materials = spawnMesh->SkeletalMesh->GetMaterials();
		spawnMesh->SetSkeletalMesh(currentMesh->SkeletalMesh, true);
		spawnMesh->SkeletalMesh->SetMaterials(prev_materials);
	}
	return townCharacter;
}


