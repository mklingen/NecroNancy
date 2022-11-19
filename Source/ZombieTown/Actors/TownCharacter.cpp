// Fill out your copyright notice in the Description page of Project Settings.


#include "TownCharacter.h"

#include "Components/CapsuleComponent.h"

#include "ZombieTown/Components/HealthComponent.h"
#include "ZombieTown/Components/ToolComponent.h"
#include "ZombieTown/Zombifier.h"
#include "ZombieTown/Components/GunComponent.h"
#include "ZombieTown/ZombieTownGameModeBase.h"
#include "ZombieTown/Helpers/PrintHelper.h"
#include "ZombieTown/Components/FlasherComponent.h"
#include "ZombieTown/Components/MeleeWeapon.h"

// Sets default values
ATownCharacter::ATownCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	zombifier = CreateDefaultSubobject<UZombifier>(TEXT("Zombifier"));
	toolComponent = CreateDefaultSubobject<UToolComponent>(TEXT("Tool"));
	flasherComponent = CreateDefaultSubobject<UFlasherComponent>(TEXT("Flasher"));
}

float ATownCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float amount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	// This should be handled by the health component itself.. very confusing, yes?
	//health->Damage(amount);
	if (health->IsDead())
	{
		UCapsuleComponent* capsule = GetCapsuleComponent();
		if (capsule)
		{
			capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		}
		USkeletalMeshComponent* mesh = GetMesh();
		if (mesh)
		{
			mesh->SetSimulatePhysics(true);
			mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
	return amount;
}


void ATownCharacter::AttachTool()
{
	if (toolComponent)
	{
		toolComponent->AttachToMesh(GetMesh());
	}
}

// Called when the game starts or when spawned
void ATownCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (ShouldAttachToolOnStartup)
	{
		AttachTool();
	}
	GameMode = Cast<AZombieTownGameModeBase>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ATownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If dead, still keep track of state (might get resurrected by nancy later).
	if (GetIsDead())
	{
		AttackedRecently = false;
		IsPanicking = false;
		IsScared = false;
		USkeletalMeshComponent* mesh = GetMesh();
		// Makes sure the actor location corresponds to the ragdoll, if it exists.
		if (mesh && mesh->Bodies.Num() > 0)
		{
			SetActorLocation(mesh->Bodies[0]->GetCOMPosition());
		}
		return;
	}

	if (AttackedRecently && GetWorld()->GetTimeSeconds() - timeOfLastAttack > 0.25f)
	{
		AttackedRecently = false;
	}

	if (ShotRecently && GetWorld()->GetTimeSeconds() - timeOfLastShot > 0.25f)
	{
		ShotRecently = false;
	}
}

// Called to bind functionality to input
void ATownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATownCharacter::Attack()
{
	AttackedRecently = true;
	timeOfLastAttack = GetWorld()->GetTimeSeconds();
}

void ATownCharacter::Shoot(const FVector& hitPoint, const FRotator& hitRotation)
{
	ShotRecently = true;
	timeOfLastShot = GetWorld()->GetTimeSeconds();

	if (toolComponent)
	{
		AActor* tool = toolComponent->GetSpawnedTool();
		if (tool)
		{
			UGunComponent* gun = tool->FindComponentByClass<UGunComponent>();
			if (gun)
			{
				gun->Shoot(hitPoint, hitRotation);
			}
		}
	}
}

bool ATownCharacter::AttackWithCooldown(float cooldown)
{
	if (GetWorld()->GetTimeSeconds() - timeOfLastAttack < cooldown)
	{
		return false;
	}
	Attack();
	return true;
}

bool ATownCharacter::ShootWithCoolDown(float cooldown, const FVector& hitPoint, const FRotator& hitRotation)
{
	if (GetWorld()->GetTimeSeconds() - timeOfLastShot < cooldown)
	{
		return false;
	}
	Shoot(hitPoint, hitRotation);
	return true;
}

bool ATownCharacter::GetIsDead() const
{
	if (!health)
	{
		return false;
	}
	return health->IsDead();
}

ATownCharacter* ATownCharacter::BecomeZombie()
{
	if (!zombifier)
	{
		LOGE("No zombifier");
	}
	ATownCharacter* zombie = zombifier->SpawnZombie(this);
	Destroy();
	return zombie;
}

bool ATownCharacter::GetHasGun() const
{
	return (GetGunOrNull() != nullptr);
}

bool ATownCharacter::GetHasMeleeWeapon() const
{
	return (GetMeleeWeaponOrNull() != nullptr);
}

UGunComponent* ATownCharacter::GetGunOrNull() const
{
	if (toolComponent)
	{
		AActor* tool = toolComponent->GetSpawnedTool();
		if (tool)
		{
			return tool->FindComponentByClass<UGunComponent>();
		}
	}
	return nullptr;
}

bool ATownCharacter::GetHasAttachedTool() const
{
	if (toolComponent)
	{
		AActor* tool = toolComponent->GetSpawnedTool();
		if (tool)
		{
			return true;
		}
	}
	return false;
}


UMeleeWeapon* ATownCharacter::GetMeleeWeaponOrNull() const
{
	if (toolComponent)
	{
		AActor* tool = toolComponent->GetSpawnedTool();
		if (tool)
		{
			return tool->FindComponentByClass<UMeleeWeapon>();
		}
	}
	return nullptr;
}
