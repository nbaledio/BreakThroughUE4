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
#include "UpperHUD.generated.h"

UCLASS(Abstract)
class BREAKTHROUGH_API UUpperHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	//Widget Components
	//Timer/Names
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimerText;

	//Health bars
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

	//Combo/attack state text
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P1ComboMask;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P2ComboMask;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P1ComboBackgroundTrue;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P1ComboBackgroundNotTrue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1ComboCountNumberBackgroundTrue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1ComboCountNumberBackgroundNotTrue;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P2ComboBackgroundTrue;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P2ComboBackgroundNotTrue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2ComboCountNumberBackgroundTrue;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2ComboCountNumberBackgroundNotTrue;
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

	//Local Variables
	UCanvasPanelSlot* P1CanvasSlot;
	UCanvasPanelSlot* P2CanvasSlot;
	FVector2D position;
	FVector2D size;

	//Functions
	void SetCharacterNames(FString P1Character, FString P2Character);
	void UpdateUpperHUD(uint8 frameCount, uint8 time, ABTCharacterBase* Player1, ABTCharacterBase* Player2);
};
