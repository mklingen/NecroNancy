#include "ZombieInteractiveObject.h"

#include "ActorHelpers.h"

// Sets default values
AZombieInteractiveObject::AZombieInteractiveObject()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	onZombieAttachedDelegate.BindUFunction(this, TEXT("OnZombieAttached"));
	onZombieDetachedDelegate.BindUFunction(this, TEXT("OnZombieDetached"));
}

// Called when the game starts or when spawned
void AZombieInteractiveObject::BeginPlay()
{
	Super::BeginPlay();

	interactionPoints = UActorHelpers::FindChildrenRecursive<AZombieInteractionPoint>(this);
	for (AZombieInteractionPoint* actionPoint : interactionPoints)
	{
		actionPoint->attachedCallback.Add(onZombieAttachedDelegate);
		actionPoint->detachedCallback.Add(onZombieDetachedDelegate);
	}
}


void AZombieInteractiveObject::OnZombieAttached(AActor* zombie)
{
	CurrentZombies++;
	if (HasEnoughZombies())
	{
		OnEnoughZombies();
	}
}


void AZombieInteractiveObject::OnZombieDetached(AActor* zombie)
{
	CurrentZombies--;
	if (!HasEnoughZombies())
	{
		OnNotEnoughZombies();
	}
}

void AZombieInteractiveObject::OnEnoughZombies()
{
	enoughZombiesCallback.Broadcast();
}

void AZombieInteractiveObject::OnNotEnoughZombies()
{
	notEnoughZombiesCallback.Broadcast();
}

AZombieInteractionPoint* AZombieInteractiveObject::GetClosestFreeInteractionPointOrNull(const FVector& location)
{
	float closestDist = std::numeric_limits<float>::max();
	AZombieInteractionPoint* closest = nullptr;
	for (AZombieInteractionPoint* pt : interactionPoints)
	{
		if (pt->GetAttachedZombieOrNull())
		{
			continue;
		}

		float dist = FVector::Dist(pt->GetActorLocation(), location);
		if (dist < closestDist)
		{
			closest = pt;
			closestDist = dist;
		}
	}
	return closest;
}

FTargetInfo AZombieInteractiveObject::GetTargetInfo(const FVector& targetedFrom)
{
	FTargetInfo info;
	info.targetActor = GetClosestFreeInteractionPointOrNull(targetedFrom);
	info.targetOffset = FTransform::Identity;
	return info;
}