// Fill out your copyright notice in the Description page of Project Settings.


#include "ScriptedSceneManager.h"

#include "ScriptedEvent.h"

#include "Blueprint/UserWidget.h"
#include "Camera/PlayerCameraManager.h"
#include "PrintHelper.h"
#include "WordBubbleComponent.h"
#include "ZombieTownGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UScriptedSceneManager::UScriptedSceneManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
	defaultViewTransitionParams.BlendTime = 1.0f;
}


// Called when the game starts
void UScriptedSceneManager::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<AZombieTownGameModeBase>(GetWorld()->GetAuthGameMode());
	pauseHintWidget = CreateWidget<UUserWidget>(GetWorld(), PauseHintClass);
	if (pauseHintWidget)
	{
		pauseHintWidget->AddToViewport(0);
		pauseHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (cameraManager)
	{
		cameraOrigViewTarget = cameraManager->GetViewTarget();
	}
}


void UScriptedSceneManager::UpdateWordBubble()
{
	if (wordBubble)
	{
		wordBubble->SetTextTitle(CurrentEvent.TitleText);
		wordBubble->SetTextContent(FText::FromString(CurrentText));
		wordBubble->BackgroundColor = CurrentEvent.BackgroundColor;
		wordBubble->TextColor = CurrentEvent.TextColor;
		wordBubble->TitleColor = CurrentEvent.TitleColor;
		wordBubble->ScreenOffset = CurrentEvent.WordBubbleScreenOffset;
	}
}

// Called every frame
void UScriptedSceneManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!hasEvent)
	{
		return;
	}


	UpdateWordBubble();

	float t = GetWorld()->GetTimeSeconds() - currentEventStartedOn - timeToStartOffset;
	float alpha = t / FMath::Max(CurrentEvent.TimeToDisplay, 1e-3);
	if (alpha > 0.0f)
	{
		if (wordBubble)
		{
			wordBubble->Show();
		}
		float normalizedT = FMath::Clamp(alpha, 0.0f, 1.0f);
		int idx = FMath::Clamp(normalizedT * fullText.Len(), 0, fullText.Len());
		if (idx != lastCharIdx)
		{
			lastCharIdx = idx;
			CurrentText = FString(idx, *fullText);
			if (wordBubble)
			{
				wordBubble->SetTextContent(FText::FromString(CurrentText));
			}
			else
			{
				LOGE("No text content widget.");
			}
		}
	}
	else
	{
		if (wordBubble)
		{
			wordBubble->Hide();
		}
	}

	if (t > CurrentEvent.TimeToDisplay + 1.5 && CurrentEvent.AutoTransition)
	{
		Clear();
	}
	else if (CurrentEvent.PauseGame)
	{
		// User unpaused the game, clear the current event.
		if (!gameMode->IsPausedForScriptedEvent || gameMode->RequestNextScriptedEvent)
		{
			gameMode->RequestNextScriptedEvent = false;
			Clear();
		}
	}

}

