#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFXHelpers.generated.h"

class USoundCue;
class UNiagaraSystem;
class USoundAttenuation;

USTRUCT(Blueprintable)
struct ZOMBIETOWN_API FSFXHelpers
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SFX")
		USoundCue* Audio = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SFX")
		UNiagaraSystem* Particles = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SFX")
		USoundAttenuation* AudioAttenuation = nullptr;

	bool PlayAudio(UObject* owner, const FVector& pos, float volume = 1.0f, float pitch = 1.0f) const;
	bool MakeParticles(UObject* owner, const FVector& pos, const FRotator& rot = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector) const;
	bool Play(UObject* owner, const FVector& pos, const FRotator& rot = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector, float volume = 1.0f, float pitch = 1.0f) const;
};
