// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticMeshFadeComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Curves/CurveFloat.h"

#include "PrintHelper.h"

// Sets default values for this component's properties
UStaticMeshFadeComponent::UStaticMeshFadeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;


}


// Called when the game starts
void UStaticMeshFadeComponent::BeginPlay()
{
	Super::BeginPlay();

	mesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (!mesh)
	{
		LOGE("Fade component used on a non-mesh %s", *(GetOwner()->GetName()));
		return;
	}

	if (!mesh->GetNumMaterials())
	{
		LOGE("Fade component used with no material %s", *(GetOwner()->GetName()));
		return;
	}
	origMaterial = mesh->GetMaterials();

	for (int i = 0; i < origMaterial.Num(); i++)
	{
		if (origMaterial[i])
		{
			origMaterial[i]->AddToRoot();
		}
	}

	if (origMaterial.Num() > 0 && FadeMaterialBase)
	{
		fadeMaterial = UMaterialInstanceDynamic::Create(FadeMaterialBase, mesh);
	}
	else
	{
		LOGE("Fade component used with no material %s", *(GetOwner()->GetName()));
		return;
	}

	State = EFadeState::Visible;
}


// Called every frame
void UStaticMeshFadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (State)
	{
		case EFadeState::Visible:
		{
			CurrentAlpha = -1;
			CurrentT = -1;
			if (!ShouldBeVisible)
			{
				StartFadingOut();
			}
			break;
		}
		case EFadeState::Invisible:
		{
			CurrentAlpha = -1;
			CurrentT = -1;
			if (ShouldBeVisible)
			{
				StartFadingIn();
			}
			break;
		}
		case EFadeState::FadingIn:
		case EFadeState::FadingOut:
		{
			float t = FMath::Clamp((GetWorld()->GetTimeSeconds() - timeStateChanged) / FMath::Max(FadeTime, 1.0e-3), 0.0f, 1.0f);
			if (State == EFadeState::FadingIn)
			{
				t = 1.0f - t;
			}
			float alpha = (FadeCurve == nullptr) ? t : FadeCurve->GetFloatValue(t);
			CurrentAlpha = alpha;
			CurrentT = t;
			if (State == EFadeState::FadingIn && t <= 0.0f)
			{
				BecomeVisible();
			}
			else if (State == EFadeState::FadingOut && t >= 1.0f)
			{
				BecomeInvisible();
			}
			if (fadeMaterial)
			{
				fadeMaterial->SetScalarParameterValue("Alpha", alpha);
			}
			break;
		}
	}
}

void UStaticMeshFadeComponent::StartFadingOut()
{
	timeStateChanged = GetWorld()->GetTimeSeconds();
	State = EFadeState::FadingOut;

	GetOwner()->SetActorHiddenInGame(false);
	if (mesh)
	{
		if (fadeMaterial)
		{
			for (int i = 0; i < origMaterial.Num(); i++)
			{
				if (origMaterial[i])
				{
					mesh->SetMaterial(i, fadeMaterial);
				}
			}
		}
	}
}

void UStaticMeshFadeComponent::StartFadingIn()
{
	timeStateChanged = GetWorld()->GetTimeSeconds();
	State = EFadeState::FadingIn;
	GetOwner()->SetActorHiddenInGame(false);
	if (fadeMaterial)
	{
		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				mesh->SetMaterial(i, fadeMaterial);
			}
		}
	}
}

void UStaticMeshFadeComponent::BecomeInvisible()
{
	GetOwner()->SetActorHiddenInGame(true);
	State = EFadeState::Invisible;
	if (mesh)
	{
		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				mesh->SetMaterial(i, origMaterial[i]);
			}
		}
	}
}

void UStaticMeshFadeComponent::BecomeVisible()
{
	GetOwner()->SetActorHiddenInGame(false);
	State = EFadeState::Visible;
	if (mesh)
	{
		for (int i = 0; i < origMaterial.Num(); i++)
		{
			if (origMaterial[i])
			{
				mesh->SetMaterial(i, origMaterial[i]);
			}
		}
	}
}

