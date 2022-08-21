// Fill out your copyright notice in the Description page of Project Settings.


#include "GunComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

#include "PrintHelper.h"

// Sets default values for this component's properties
UGunComponent::UGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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


// Called every frame
void UGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

