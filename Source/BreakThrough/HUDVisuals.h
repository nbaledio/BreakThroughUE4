// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "HUDVisuals.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BREAKTHROUGH_API UHUDVisuals : public UUserWidget
{
	GENERATED_BODY()

public:
	//Widget Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* P1HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1ResolveBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2ResolveBar;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CharacterName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TimerText;

	//Functions
	void SetCharacterNames(FString P1Character, FString P2Character);
	void UpdateGraphics(int32 time, int32 P1Health, int32 P1MaxHealth, int32 P1Resolve, int32 P1Durability, int32 P2Health, int32 P2MaxHealth, int32 P2Resolve, int32 P2Durability);
	void UpdateUpperHUD(int32 time, int32 P1Health, int32 P1MaxHealth, int32 P2Health, int32 P2MaxHealth);
	void UpdateLowerHUD(int32 P1Resolve, int32 P1Durability, int32 P2Resolve, int32 P2Durability);

};
