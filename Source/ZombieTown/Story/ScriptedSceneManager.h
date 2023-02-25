// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZombieTown/Story/ScriptedEvent.h"
#include "ZombieTown/Story/ScriptedSequence.h"
#include "ZombieTown/Interfaces/OnLevelExitedInterface.h"

#include "ScriptedSceneManager.generated.h"

class UWordBubbleComponent;
class AZombieTownGameModeBase;
class UUserWidget;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UScriptedSceneManager : public UActorComponent, public IOnLevelExitedInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UScriptedSceneManager();

	// Callback for when the level exited.
	void OnLevelExited() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Time that the current event started.
	float currentEventStartedOn = 0;
	// The character in the string we were last displaying.
	int lastCharIdx = -1;
	// The full text of the current event.
	FString fullText = "";
	// If true, we have an event.
	bool hasEvent = false;

	// Word bubble to display on the screen.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UWordBubbleComponent* wordBubble = nullptr;

	// Pointer to the game mode.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AZombieTownGameModeBase* gameMode = nullptr;

	// Cache of the original thing that the camera was targeting before
	// a scripted event started playing.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AActor* cameraOrigViewTarget = nullptr;

	// Text that appears on screen to let us know that the game is paused, and how
	// to unpause it.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UUserWidget* pauseHintWidget = nullptr;

	// If the camera is targeting a skeleton, this is the mesh of that skeleton.
	UPROPERTY()
		USkeletalMeshComponent* targetSkeletalMesh = nullptr;
	
	// The animation that the target skeleton was playing before the scripted event
	// started.
	UPROPERTY()
		TSubclassOf<UAnimInstance> prevAnimationStateMachine = nullptr;

	// Updates the word bubble on screen.
	void UpdateWordBubble();
	// Fudge factor for when to start playing the scripted event.
	float timeToStartOffset = 0.0f;
	// Default way that we will move the camera between scripted targets.
	FViewTargetTransitionParams defaultViewTransitionParams;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// The current text on the screen.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString CurrentText;

	DECLARE_DYNAMIC_DELEGATE(FCompletedEvent);

	// Displays a single event, and then calls a delegate after it completes.
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		void DisplayEvent(FScriptedEvent scriptedEvent, const FCompletedEvent& completedDelegate);

	// Displays a whole sequence of events, and calls a delegate after it completes.
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		bool DisplaySequence(FScriptedSequence sequence, const FCompletedEvent& completedDelegate);

	// Clears the current event.
	UFUNCTION(BlueprintCallable)
		void Clear();

	// Displays an event with the given ID, and calls a delegate when it is complete.
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "completedDelegate"))
		bool DisplayEventId(const FString& id, const FCompletedEvent& completedDelegate);

	// The list of known events.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (TitleProperty = "Id"))
		TArray<FScriptedEvent> Events;

	// The list of known event sequences.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (TitleProperty = "Id"))
		TArray<FScriptedSequence> Sequences;

	// The event that is currently playing.
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FScriptedEvent CurrentEvent;

	// The prototype for the word bubble.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> WordBubbleClass;

	// The prototype for the pause hint.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UUserWidget> PauseHintClass;

private:
	FCompletedEvent currentCompletionCallback;

};
