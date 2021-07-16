// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectForeground.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API UCharacterSelectForeground : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UImage* P1Cursor;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2Cursor;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* BackMenu;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BackMenuYesText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BackMenuNoText;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* BackMenuSlide;
	void PlayBackMenuSlide();
	void PlayBackMenuSlideReverse();
};
