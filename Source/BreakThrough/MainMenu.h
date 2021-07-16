// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/PlayerController.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Slate.h"
#include "Math/UnrealMathUtility.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

private:
	//Widget Components
	//Menu Panels
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* MainMenu;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* PlayLocalMenu;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* OptionsMenu;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* SideSelectMenu;

	//Images
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Marker;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1ControllerIcon;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2ControllerIcon;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CPUText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CPUText;

	//Widget Animations
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* MarkerAnimation;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* FlowerAnimation;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* CloudAnimation;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* SideSelectSlide;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P1CToL;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P1CToR;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P2CToL;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P2CToR;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* ResetControllerPositions;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* LoadingIconSpin;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* LoadingScreenFadeIn;

	//Local Variables
	int menuIndex = 0;
	int optionIndex = 0;
	int P1Side;
	int P2Side;

	bool P1AcceptConfirmInput;
	bool P1AcceptBackInput;
	bool P1AcceptVerticalInput;
	bool P1AcceptHorizontalInput;
	bool P2AcceptHorizontalInput;
	bool loading;

	FString Gamemode;

	//Input variables
	//P1 inputs
	float P1_HORIZONTAL_AXIS;
	float P1_VERTICAL_AXIS;
	bool P1_INPUT_DOWN;
	bool P1_INPUT_UP;
	bool P1_INPUT_LEFT;
	bool P1_INPUT_RIGHT;
	bool P1_INPUT_ACCEPT;
	bool P1_INPUT_BACK;

	//P2 inputs
	float P2_HORIZONTAL_AXIS;
	float P2_VERTICAL_AXIS;
	bool P2_INPUT_LEFT;
	bool P2_INPUT_RIGHT;

	//Controllers
	APlayerController* P1Controller;
	APlayerController* P2Controller;

	//Functions
	void GetP1Inputs();
	void GetP2Inputs();
	void MainMenuInteractions();
	void PlayLocalMenuInteractions();
	void OptionsMenuInteractions();
	void SideSelectMenuInteractions();
	void SetMarker(int currentOptionIndex);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
