// Fill out your copyright notice in the Description page of Project Settings.


#include "GunComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

#include "ZombieTown/Helpers/PrintHelper.h"

// Sets default values for this component's properties
UGunComponent::UGunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UGunComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* owner = GetOwner();
	if (owner)
	{
		Mesh = Cast<UStaticMeshComponent>(owner->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	}

	if (!Mesh)
	{
		LOGE("Could not get static mesh component from parent.");
	}

}


void UGunComponent::Shoot(const FVector& hitPoint, const FRotator& hitRotation)
{
	if (Mesh && MuzzleFlashParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashParticles,
			Mesh, MuzzleSocketName,
			FVector(), FRotator(),
			EAttachLocation::Type::SnapToTarget,
			true, true, ENCPoolMethod::None, false);
	}

	if (HitParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
			HitParticles, hitPoint, hitRotation, FVector(1, 1, 1), true);
	}
	timeLastShot = GetWorld()->GetTimeSeconds();
}

