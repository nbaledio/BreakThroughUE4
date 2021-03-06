// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "BTCharacterBase.h"
#include "HUDVisuals.generated.h"

UCLASS(Abstract)
class BREAKTHROUGH_API UHUDVisuals : public UUserWidget
{
	GENERATED_BODY()

public:
	//Widget Components
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1HealthBarFlash;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1HealthRedBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthBarFlash;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthRedBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1DurabilityBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2DurabilityBar;

	UPROPERTY(meta = (BindWidget))
		class UImage* P1ResolveBar1;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1ResolveBar2;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1ResolveBar3;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1ResolveBar4;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ResolveBar1;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ResolveBar2;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ResolveBar3;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ResolveBar4;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimerText;

	/*UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P1ComboMask;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P2ComboMask;*/
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1ComboCountHitsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2ComboCountHitsText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1ComboCountNumber;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2ComboCountNumber;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CounterText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CounterText;

	//Widget Animations
	/*UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* BlackScreenFadeIn;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* BlackScreenFadeOut;*/

	//Local Variables
	UCanvasPanelSlot* P1CanvasSlot;
	UCanvasPanelSlot* P2CanvasSlot;
	FVector2D position;
	FVector2D size;

	//Functions
	void SetCharacterNames(FString P1Character, FString P2Character);
	void UpdateUpperHUD(uint8 frameCount, uint8 time, ABTCharacterBase* Player1, ABTCharacterBase* Player2);
	void UpdateLowerHUD(ABTCharacterBase* Player1, ABTCharacterBase* Player2);
};
