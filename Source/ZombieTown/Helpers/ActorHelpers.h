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

	static void DoRecursive(AActor* root, std::function<void(AActor*)>& callback);
	
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

	// Creates an actor of the given type and attaches it to the root actor with the given relative location and rotation.
	// Returns nullptr if no such actor could be created.
	template <typename TActorType>
	static TActorType* CreateAndAttach(TSubclassOf<AActor> actorClass, AActor* root, const FVector& relativeLocation, const FRotator& relativeRotation)
	{
		// Check for nullptr input parameters
		if (!actorClass || !root)
		{
			return nullptr;
		}


		FVector worldLocation = root->GetActorTransform().TransformPosition(relativeLocation);
		FRotator worldRotation = FRotator(root->GetActorTransform().TransformRotation(relativeRotation.Quaternion()));
		TActorType* childActor = Cast<TActorType>(root->GetWorld()->SpawnActor(actorClass, &worldLocation, &worldRotation));
		if (!childActor)
		{
			return nullptr;
		}
		childActor->AttachToActor(root, FAttachmentTransformRules::KeepWorldTransform);
		return childActor;
	}

	// Converts an object to a type of TScriptInterface, assuming it implements that interface.
	template <typename TInterfaceType>
	static TScriptInterface<TInterfaceType> GetScriptInterface(UObject* obj) {
		TScriptInterface<TInterfaceType> returnType;
		returnType.SetInterface(Cast<TInterfaceType>(obj));
		returnType.SetObject(obj);
		return returnType;
	}

	// Gets either an actor or a component which implements the given interface, or invalid (null) interface otherwise.
	template <typename TInterfaceType>
	static TScriptInterface<TInterfaceType> FindActorOrComponentInterface(TSubclassOf<UInterface> interfaceClass, AActor* root)
	{
		if (Cast<TInterfaceType>(root))
		{
			return GetScriptInterface<TInterfaceType>(root);
		}
		TArray<UActorComponent*> components = root->GetComponentsByInterface(interfaceClass);
		if (components.Num() > 0)
		{
			return GetScriptInterface<TInterfaceType>(components[0]);
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (AActor* child : children)
		{
			TInterfaceType* casted = Cast<TInterfaceType>(child);
			if (casted)
			{
				return GetScriptInterface<TInterfaceType>(child);
			}
			TArray<UActorComponent*> childComponents = child->GetComponentsByInterface(interfaceClass);
			if (childComponents.Num() > 0)
			{
				return GetScriptInterface<TInterfaceType>(childComponents[0]);
			}
		}
		return nullptr;
	}

#define GET_INTERFACE(InterfaceType, root) UActorHelpers::FindActorOrComponentInterface<I##InterfaceType>(U##InterfaceType::StaticClass(), root)

	// Returns all the interfaces including the root and all components/actors that are below the root.
	template <typename TInterfaceType>
	static void FindActorOrComponentInterfacesRecursive(TSubclassOf<UInterface> interfaceClass, AActor* root, TArray<TScriptInterface<TInterfaceType>>& interfaces)
	{
		if (Cast<TInterfaceType>(root))
		{
			interfaces.Add(GetScriptInterface<TInterfaceType>(root));
		}
		TArray<UActorComponent*> components = root->GetComponentsByInterface(interfaceClass);
		for (UActorComponent* component : components)
		{
			interfaces.Add(GetScriptInterface<TInterfaceType>(component));
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (AActor* child : children)
		{
			FindActorOrComponentInterfacesRecursive(interfaceClass, child, interfaces);
		}
	}

	template <typename TInterfaceType>
	static TArray<TScriptInterface<TInterfaceType>> FindActorOrComponentInterfacesRecursive(TSubclassOf<UInterface> interfaceClass, AActor* root) 
	{
		TArray<TScriptInterface<TInterfaceType>> a;
		FindActorOrComponentInterfacesRecursive(interfaceClass, root, a);
		return a;
	}

#define GET_INTERFACES_RECURSIVE(InterfaceType, root) UActorHelpers::FindActorOrComponentInterfacesRecursive<I##InterfaceType>(U##InterfaceType::StaticClass(), root)


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
		TComponent* rootComponent = root->FindComponentByClass<TComponent>();
		if (rootComponent)
		{
			return rootComponent;
		}
		TArray<AActor*> children;
		root->GetAttachedActors(children, true, true);
		for (const AActor* child : children)
		{
			TComponent* childComponent = child->FindComponentByClass<TComponent>();
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

	static AActor* GetRootActor(AActor* child);
};
