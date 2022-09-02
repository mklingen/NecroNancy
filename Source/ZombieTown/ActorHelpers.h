#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "ActorHelpers.generated.h"

UCLASS()
class ZOMBIETOWN_API UActorHelpers : public UObject
{
	GENERATED_BODY()

public:

	UActorHelpers() = default;

	static void DoRecursive(AActor* root, std::function<void(AActor*)> callback);
	
	template <typename TComponent>
	static TComponent* FindComponentRecursive(const AActor* root)
	{
		TComponent* rootComponent = FindComponentOfClass<TComponent>(root);
		if (rootComponent)
		{
			return rootComponent;
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (const AActor* child : children)
		{
			TComponent* childComponent = FindComponentByClass<TComponent>(child);
			if (childComponent)
			{
				return childComponent;
			}
		}
		return nullptr;
	}

	template <typename TComponent>
	static TArray<TComponent*> FindComponentsRecursive(const AActor* root)
	{
		TArray<TComponent*> components;
		TComponent* rootComponent = root->FindComponentByClass<TComponent>();
		if (rootComponent)
		{
			components.Add(rootComponent);
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (const AActor* child : children)
		{
			TComponent* childComponent = child->FindComponentByClass<TComponent>();
			if (childComponent)
			{
				components.Add(childComponent);
			}
		}
		return components;
	}

	static double DistanceToActor(const AActor* actor, const FVector& point, ECollisionChannel channels, FVector& pointsOut);

};
