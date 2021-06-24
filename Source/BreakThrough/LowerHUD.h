// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "BTCharacterBase.h"
#include "LowerHUD.generated.h"

UCLASS(Abstract)
class BREAKTHROUGH_API ULowerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	//Widget Components
	//Durabiliry bars
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P1DurabilityBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* P2DurabilityBar;

	//Resolve bars
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

	//Widget Animations
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* BlackScreenFadeIn;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* BlackScreenFadeOut;

	//Functions
	void UpdateLowerHUD(ABTCharacterBase* Player1, ABTCharacterBase* Player2);
};

