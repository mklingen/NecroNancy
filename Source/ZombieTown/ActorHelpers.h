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
	
	template <typename TActorType>
	static TActorType* FindChildRecursive(const AActor* root)
	{
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (AActor* child : children)
		{
			TActorType* casted = Cast<TActorType>(child);
			if (casted)
			{
				return casted;
			}
		}
		return nullptr;
	}

	template <typename TInterfaceType>
	static TInterfaceType* FindActorOrComponentInterface(TSubclassOf<UInterface> interfaceClass, AActor* root)
	{
		if (Cast<TInterfaceType>(root))
		{
			return Cast<TInterfaceType>(root);
		}
		TArray<UActorComponent*> components = root->GetComponentsByInterface(interfaceClass);
		if (components.Num() > 0)
		{
			return Cast<TInterfaceType>(components[0]);
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (AActor* child : children)
		{
			TInterfaceType* casted = Cast<TInterfaceType>(child);
			if (casted)
			{
				return casted;
			}
			TArray<UActorComponent*> childComponents = child->GetComponentsByInterface(interfaceClass);
			if (childComponents.Num() > 0)
			{
				return Cast<TInterfaceType>(childComponents[0]);
			}
		}
		return nullptr;
	}

	template <typename TActorType>
	static TArray<TActorType*> FindChildrenRecursive(const AActor* root)
	{
		TArray<TActorType*> castedChildren;
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (AActor* child : children)
		{
			TActorType* casted = Cast<TActorType>(child);
			if (casted)
			{
				castedChildren.Add(casted);
			}
		}
		return castedChildren;
	}

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
