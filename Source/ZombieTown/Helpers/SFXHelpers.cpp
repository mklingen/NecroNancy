#include "SFXHelpers.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


bool FSFXHelpers::PlayAudio(UObject* owner, const FVector& pos, float volume, float pitch) const
{
	if (Audio)
	{
		UGameplayStatics::PlaySoundAtLocation(owner, Audio, pos, volume, pitch, 0.0f, AudioAttenuation);
		return true;
	}
	return false;
}

bool FSFXHelpers::MakeParticles(UObject* owner, const FVector& pos, const FRotator& rot, const FVector& scale) const
{
	if (Particles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(owner, Particles,
			pos, rot, scale, true, true);
		return true;
	}
	return false;
}

bool FSFXHelpers::Play(UObject* owner, const FVector& pos, const FRotator& rot, const FVector& scale, float volume, float pitch) const
{
	bool audioPlayed = PlayAudio(owner, pos, volume, pitch);
	bool particlesPlayed = MakeParticles(owner, pos, rot, scale);
	return audioPlayed || particlesPlayed;
}
