// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ScriptSpeechElement.generated.h"

USTRUCT(BlueprintType)
struct FScriptSpeechElement
{
	GENERATED_BODY()

		// Must be unique, and correspond to the title text in a ScriptedSequence.
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText CharacterName;

	// The text the character will say.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText Text;
};
