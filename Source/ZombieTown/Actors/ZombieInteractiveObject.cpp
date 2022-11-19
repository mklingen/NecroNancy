#include "ZombieInteractiveObject.h"

#include "ZombieTown/Helpers/ActorHelpers.h"
#include "ZombieTown/Helpers/PrintHelper.h"

// Sets default values
AZombieInteractiveObject::AZombieInteractiveObject()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	onZombieAttachedDelegate.BindUFunction(this, TEXT("OnZombieAttached"));
	onZombieDetachedDelegate.BindUFunction(this, TEXT("OnZombieDetached"));
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
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
	Zombies.Add(zombie);
}


void AZombieInteractiveObject::OnZombieDetached(AActor* zombie)
{
	if (Zombies.Contains(zombie))
	{
		CurrentZombies--;
		if (!HasEnoughZombies())
		{
			OnNotEnoughZombies();
		}
		Zombies.Remove(zombie);
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

void AZombieInteractiveObject::OnInteractionComplete()
{
	LOGI("Interaction complete");
	InteractionCompleteCallback.Broadcast();
	for (AZombieInteractionPoint* actionPoint : interactionPoints)
	{
		if (BecomesInActiveOnComplete)
		{
			actionPoint->Deactivate();
		}
		else
		{
			actionPoint->Detach();
		}
	}
	wasEverComplete = true;
}

AZombieInteractionPoint* AZombieInteractiveObject::GetClosestFreeInteractionPointOrNull(const FVector& location)
{
	float closestDist = std::numeric_limits<float>::max();
	AZombieInteractionPoint* closest = nullptr;
	for (AZombieInteractionPoint* pt : interactionPoints)
	{
		if (pt->GetReservedZombieOrNull())
		{
			continue;
		}

		if (!pt->IsActive)
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

bool AZombieInteractiveObject::CanInteract() const
{
	if (wasEverComplete && BecomesInActiveOnComplete)
	{
		return false;
	}
	return true;
}

void AZombieInteractiveObject::OnTargeted(AActor* instigator, AActor* actor)
{
	for (AZombieInteractionPoint* pt : interactionPoints)
	{
		if (pt == actor)
		{
			pt->Reserve(instigator);
			break;
		}
	}
}

void AZombieInteractiveObject::OnTargetReleased(AActor* instigator, AActor* actor)
{
	for (AZombieInteractionPoint* pt : interactionPoints)
	{
		if (pt == actor && pt->GetReservedZombieOrNull() == instigator)
		{
			pt->UnReserve();
			break;
		}
	}
}