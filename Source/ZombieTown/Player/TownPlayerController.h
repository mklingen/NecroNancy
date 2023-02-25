// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Containers/EnumAsByte.h"

#include "TownPlayerController.generated.h"

class ATownCharacter;
class AZombieAIController;
class ATownieAIController;
class UNiagaraSystem;

UCLASS()
class ZOMBIETOWN_API ATownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATownPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float dt) override;

	// If true, this player can summon zombies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool CanSummon = false;

	// If true, this player can send zombies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool CanSend = false;

	// If true, this player is currently summoning zombies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsSummoning = false;

	// If true, this player is currently sending zombies in the direction
	// they are facing.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsSending = false;

	// The distance in which we can use objects.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float UseRadius = 150.0f;

	// The distance in which we can summon zombies.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SummonDistance = 800.0f;

	// The distance in which we can send zombies.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SendDistance = 600.0f;

	// Size of the "send" sphere. The sphere will hit objects and the
	// zombies will be sent to that object.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SphereTraceSendRadius = 50.0f;

	// Particle system for when we summon zombies.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UNiagaraSystem* SummonParticles;

	// Particle system for when we send zombies.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UNiagaraSystem* SendParticles;

	// The prototype class for zombies.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<ATownCharacter> ZombieClass;

	// Default number of zombies we can have.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		int DefaultNumZombies = 6;

	// Tool that the player will spawn with.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TSubclassOf<AActor> ToolClass;

protected:
	// Pointer to the underlying character.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ATownCharacter* townCharacter = nullptr;

	// Helper for alive status (for sending/summoning).
	enum class LivingStatus
	{
		AliveOnly, // Only living things.
		DeadOnly, // Only dead things.
		AliveOrDead // Either alive things or dead things.
	};
	// Returns an array of zombies within a radius.
	bool GetZombiesInRadius(float radius, TArray<AZombieAIController*>& zombiesOut) const;
	// Returns an array of townies (non zombie NPCs) in a radius.
	bool GetTowniesInRadius(float radius, TArray<ATownieAIController*>& zombiesOut, LivingStatus livingStatus) const;
	// Gets a hit result for the thing the character is pointing at.
	bool GetPointingAt(float dist, FHitResult& result, const TArray<AActor*> actorsToIgnore) const;

	// Controls up/down axis.
	void OnUpDown(float value);
	// Controls left/right axis.
	void OnLeftRight(float value);
	// Controls attack button.
	void OnAttack();
	// Controls summon button.
	void OnSummon(float value);
	// Controls send button.
	void OnSending(float value);
	// Controls use button.
	void OnUse();
	// Debug function that causes us to go to the next level.
	void OnDebugNextLevel();
	// Called when summoning zombies.
	void SummonZombies();
	// Called when sending zombies.
	void SendZombies();
	// Last input on the up/down axis.
	float lastUpDownInput = 0;
	// Last input on the left/right axis.
	float lastLeftRightInput = 0;
	// Whether or not the game is paused (usually, for a scripted event).
	bool IsGamePaused() const;
};
