#include "ActorHelpers.h"

#include "Components/ShapeComponent.h"

void UActorHelpers::DoRecursive(AActor* root, std::function<void(AActor*)>& callback)
{
	callback(root);
	TArray<AActor*> children;
	// This is already recursive!
	root->GetAttachedActors(children, true /* reset the array. */, true /* recursively find the attached actors.*/);
	for (AActor* child : children)
	{
		callback(child);
	}
}

double UActorHelpers::DistanceToActor(const AActor* actor, const FVector& point, ECollisionChannel channel, FVector& pointsOut)
{
	TArray<UShapeComponent*> collisions = 
		UActorHelpers::FindComponentsRecursive<UShapeComponent>(actor);
	double minDist = FVector::Dist(actor->GetActorLocation(), point);
	pointsOut = actor->GetActorLocation();
	for (UShapeComponent* shape : collisions)
	{
		if (shape->GetCollisionObjectType() != channel) {
			continue;
		}
		if (shape->GetCollisionEnabled() == ECollisionEnabled::QueryOnly || 
			shape->GetCollisionEnabled() == ECollisionEnabled::NoCollision) {
			continue;
		}
		FVector pt;
		double dist = shape->GetDistanceToCollision(point, pt);
		if (dist < minDist)
		{
			minDist = dist;
			pointsOut = pt;
		}
	}
	TArray<UStaticMeshComponent*> meshes = UActorHelpers::FindComponentsRecursive<UStaticMeshComponent>(actor);
	for (UStaticMeshComponent* mesh : meshes)
	{
		FVector pt;
		float dist = mesh->GetClosestPointOnCollision(point, pt);
		if (dist < minDist)
		{
			minDist = dist;
			pointsOut = pt;
		}
	}

	return minDist;


}

AActor* UActorHelpers::GetRootActor(AActor* child)
{
	if (child->GetParentActor())
	{
		return GetRootActor(child->GetParentActor());
	}
	return child;
}
