// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomScript.generated.h"


class UScriptedSceneManager;

UCLASS(Blueprintable)
class ZOMBIETOWN_API UCustomScript : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Script")
		void OnBegin(UScriptedSceneManager* manager);
	UFUNCTION(BlueprintNativeEvent, Category = "Script")
		void OnEnd(UScriptedSceneManager* manager);
	UFUNCTION(BlueprintNativeEvent, Category = "Script")
		void OnUpdate(UScriptedSceneManager* manager);
	UFUNCTION(BlueprintNativeEvent, Category = "Script")
		bool ShouldTransition(UScriptedSceneManager* manager);

	void OnBegin_Implementation(UScriptedSceneManager* manager);
	void OnEnd_Implementation(UScriptedSceneManager* manager);
	void OnUpdate_Implementation(UScriptedSceneManager* manager);
	bool ShouldTransition_Implementation(UScriptedSceneManager* manager);
};
