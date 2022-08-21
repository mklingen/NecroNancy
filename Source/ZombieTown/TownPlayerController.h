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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool CanSummon = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool CanSend = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsSummoning = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsSending = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float UseRadius = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SummonDistance = 800.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SendDistance = 600.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float SphereTraceSendRadius = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UNiagaraSystem* SummonParticles;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UNiagaraSystem* SendParticles;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		ATownCharacter* townCharacter = nullptr;

	enum class LivingStatus
	{
		AliveOnly,
		DeadOnly,
		AliveOrDead
	};
	bool GetZombiesInRadius(float radius, TArray<AZombieAIController*>& zombiesOut) const;
	bool GetTowniesInRadius(float radius, TArray<ATownieAIController*>& zombiesOut, LivingStatus livingStatus) const;
	bool GetPointingAt(float dist, FHitResult& result, const TArray<AActor*> actorsToIgnore) const;

	void OnUpDown(float value);
	void OnLeftRight(float value);
	void OnAttack();
	void OnSummon(float value);
	void OnSending(float value);
	void OnUse();

	void SummonZombies();
	void SendZombies();

	float lastUpDownInput = 0;
	float lastLeftRightInput = 0;

	bool IsGamePaused() const;
};
