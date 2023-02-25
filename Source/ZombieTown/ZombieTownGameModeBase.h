// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZombieTownGameModeBase.generated.h"

UCLASS()
class ZOMBIETOWN_API AZombieTownGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// If true the game is paused waiting for the next scripted event.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsPausedForScriptedEvent = false;

	// If true, we can request the next scripted event from the game.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AllowRequestNextScriptedEvent = false;

	// If true, we are requesting the next scripted event from teh game.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool RequestNextScriptedEvent = false;
};
