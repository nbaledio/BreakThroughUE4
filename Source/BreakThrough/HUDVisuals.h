// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
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
		class UProgressBar* P1HealthRedBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthRedBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1DurabilityBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2DurabilityBar;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1ComboTimer;	
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ComboTimer;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1ComboTimerBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2ComboTimerBar;
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
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimerText;	

	//Functions
	void SetCharacterNames(FString P1Character, FString P2Character);
	void UpdateUpperHUD(uint8 frameCount, uint8 time, ABTCharacterBase* Player1, ABTCharacterBase* Player2);
	void UpdateLowerHUD(ABTCharacterBase* Player1, ABTCharacterBase* Player2);
};
