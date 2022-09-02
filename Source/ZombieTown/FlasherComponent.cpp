// Fill out your copyright notice in the Description page of Project Settings.


#include "FlasherComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PrintHelper.h"

// Sets default values for this component's properties
UFlasherComponent::UFlasherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UFlasherComponent::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate delegate;
	delegate.BindUFunction(this, TEXT("OnTakeAnyDamage"));
	if (!delegate.IsBound())
	{
		LOGE("Unable to bind damage delegate.");
	}
	this->GetOwner()->OnTakeAnyDamage.Add(delegate);

	staticMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	skeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	// TODO, should handle *all* meshes!
	if (skeletalMesh)
	{
		origMaterial = skeletalMesh->GetMaterials();

		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				origMaterial[i]->AddToRoot();
			}
		}

		if (origMaterial.Num() > 0 && FlashMaterialBase)
		{
			flashMaterial = UMaterialInstanceDynamic::Create(FlashMaterialBase, skeletalMesh);
		}
		else
		{
			LOGE("Flash component used with no material %s", *(GetOwner()->GetName()));
			return;
		}

		isFlashing = false;
		return;
	}
	else if (staticMesh)
	{


		origMaterial = staticMesh->GetMaterials();

		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				origMaterial[i]->AddToRoot();
			}
		}

		if (origMaterial.Num() > 0 && FlashMaterialBase)
		{
			flashMaterial = UMaterialInstanceDynamic::Create(FlashMaterialBase, staticMesh);
		}
		else
		{
			LOGE("Flash component used with no material %s", *(GetOwner()->GetName()));
			return;
		}

		isFlashing = false;
	}
	else
	{
		LOGE("Flasher must be used on either a skeletal or static mesh.");
		return;
	}
}


// Called every frame
void UFlasherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float time = GetWorld()->GetTimeSeconds();
	if (isFlashing)
	{
		if (flashMaterial)
		{
			flashMaterial->SetVectorParameterValue("FlashColor", FlashColor);
		}
		float flashiness = FMath::Cos((time - timeStartedFlashing) * FlashRateHz * 2 * PI);
		if (flashiness > 0)
		{
			TurnOn();
		}
		else
		{
			TurnOff();
		}
		if (time - timeStartedFlashing > FlashTime)
		{
			StopFlashing();
			return;
		}
	}
}

void UFlasherComponent::Flash()
{
	timeStartedFlashing = GetWorld()->GetTimeSeconds();
	TurnOn();
	isFlashing = true;
	
}

void UFlasherComponent::StopFlashing()
{
	isFlashing = false;
	TurnOff();

}

void UFlasherComponent::OnTakeAnyDamage(AActor* DamagedActor,
	float damage,
	const class UDamageType* DamageType,
	class AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (FlashOnDamage) 
	{
		Flash();
	}
}

void UFlasherComponent::TurnOn()
{
	if (flashMaterial)
	{
		flashMaterial->SetVectorParameterValue("FlashColor", FlashColor);
		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				if (staticMesh)
				{
					staticMesh->SetMaterial(i, flashMaterial);
				}
				if (skeletalMesh)
				{
					skeletalMesh->SetMaterial(i, flashMaterial);
				}
			}
		}
	}
}

void UFlasherComponent::TurnOff()
{
	for (int i = 0; i < origMaterial.Num(); i++)
	{
		if (origMaterial[i])
		{
			if (staticMesh)
			{
				staticMesh->SetMaterial(i, origMaterial[i]);
			}
			if (skeletalMesh)
			{
				skeletalMesh->SetMaterial(i, origMaterial[i]);
			}
		}
	}
}