void UScriptedSceneManager::DisplayEvent(FScriptedEvent scriptedEvent, const FCompletedEvent& callback)
{
	hasEvent = true;
	CurrentEvent = scriptedEvent;
	currentEventStartedOn = GetWorld()->GetTimeSeconds();
	lastCharIdx = -1;
	CurrentText = "";
	fullText = CurrentEvent.Text.ToString();
	currentCompletionCallback = callback;
	targetSkeletalMesh = nullptr;
	prevAnimationStateMachine = nullptr;
	if (CurrentEvent.PlaceWordBubbleOverPlayer)
	{
		CurrentEvent.PlaceWordBubbleOverActor = Cast<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (!CurrentEvent.PlaceWordBubbleOverActor)
		{
			LOGE("No player pawn?");
		}
	}
	if (wordBubble)
	{
		wordBubble->DestroyWordBubble();
		wordBubble = nullptr;
	}

	if (CurrentEvent.PlaceWordBubbleOverActor && CurrentEvent.PlayActorAnimation)
	{
		USkeletalMeshComponent* actorMesh = CurrentEvent.PlaceWordBubbleOverActor->FindComponentByClass<USkeletalMeshComponent>();
		if (actorMesh != nullptr)
		{
			targetSkeletalMesh = actorMesh;
			prevAnimationStateMachine = targetSkeletalMesh->AnimClass;
			targetSkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			targetSkeletalMesh->SetAnimation(CurrentEvent.PlayActorAnimation);
			targetSkeletalMesh->Play(CurrentEvent.AnimationLoops);
		}
	}

	if (WordBubbleClass && CurrentEvent.PlaceWordBubbleOverActor)
	{
		AActor* wordBubbleActor = GetWorld()->SpawnActor<AActor>(WordBubbleClass, FTransform());
		wordBubbleActor->AttachToActor(CurrentEvent.PlaceWordBubbleOverActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		wordBubble = wordBubbleActor->FindComponentByClass<UWordBubbleComponent>();
		if (!wordBubble)
		{
			LOGE("Could not create word bubble?");
		}
		else
		{
			LOGI("Spawned a word bubble.");
		}
	}
	else
	{
		LOGD("Not spawning word bubble.");
	}

	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CurrentEvent.MoveCameraToActorTransform)
	{
		if (cameraManager)
		{
			timeToStartOffset = CurrentEvent.CameraTransitionParams.BlendTime;
			if (wordBubble)
			{
				wordBubble->Hide();
			}
			cameraManager->SetViewTarget(CurrentEvent.MoveCameraToActorTransform, CurrentEvent.CameraTransitionParams);
		}
	}
	else
	{
		timeToStartOffset = 0;
		if (cameraManager)
		{
			cameraManager->SetViewTarget(cameraOrigViewTarget, defaultViewTransitionParams);
		}
	}

	if (CurrentEvent.PauseGame)
	{
		if (gameMode)
		{
			gameMode->IsPausedForScriptedEvent = true;
			gameMode->AllowRequestNextScriptedEvent = true;
		}

		if (pauseHintWidget)
		{
			pauseHintWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	UpdateWordBubble();
}


void UScriptedSceneManager::Clear()
{
	if (hasEvent && CurrentEvent.PauseGame)
	{
		// Unpause the game.
		if (gameMode && CurrentEvent.GoToIdNext.IsEmpty())
		{
			gameMode->IsPausedForScriptedEvent = false;
			gameMode->AllowRequestNextScriptedEvent = false;
		}
		// Reset the camera.
		if (CurrentEvent.MoveCameraToActorTransform && CurrentEvent.GoToIdNext.IsEmpty())
		{
			APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			if (cameraManager)
			{
				cameraManager->SetViewTarget(cameraOrigViewTarget, CurrentEvent.CameraReturnTransitionParams);
			}
		}
	}
	if (pauseHintWidget)
	{
		pauseHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (hasEvent)
	{
		hasEvent = false;
		if (wordBubble)
		{
			wordBubble->DestroyWordBubble();
			wordBubble = nullptr;
		}

		if (targetSkeletalMesh && prevAnimationStateMachine)
		{
			targetSkeletalMesh->SetAnimInstanceClass(prevAnimationStateMachine);
			targetSkeletalMesh = nullptr;
			prevAnimationStateMachine = nullptr;
		}

		if (!CurrentEvent.GoToIdNext.IsEmpty())
		{
			LOGI("Auto-transitioning to the next event.");
			DisplayEventId(CurrentEvent.GoToIdNext, currentCompletionCallback);
		}
		else
		{
			LOGI("Event %s complete. Clearing and calling completion delegates..", *(CurrentEvent.Id));
			if (!currentCompletionCallback.IsBound()) {
				LOGI("Event %s has no completion callback. Ignoring.", *(CurrentEvent.Id));
			}
			// Reset the event.
			CurrentEvent = FScriptedEvent();
			// Reset the callback. Note we have to do this BEFORE executing the previous callback,
			// not after (the completion callback could RESET itself while executing!)
			auto prevCallback = currentCompletionCallback;
			currentCompletionCallback = FCompletedEvent();
			prevCallback.ExecuteIfBound();
		}
	}
}

bool UScriptedSceneManager::DisplayEventId(const FString& id, const FCompletedEvent& completionDelegate)
{
	for (const FScriptedEvent& ev : Events)
	{
		if (ev.Id == id)
		{
			LOGI("Displaying event %s", *id);
			DisplayEvent(ev, completionDelegate);
			return true;
		}
	}
	for (const FScriptedSequence& ev : Sequences)
	{
		if (ev.Id == id)
		{
			LOGI("Displaying sequence %s", *id);
			DisplaySequence(ev, completionDelegate);
			return true;
		}
	}
	LOGE("Tried to access event %s, but it was not in the list of events or sequences.", *id);
	currentCompletionCallback = FCompletedEvent();
	return false;
}


bool UScriptedSceneManager::DisplaySequence(FScriptedSequence sequence, const FCompletedEvent& completionDelegate)
{
	TArray<FScriptedEvent> events;
	sequence.GenerateEvents(events);
	for (const FScriptedEvent& ev : events)
	{
		bool foundExisting = false;
		for (FScriptedEvent& existing : Events)
		{
			if (existing.Id == ev.Id)
			{
				existing = ev;
				foundExisting = true;
				break;
			}
		}
		if (foundExisting)
		{
			break;
		}
		Events.Add(ev);
	}
	if (!events.IsEmpty())
	{
		return DisplayEventId(events[0].Id, completionDelegate);
	}
	return false;
}
