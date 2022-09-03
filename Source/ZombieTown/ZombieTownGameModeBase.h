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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsPausedForScriptedEvent = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AllowRequestNextScriptedEvent = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool RequestNextScriptedEvent = false;
};
