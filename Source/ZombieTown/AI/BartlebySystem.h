// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <deque>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BartlebySystem.generated.h"
class UBartlebyInput;
DECLARE_DELEGATE_OneParam(FOnOpenAICompleteDelegate, const FString&);

USTRUCT(Blueprintable)
struct FDoor {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Room1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Room2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Description = "door";
};
class ABartlebyRoom;
UCLASS()
class ZOMBIETOWN_API ABartlebySystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABartlebySystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class ABartlebyController* Controller = nullptr;
	UFUNCTION()
		void Say(AActor* actor, const FString& title, const FString& text);

	UFUNCTION()
		void CollectInput();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		TArray<ABartlebyRoom*> Rooms;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<FDoor> Doors;

	UFUNCTION()
		ABartlebyRoom* GetRoomOrNull(const FString& id);

	UFUNCTION()
		ABartlebyRoom* GetRoomAtOrNull(const FVector& pos);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UScriptedSceneManager* SceneManager = nullptr;

	UFUNCTION()
		TArray<FDoor> GetDoorsAt(const FString& roomId);

	UFUNCTION()
		TArray<class UBartlebyObject*> GetObjectsAt(const FString& roomId);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UBartlebyInput> InputWidgetClass;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		bool IsWaitingOnInput = false;


	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		bool IsWaitingOnOpenAI = false;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		FString LastThingOpenAISaid = "";

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
		FString LastThingPlayerSaid;

	UFUNCTION()
		void OnSayCompleted();

	UPROPERTY()
		bool NeedsHelpString = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		bool IsEnabled = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Prompt")
		FString HelpPrompt = "BARTLEBY API:\n"
		"* say(Phrase) # says the given phrase to the guest. Keep phrases short and pithy.\n"
		"Example:\nsay(hello I am Bartleby)\n"
		"* go(Room_ID) # goes to the room from the current room.\n"
		"Example:\ngo(entry_hall)\n"
		"* examine(Object_ID) # examines the object in the room. It's important to examine something before making things up.\n"
		"Example:\nexamine(sunglasses)\n"
		"* think(Thought) # causes Bartleby to think something.\n"
		"Example:\nthink(I must tell a compelling story to this guest!)";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Prompt")
		FString GroundingPrompt = "INFO:\n"
		"You control Bartleby, a helpful and erudite british tour guide. "
		"Guide guests through the museum and make up a compelling story about what is in it.\n"
		"Do this using the Bartleby API, one action at a time.\n";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Prompt")
		FString SeeGuestPrompt = "A guest is here. ";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Prompt")
		FString GuestSaidPrompt = "The guest said:";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		FString URL = "https://api.openai.com/v1/chat/completions";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		FString OpenAiKey = "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";

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Prompt")
		FString LastFullPrompt = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		double Temperature = 0.4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		FString Model = "gpt-3.5-turbo";
	void StartOpenAICall();

	enum class BartlebyLogType
	{
		Prompt,
		Output
	};

	struct BartlebyLogElement
	{
		BartlebyLogType Type;
		FString Content;
	};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "API")
		int32 MaxNumLogElements = 8;
	FString GenerateHelpString();
	FString GenerateStatusString();
	FString GeneratePrompt(bool askForHelp);
	FString GenerateYouSeeString();
	FString GenerateDoorsString();
	void AppendMsg(const FString& lastMsg);
	void AddLog(const FString& log);
	std::deque<BartlebyLogElement> Log;
	TArray<FString> Thoughts;
	FString appendedMsg;
private:
	UPROPERTY()
		UBartlebyInput* inputWidget = nullptr;
};
