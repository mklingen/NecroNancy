// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolComponent.h"

#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UToolComponent::UToolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

bool UToolComponent::AttachToMesh(USkeletalMeshComponent* mesh)
{
	if (ToolActorClass && mesh)
	{
		spawnedTool = GetWorld()->SpawnActor<AActor>(ToolActorClass, FTransform());
		spawnedTool->AttachToComponent(mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ToolSocketName);
		return true;
	}
	return false;
}


// Called when the game starts
void UToolComponent::BeginPlay()
{
	Super::BeginPlay();


}

AActor* UToolComponent::GetSpawnedTool() const
{
	return spawnedTool;
}

bool UToolComponent::Attach(TSubclassOf<AActor> toolActor)
{
	ToolActorClass = toolActor;
	Detach();
	USkeletalMeshComponent* mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (mesh)
	{
		return AttachToMesh(mesh);
	}
	return false;
}

void UToolComponent::Detach()
{
	if (spawnedTool)
	{
		spawnedTool->Destroy();
	}
}

