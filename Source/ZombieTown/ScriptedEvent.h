// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimationAsset.h"
#include "ScriptedEvent.generated.h"

USTRUCT(BlueprintType)
struct FScriptedEvent
{
	GENERATED_BODY()


		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText TitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FText Text;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeToDisplay = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		AActor* PlaceWordBubbleOverActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		bool PlaceWordBubbleOverPlayer = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		FLinearColor BackgroundColor = FLinearColor(1, 1, 1, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		FLinearColor TextColor = FLinearColor(0, 0, 0, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		FLinearColor TitleColor = FLinearColor(0, 0, 0, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
		AActor* MoveCameraToActorTransform = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
		FViewTargetTransitionParams CameraTransitionParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
		FViewTargetTransitionParams CameraReturnTransitionParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Word Bubble")
		FVector2D WordBubbleScreenOffset = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool PauseGame = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString GoToIdNext = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AutoTransition = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Animation")
		UAnimationAsset* PlayActorAnimation = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		bool AnimationLoops = true;
};
