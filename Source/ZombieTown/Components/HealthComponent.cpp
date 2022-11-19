// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "ZombieTown/Helpers/ActorHelpers.h"
#include "ZombieTown/Helpers/PrintHelper.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate delegate;
	delegate.BindUFunction(this, TEXT("OnTakeAnyDamage"));
	this->GetOwner()->OnTakeAnyDamage.Add(delegate);
	Health = MaxHealth;

}

void UHealthComponent::OnTakeAnyDamage(AActor* DamagedActor,
	float damage,
	const class UDamageType* DamageType,
	class AController* InstigatedBy,
	AActor* DamageCauser)
{
	this->Damage(damage);
	if (DamageChildActors)
	{
		FDamageEvent damageEvent;
		damageEvent.DamageTypeClass = DamageType->StaticClass();
		TArray<AActor*> attachedChildren;
		// NOTE: confusingly enough, this is how you get children.
		// I have no idea what "Children" actually returns, but it isn't this!
		DamagedActor->GetAttachedActors(attachedChildren);
		for (AActor* child : attachedChildren)
		{
			UActorHelpers::DoRecursive(child, [damage, damageEvent, InstigatedBy, DamageCauser](AActor* actor) {
				actor->TakeDamage(damage, damageEvent, InstigatedBy, DamageCauser);
			});
		}
	}
}


