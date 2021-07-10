// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/PlayerController.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Slate.h"
#include "Math/UnrealMathUtility.h"
#include "CharacterSelect.generated.h"

UCLASS(Abstract)
class BREAKTHROUGH_API UCharacterSelect : public UUserWidget
{
	GENERATED_BODY()

public:
	//Widget Components
	//Character Select Components
	UPROPERTY(meta = (BindWidget))
		class UImage* P1CharacterPortrait;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2CharacterPortrait;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2CharacterName;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P1ColorSelectMenu;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* P2ColorSelectMenu;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P1ColorText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* P2ColorText;
	UPROPERTY(meta = (BindWidget))
		class UImage* P1Cursor;
	UPROPERTY(meta = (BindWidget))
		class UImage* P2Cursor;
	UPROPERTY(meta = (BindWidget))
		class UImage* ACH_Headshot;
	UPROPERTY(meta = (BindWidget))
		class UImage* DHA_Headshot;

	//Stage Select Components
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* StageSelectMenu;
	UPROPERTY(meta = (BindWidget))
		class UImage* StageSelectCursor;
	UPROPERTY(meta = (BindWidget))
		class UImage* StagePreview;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* StageNameText;
	UPROPERTY(meta = (BindWidget))
		class UImage* TrainingStageIcon;
	UPROPERTY(meta = (BindWidget))
		class UImage* DhaliaStageIcon;
	UPROPERTY(meta = (BindWidget))
		class UImage* IzanagiCastleIcon;

	//Animations
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P1PortraitSlide;
	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* P2PortraitSlide;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
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
	bool P1_INPUT_START;

	//P2 inputs
	float P2_HORIZONTAL_AXIS;
	float P2_VERTICAL_AXIS;
	bool P2_INPUT_DOWN;
	bool P2_INPUT_UP;
	bool P2_INPUT_LEFT;
	bool P2_INPUT_RIGHT;
	bool P2_INPUT_ACCEPT;
	bool P2_INPUT_BACK;
	bool P2_INPUT_START;

	//Local Variables
	FString Gamemode = "CPU";
	FString P1Side = "Left";
	FString P2Side = "Right";

	TArray<UImage*> Headshots;
	TArray<UImage*> StageIcons;
	
	float CursorSpeed = 15.0f;

	bool P1OnEnter;
	bool P2OnEnter;
	bool P1CharacterSelected;
	bool P1Ready;
	bool P2CharacterSelected;
	bool P2Ready;
	bool P1AcceptConfirmInput;
	bool P1AcceptBackInput;
	bool P1AcceptScrollInput;
	bool P2AcceptConfirmInput;
	bool P2AcceptBackInput;
	bool P2AcceptScrollInput;

	int P1HoveredCharacter;
	int P2HoveredCharacter;
	int P1Character = -1;
	int P1Color = 1;
	int P2Character = -1;
	int P2Color = 1;

	//Controllers
	APlayerController* P1Controller;
	APlayerController* P2Controller;

	//Functions
	void GetP1Inputs();
	void GetP2Inputs();
	void VsCPUMenuInteractions();
	void Vs2PMenuInteractions();
	void StageSelectInputs();
	int P1CursorCollisionDetection();
	void SetP1CharacterPortrait(int CharacterCode);
	int P2CursorCollisionDetection();
	void SetP2CharacterPortrait(int CharacterCode);
	int StageSelectCursorCollisionDetection();
	void SetStagePreview(int StageCode);
};
