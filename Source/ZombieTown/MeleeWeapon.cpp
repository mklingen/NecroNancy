// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UMeleeWeapon::UMeleeWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}



// Called when the game starts
void UMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void UMeleeWeapon::OnAttack(const FVector& hitPoint)
{


}

void UMeleeWeapon::DoAttack(AActor* enemy)
{
	FDamageEvent damageEvent;
	enemy->TakeDamage(Damage, damageEvent, nullptr, GetOwner());

	if (Knockback > 0)
	{
		ACharacter* character = Cast<ACharacter>(enemy);
		if (character)
		{
			FVector dir = character->GetActorLocation() - GetOwner()->GetActorLocation();
			dir.Z = 0;
			dir.Normalize();
			character->LaunchCharacter(dir * Knockback, true, false);
		}
	}
}

