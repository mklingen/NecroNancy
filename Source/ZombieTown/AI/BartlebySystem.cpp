// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieTown/AI/BartlebySystem.h"
#include "ZombieTown/AI/BartlebyRoom.h"
#include "ZombieTown/AI/BartlebyObject.h"
#include "ZombieTown/AI/BartlebyGuest.h"
#include "ZombieTown/Story/ScriptedEvent.h"
#include "ZombieTown/Story/ScriptedSceneManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "ZombieTown/BartlebyInput.h"
#include "ZombieTown/Helpers/PrintHelper.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/AI/BartlebyController.h"
// Sets default values
ABartlebySystem::ABartlebySystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneManager = CreateDefaultSubobject<UScriptedSceneManager>(TEXT("Scene Manager"));
}

// Called when the game starts or when spawned
void ABartlebySystem::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABartlebyRoom::StaticClass(), FoundActors);

	for (AActor* actor : FoundActors)
	{
		Rooms.Add(Cast<ABartlebyRoom>(actor));
	}

	inputWidget = CreateWidget<UBartlebyInput>(GetWorld(), InputWidgetClass);
	if (inputWidget)
	{
		inputWidget->AddToViewport(0);
		inputWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	//StartOpenAICall();
}

void ABartlebySystem::CollectInput()
{
	if (inputWidget)
	{
		IsWaitingOnInput = true;
		inputWidget->InputText = FText::FromString("");
		inputWidget->CancelButtonPressed = false;
		inputWidget->SayButtonPressed = false;
		inputWidget->SetVisibility(ESlateVisibility::Visible);
		APlayerController* player = GetWorld()->GetFirstPlayerController();
		if (player)
		{
			player->SetInputMode(FInputModeUIOnly());
			player->SetShowMouseCursor(true);
		}
	}
}

