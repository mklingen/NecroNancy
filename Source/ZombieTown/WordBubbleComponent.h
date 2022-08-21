// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WordBubbleComponent.generated.h"

class UUserWidget;
class UWidget;
class UTextBlock;
class UBorder;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIETOWN_API UWordBubbleComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWordBubbleComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UUserWidget> TextBoxWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D ScreenOffset = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor BackgroundColor = FLinearColor(1, 1, 1, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor TextColor = FLinearColor(0, 0, 0, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor TitleColor = FLinearColor(0, 0, 0, 1);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FadeTime = 0.25f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UUserWidget* TextBoxWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UTextBlock* TextTitleWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UTextBlock* TextContentWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBorder* BorderWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		UBorder* OutlineWidget = nullptr;

	UFUNCTION(BlueprintCallable)
		void SetTextContent(const FText& text);

	UFUNCTION(BlueprintCallable)
		void SetTextTitle(const FText& text);

	UFUNCTION(BlueprintCallable)
		void Show();

	UFUNCTION(BlueprintCallable)
		void Hide();

	UFUNCTION(BlueprintCallable)
		void DestroyWordBubble();
	UFUNCTION(BlueprintCallable)
		void DestroyWordBubbleImmediately();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float timeStartedFading = 0;
	bool fadingIn = false;
	bool destroyAfterHidden = false;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool ProjectToScreen(const FVector& worldLocation, FVector2D& screenLocationout, int& widthOut, int& heightOut);
};
