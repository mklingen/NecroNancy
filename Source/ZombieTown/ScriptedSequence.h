// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ScriptedEvent.h"
#include "ScriptSpeechElement.h"
#include "ScriptedSequence.generated.h"

// Used to generate a bunch of scripted events from a simpler script.
// All of the events will run one after the other. IDs will be generated for
// them.
USTRUCT(BlueprintType)
struct FScriptedSequence
{
	GENERATED_BODY()

		// Works like a ScriptedEvent id.
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Id;

	// List of characters in this sequence.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScriptedEvent> CharacterTemplates;

	// Characters in the list will talk with the given script, events
	// will be generated one after the other.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScriptSpeechElement> Script;

	void GenerateEvents(TArray<FScriptedEvent>& events);
	FScriptedEvent GenerateEvent(const FScriptSpeechElement& element);
};
