// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnActorOnDeath.h"

#include "Zombietown/Components/HealthComponent.h"
#include "ZombieTown/Helpers/PrintHelper.h"

// Sets default values for this component's properties
USpawnActorOnDeath::USpawnActorOnDeath()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void USpawnActorOnDeath::BeginPlay()
{
	Super::BeginPlay();
	UHealthComponent* health = GetOwner()->FindComponentByClass<UHealthComponent>();
	if (!health)
	{
		LOGE("Actor %s needs a health component to implement SpawnActorOnDeath.", *(GetOwner()->GetActorNameOrLabel()));
		return;
	}
	health->GetDiedBroadcast().AddLambda([this]() {
		if (!SpawnActorClass) {
			LOGE("No actor class to spawn.");
			return;
		}
		FActorSpawnParameters spawnParams;
		spawnParams.bNoFail = true;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FTransform& transform = GetOwner()->GetActorTransform();
		GetWorld()->SpawnActor<AActor>(SpawnActorClass, transform, spawnParams);
	});
	
}