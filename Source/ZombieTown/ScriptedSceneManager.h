// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScriptedEvent.h"
#include "ScriptedSequence.h"

#include "ScriptedSceneManager.generated.h"

class UWordBubbleComponent;
class AZombieTownGameModeBase;
class UUserWidget;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UScriptedSceneManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UScriptedSceneManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float currentEventStartedOn = 0;
	int lastCharIdx = -1;
	FString fullText = "";
	bool hasEvent = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UWordBubbleComponent* wordBubble = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AZombieTownGameModeBase* gameMode = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AActor* cameraOrigViewTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UUserWidget* pauseHintWidget = nullptr;

	void UpdateWordBubble();
	float timeToStartOffset = 0.0f;
	FViewTargetTransitionParams defaultViewTransitionParams;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString CurrentText;

	DECLARE_DYNAMIC_DELEGATE(FCompletedEvent);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		void DisplayEvent(FScriptedEvent scriptedEvent, const FCompletedEvent& completedDelegate);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		bool DisplaySequence(FScriptedSequence sequence, const FCompletedEvent& completedDelegate);


	UFUNCTION(BlueprintCallable)
		void Clear();

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		bool DisplayEventId(const FString& id, const FCompletedEvent& completedDelegate);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScriptedEvent> Events;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FScriptedSequence> Sequences;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FScriptedEvent CurrentEvent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> WordBubbleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UUserWidget> PauseHintClass;

private:
	FCompletedEvent currentCompletionCallback;

};
