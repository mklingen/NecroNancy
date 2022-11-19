#include "ZombieInteractionPoint.h"

// Sets default values
AZombieInteractionPoint::AZombieInteractionPoint()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

// Called when the game starts or when spawned
void AZombieInteractionPoint::BeginPlay()
{
	Super::BeginPlay();
}

bool AZombieInteractionPoint::Attach(AActor* zombie)
{
	if (GetAttachedZombieOrNull())
	{
		return false;
	}
	attachedZombie = zombie;
	reservedZombie = zombie;
	attachedCallback.Broadcast(zombie);
	return true;
}

bool AZombieInteractionPoint::Detach()
{
	if (!GetAttachedZombieOrNull())
	{
		return false;
	}
	detachedCallback.Broadcast(GetAttachedZombieOrNull());
	attachedZombie = nullptr;
	reservedZombie = nullptr;
	return true;
}

bool AZombieInteractionPoint::Reserve(AActor* zombie) {
	if (GetReservedZombieOrNull())
	{
		return false;
	}
	reservedZombie = zombie;
	reservedCallback.Broadcast(zombie);
	return true;
}

bool AZombieInteractionPoint::UnReserve() {
	if (!GetReservedZombieOrNull())
	{
		return false;
	}
	unReservedCallback.Broadcast(GetReservedZombieOrNull());
	reservedZombie = nullptr;
	return true;
}