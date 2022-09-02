// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelExit.generated.h"

class AZombieTownGameModeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIETOWN_API ULevelExit : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelExit();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool IsEnabled = false;

	UPROPERTY()
		AActor* player = nullptr;

	UPROPERTY(EditAnywhere)
		FName NextLevel;

	UPROPERTY(EditAnywhere)
		FName CurrentLevel;
	
public:	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, 
						AActor* Other, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void Exit();

	UFUNCTION()
		void UnloadPreviousLevel();

	UFUNCTION()
		void LoadNextLevel();

	UFUNCTION()
		void OnNextLevelLoaded();

private:
	UPROPERTY()
		FTimerHandle fadeTimer;

	AZombieTownGameModeBase* gameMode = nullptr;
};
