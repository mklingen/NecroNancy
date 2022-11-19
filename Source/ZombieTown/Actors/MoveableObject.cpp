#include "MoveableObject.h"
#include "ZombieTown/Helpers/DebugDrawHelpers.h"

// Sets default values
AMoveableObject::AMoveableObject() : AZombieInteractiveObject()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BecomesInActiveOnComplete = true;
}

// Called when the game starts or when spawned
void AMoveableObject::BeginPlay()
{
	Super::BeginPlay();
	transformOnStart = GetActorTransform();
	transformOnEnd = transformOnStart * MoveTarget.Target;
}

// Called every frame
void AMoveableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsMoving && !GetIsDoneMoving())
	{
		float time = GetWorld()->GetTimeSeconds();
		float alpha = (time - timeStartedMoving) / MoveTime;
		if (alpha < 1.0f) {
			float interp = FMath::InterpEaseInOut(0.0f, 1.0f, alpha, 3.0f);
			SetActorTransform(FTransform(FQuat::Slerp(transformOnStart.GetRotation(), transformOnEnd.GetRotation(), interp), transformOnStart.GetLocation() * (1.0f - interp) + transformOnEnd.GetLocation() * interp));
		}
		else {
			NOTIFY_I("Done moving.");
			IsMoving = false;
			doneMoving = true;
			OnInteractionComplete();
		}
	}
}


void AMoveableObject::OnEnoughZombies()
{
	Super::OnEnoughZombies();
	if (doneMoving) {
		return;
	}

	IsMoving = true;
	if (timeStartedMoving < 0)
	{
		timeStartedMoving = GetWorld()->GetTimeSeconds();
	}
	NOTIFY_I("Started moving.");
}

void AMoveableObject::OnNotEnoughZombies()
{
	Super::OnNotEnoughZombies();
	IsMoving = false;
	NOTIFY_I("Missing zombies.");
}

void AMoveableObject::OnInteractionComplete()
{
	Super::OnInteractionComplete();
	IsMoving = false;
	doneMoving = true;
}

bool AMoveableObject::CanInteract() const {
	return !GetIsDoneMoving();
}

bool AMoveableObject::GetIsDoneMoving() const {
	return doneMoving;
}