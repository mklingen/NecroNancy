// Fill out your copyright notice in the Description page of Project Settings.


#include "WordBubbleComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Camera/PlayerCameraManager.h"
#include "PrintHelper.h"

// Sets default values for this component's properties
UWordBubbleComponent::UWordBubbleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;

}


// Called when the game starts
void UWordBubbleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TextBoxWidgetClass)
	{
		TextBoxWidget = CreateWidget<UUserWidget>(GetWorld(), TextBoxWidgetClass);
	}
	if (TextBoxWidget)
	{
		TextBoxWidget->AddToViewport(0);
		TextTitleWidget = Cast<UTextBlock>(TextBoxWidget->WidgetTree->FindWidget(TEXT("TitleText")));
		TextContentWidget = Cast<UTextBlock>(TextBoxWidget->WidgetTree->FindWidget(TEXT("TextContent")));
		BorderWidget = Cast<UBorder>(TextBoxWidget->WidgetTree->FindWidget(TEXT("Border")));
		OutlineWidget = Cast<UBorder>(TextBoxWidget->WidgetTree->FindWidget(TEXT("Outline")));
		TextBoxWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}


// Called every frame
void UWordBubbleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float tFade = FMath::Clamp((GetWorld()->GetTimeSeconds() - timeStartedFading) / FMath::Max(FadeTime, 1e-3), 0.0f, 1.0f);
	if (!fadingIn)
	{
		tFade = 1.0f - tFade;
		if (tFade <= 1e-3 && TextBoxWidget)
		{
			TextBoxWidget->SetVisibility(ESlateVisibility::Hidden);
			if (destroyAfterHidden)
			{
				TextBoxWidget->RemoveFromViewport();
				GetOwner()->Destroy();
				return;
			}
		}
	}

	if (TextTitleWidget)
	{
		auto titleColor = TitleColor;
		titleColor.A = tFade * titleColor.A;
		TextTitleWidget->SetColorAndOpacity(titleColor);
	}
	if (TextContentWidget)
	{
		auto textColor = TextColor;
		textColor.A = tFade * textColor.A;
		TextContentWidget->SetColorAndOpacity(textColor);
	}
	if (BorderWidget)
	{
		if (TextBoxWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			FLinearColor color = BackgroundColor;
			color.A = tFade * color.A;
			BorderWidget->Background.TintColor = color;
			auto* dynamicMaterial = BorderWidget->GetDynamicMaterial();
			if (dynamicMaterial)
			{
				dynamicMaterial->SetScalarParameterValue("Opacity", color.A);
			}
			if (OutlineWidget)
			{
				auto* outlineMaterial = OutlineWidget->GetDynamicMaterial();
				if (outlineMaterial)
				{
					outlineMaterial->SetScalarParameterValue("Opacity", color.A);
				}
			}
		}
	}

	if (TextBoxWidget)
	{
		FVector2D screenPos;
		int screenWidth;
		int screenHeight;
		if (ProjectToScreen(GetComponentLocation(), screenPos, screenWidth, screenHeight))
		{
			FGeometry geom = TextBoxWidget->GetCachedGeometry();
			FVector2D size = geom.GetAbsoluteSize();
			float minX = size.X * 0.5f;
			float minY = size.Y * 0.5f;
			float maxX = screenWidth - minX;
			float maxY = screenHeight - minY;
			screenPos += ScreenOffset;
			screenPos = FVector2D(FMath::RoundFromZero(FMath::Clamp(screenPos.X, minX, maxX)), FMath::RoundFromZero(FMath::Clamp(screenPos.Y, minY, maxY)));

			TextBoxWidget->SetPositionInViewport(screenPos, true);
		}
	}
}

void UWordBubbleComponent::SetTextContent(const FText& text)
{
	if (TextContentWidget)
	{
		TextContentWidget->SetText(text);
	}
}

void UWordBubbleComponent::SetTextTitle(const FText& text)
{
	if (TextTitleWidget)
	{
		TextTitleWidget->SetText(text);
	}
}

bool UWordBubbleComponent::ProjectToScreen(const FVector& worldLocation, FVector2D& screenLocationout, int& widthOut, int& heightOut)
{
	if (!GetWorld())
	{
		return false;
	}
	const APlayerController* playerController = Cast<const APlayerController>(GetWorld()->GetFirstPlayerController());

	if (!playerController)
	{
		return false;
	}

	playerController->ProjectWorldLocationToScreen(worldLocation, screenLocationout);

	playerController->GetViewportSize(widthOut, heightOut);

	return true;
}

void UWordBubbleComponent::Show()
{
	if (TextBoxWidget)
	{
		if (TextBoxWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			return;
		}
		TextBoxWidget->SetVisibility(ESlateVisibility::Visible);
		if (BorderWidget)
		{
			BorderWidget->Background.TintColor = FLinearColor(0, 0, 0, 0);
		}
	}
	fadingIn = true;
	timeStartedFading = GetWorld()->GetTimeSeconds();
}

void UWordBubbleComponent::Hide()
{
	if (TextBoxWidget)
	{
		if (TextBoxWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			return;
		}
	}
	fadingIn = false;
	timeStartedFading = GetWorld()->GetTimeSeconds();
}

void UWordBubbleComponent::DestroyWordBubble()
{
	destroyAfterHidden = true;
	Hide();
}

void UWordBubbleComponent::DestroyWordBubbleImmediately()
{
	if (TextBoxWidget)
	{
		TextBoxWidget->RemoveFromViewport();
	}
}