// Called every frame
void ABartlebySystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsWaitingOnInput)
	{
		LastThingPlayerSaid = inputWidget->InputText.ToString();
		if (inputWidget->CancelButtonPressed)
		{
			LastThingPlayerSaid = "";
			IsWaitingOnInput = false;
		}
		if (inputWidget->SayButtonPressed)
		{
			IsWaitingOnInput = false;
		}

		if (!IsWaitingOnInput)
		{
			APlayerController* player = GetWorld()->GetFirstPlayerController();
			if (player)
			{
				player->SetInputMode(FInputModeGameOnly());
			}
			inputWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}


ABartlebyRoom* ABartlebySystem::GetRoomOrNull(const FString& id)
{
	FString lower = id.ToLower();
	for (ABartlebyRoom* room : Rooms)
	{
		if (room && room->Id == id || room->Id.ToLower().Contains(lower))
		{
			return room;
		}
	}
	return nullptr;
}


TArray<FDoor> ABartlebySystem::GetDoorsAt(const FString& roomId)
{
	TArray<FDoor> out;
	for (const FDoor& door : Doors)
	{
		if (door.Room1 == roomId || door.Room2 == roomId)
		{
			out.Add(door);
		}
	}
	return out;
}


ABartlebyRoom* ABartlebySystem::GetRoomAtOrNull(const FVector& pos)
{
	for (ABartlebyRoom* room : Rooms)
	{
		if (room->IsInside(pos))
		{
			return room;
		}
	}
	return nullptr;
}

void ABartlebySystem::Say(AActor* actor, const FString& title, const FString& text)
{
	FScriptedEvent event;
	event.Id = text;
	event.PlaceWordBubbleOverActor = actor;
	event.Text = FText::FromString(text);
	event.TitleText = FText::FromString(title);
	event.PauseGame = true;
	if (SceneManager)
	{
		UScriptedSceneManager::FCompletedEvent completed;
		completed.BindUFunction(this, TEXT("OnSayCompleted"));
		SceneManager->DisplayEvent(event, completed);
	}
}

void ABartlebySystem::OnSayCompleted()
{
	CollectInput();
}

TArray<UBartlebyObject*> ABartlebySystem::GetObjectsAt(const FString& roomId)
{
	TArray<UBartlebyObject*> objects;
	auto room = GetRoomOrNull(roomId);
	if (!room)
	{
		return objects;
	}
	return room->Objects;
}

FString ABartlebySystem::GenerateYouSeeString()
{
	if (!Controller)
	{
		LOGE("NO controller");
		return "";
	}
	TArray<UBartlebyObject*> objects = GetObjectsAt(Controller->CurrentRoom->Id);
	FVector pos = Controller->GetCharacter()->GetActorLocation();
	Algo::Sort(objects, [&pos](const UBartlebyObject* a, const UBartlebyObject* b)
		{
			return FVector::Dist(a->GetOwner()->GetActorLocation(), pos) < FVector::Dist(b->GetOwner()->GetActorLocation(), pos);
		});
	if (objects.Num() == 0)
	{
		return "nothing";
	}

	FString S = "[";
	for (int32 i = 0; i < objects.Num(); i++)
	{
		S += objects[i]->Id;
		if (i < objects.Num() - 1)
		{
			S += ",";
		}
	}
	S += "]";
	return S;
	
}

FString ABartlebySystem::GenerateDoorsString()
{
	if (!Controller)
	{
		LOGE("NO controller");
		return "";
	}
	const auto& roomId = Controller->CurrentRoom->Id;
	auto doors = GetDoorsAt(roomId);
	if (doors.Num() == 0)
	{
		return "";
	}

	FString S = "[";
	for (int32 i = 0; i < doors.Num(); i++)
	{
		if (doors[i].Room1 == roomId)
		{
			S += doors[i].Room2;
		}
		else
		{
			S += doors[i].Room1;
		}
		if (i < doors.Num() - 1)
		{
			S += ",";
		}
	}
	S += "]";
	return S;
}

FString ABartlebySystem::GenerateStatusString()
{
	if (!Controller)
	{
		LOGE("NO controller");
		return "";
	}
	FString guestString;
	if (LastThingPlayerSaid != "")
	{
		guestString = GuestSaidPrompt + " \"" + LastThingPlayerSaid + "\"";
	}
	return "You are in room_id=\"" + Controller->CurrentRoom->Id + "\".\nroom_description=\"" + Controller->CurrentRoom->Description + "\"\n"
		"nearby_object_ids=" + GenerateYouSeeString() +
		"\nadjacent_rooms=" + GenerateDoorsString() +
		"\n" + SeeGuestPrompt + guestString;
}

FString ABartlebySystem::GeneratePrompt(bool askForHelp)
{
	FString helpString;
	if (askForHelp)
	{
		helpString = GenerateHelpString() + "\n";
	}
	return GroundingPrompt +
		helpString +
		"STATUS:\n" + GenerateStatusString() + "\n" +
		"Enter exactly one action now:\n";
}

FString ABartlebySystem::GenerateHelpString()
{
	return HelpPrompt;
}

void ABartlebySystem::StartOpenAICall()
{
	if (!IsEnabled)
	{
		return;
	}

	FHttpModule& httpModule = FHttpModule::Get();
	FHttpRequestRef request = httpModule.CreateRequest();
	request->SetURL(URL);
	request->SetVerb(TEXT("POST"));
	request->SetHeader(TEXT("Content-type"), TEXT("application/json"));
	request->SetHeader(TEXT("Authorization"), TEXT("Bearer " + OpenAiKey));
	// Create a new JSON object
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	// Add the model name, messages, and temperature to the object
	JsonObject->SetStringField(TEXT("model"), Model);

	LastFullPrompt = "";
	if (!appendedMsg.IsEmpty())
	{
		AddLog(appendedMsg);
		LastFullPrompt += appendedMsg + "\n";
		appendedMsg = "";
	}
	FString nextPrompt = GeneratePrompt(false);
	LastFullPrompt += nextPrompt;
	AddLog(nextPrompt);
	NeedsHelpString = false; // TODO, when the AI fails, give it another help string.
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	{
		// Always generate the help string.
		TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
		MessageObject->SetStringField(TEXT("role"), TEXT("user"));
		MessageObject->SetStringField(TEXT("content"), GenerateHelpString());
		MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
	}
	for (const auto& log_element : Log)
	{
		TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
		if (log_element.Type == BartlebyLogType::Prompt)
		{
			MessageObject->SetStringField(TEXT("role"), TEXT("user"));
		}
		else
		{
			MessageObject->SetStringField(TEXT("role"), TEXT("assistant"));
		}
		MessageObject->SetStringField(TEXT("content"), log_element.Content);
		MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
	}
	JsonObject->SetArrayField(TEXT("messages"), MessagesArray);

	JsonObject->SetNumberField(TEXT("temperature"), Temperature);

	// Convert the JSON object to a string.
	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
	request->SetContentAsString(JsonString);
	IsWaitingOnOpenAI = true;
	auto completionCallback = [&](
		FHttpRequestPtr pRequest,
		FHttpResponsePtr pResponse,
		bool connectedSuccessfully)
	{
		IsWaitingOnOpenAI = false;
		if (connectedSuccessfully) {

			// We should have a JSON response - attempt to process it.
			IsWaitingOnOpenAI = false;
			// Validate http called us back on the Game Thread...
			check(IsInGameThread());
			LOGI("%s", *(pResponse->GetContentAsString()));
			TSharedRef<TJsonReader<TCHAR>> JsonReader = 
				TJsonReaderFactory<TCHAR>::Create(pResponse->GetContentAsString());
			TSharedPtr<FJsonObject> jsonObject;
			if (FJsonSerializer::Deserialize(JsonReader, jsonObject))
			{

				if (jsonObject)
				{
					TArray<TSharedPtr<FJsonValue>> ChoicesArray = jsonObject->GetArrayField("choices");
					if (ChoicesArray.Num() > 0)
					{
						TSharedPtr<FJsonObject> ChoiceObject = ChoicesArray[0]->AsObject();
						if (ChoiceObject)
						{
							TSharedPtr<FJsonObject> MessageObject = ChoiceObject->GetObjectField("message");
							FString Role = MessageObject->GetStringField("role");
							FString Content = MessageObject->GetStringField("content");
							LastThingOpenAISaid = Content;
							TArray<FString> Lines;
							LastThingOpenAISaid.ParseIntoArrayLines(Lines, true);
							if (Lines.Num() > 0)
							{
								LastThingOpenAISaid = Lines[0];
							}
							Log.push_back(BartlebyLogElement{ BartlebyLogType::Output, LastThingOpenAISaid });
						}
					}
				}
				
			}
			else
			{
				LOGE("Failed to deserialize from json reader!");
			}
		}
		else 
		{
			switch (pRequest->GetStatus()) 
			{
			case EHttpRequestStatus::Failed_ConnectionError:
				LOGE("Connection failed.");
			default:
				LOGE("Request failed.");
			}
		}
		};
	request->OnProcessRequestComplete().BindLambda(completionCallback);

	// Finally, submit the request for processing
	request->ProcessRequest();
}

void ABartlebySystem::AppendMsg(const FString& append)
{
	appendedMsg += append;
}

void ABartlebySystem::AddLog(const FString& log)
{
	Log.push_back(BartlebyLogElement{ BartlebyLogType::Prompt, log });
	if (Log.size() > MaxNumLogElements)
	{
		Log.pop_front();
	}
}