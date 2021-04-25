// Copyright 2021 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BreakThroughPlayerController.generated.h"

/**
 * 
 */
enum BreakThroughInputs 
{
	INPUT_DOWN = (1 << 0),
	INPUT_UP = (1 << 1),
	INPUT_LEFT = (1 << 2),
	INPUT_RIGHT = (1 << 3),
	INPUT_LIGHT = (1 << 4),
	INPUT_MEDIUM = (1 << 5),
	INPUT_HEAVY = (1 << 6),
	INPUT_BREAK = (1 << 7),
	INPUT_TAUNT = (1 << 8),
	INPUT_START = (1 << 9),
	INPUT_SELECT = (1 << 10),
	INPUT_LEFT_STICK_CLICK = (1 << 11),
	INPUT_RIGHT_STICK_CLICK = (1 << 12),
	INPUT_LM_COMBO = INPUT_LIGHT + INPUT_MEDIUM,
	INPUT_HB_COMBO = INPUT_HEAVY + INPUT_BREAK,
	INPUT_MH_COMBO = INPUT_MEDIUM + INPUT_HEAVY,
	INPUT_LB_COMBO = INPUT_LIGHT + INPUT_BREAK,
};

USTRUCT(BlueprintType)
struct FKeyBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	FKey     key;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	int32    input;
};

UCLASS()
class BREAKTHROUGH_API ABreakThroughPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		TArray<FKeyBinding> InputTable =
	{
		{ EKeys::Gamepad_DPad_Down,         INPUT_DOWN },
		{ EKeys::Gamepad_LeftStick_Down,    INPUT_DOWN },
		{ EKeys::Gamepad_DPad_Up,           INPUT_UP },
		{ EKeys::Gamepad_LeftStick_Up,      INPUT_UP },
		{ EKeys::Gamepad_DPad_Left,         INPUT_LEFT },
		{ EKeys::Gamepad_LeftStick_Left,    INPUT_LEFT },
		{ EKeys::Gamepad_DPad_Right,        INPUT_RIGHT },
		{ EKeys::Gamepad_LeftStick_Right,   INPUT_RIGHT },
		{ EKeys::Gamepad_FaceButton_Left,   INPUT_LIGHT },
		{ EKeys::Gamepad_FaceButton_Top,    INPUT_MEDIUM },
		{ EKeys::Gamepad_FaceButton_Right,  INPUT_HEAVY },
		{ EKeys::Gamepad_FaceButton_Bottom, INPUT_BREAK },
		{ EKeys::Gamepad_RightShoulder,     INPUT_LM_COMBO },
		{ EKeys::Gamepad_RightTrigger,      INPUT_HB_COMBO },
		{ EKeys::Gamepad_LeftShoulder,      INPUT_LB_COMBO },
		{ EKeys::Gamepad_LeftTrigger,       INPUT_MH_COMBO },
		{ EKeys::Gamepad_Special_Right,     INPUT_START },
		{ EKeys::Gamepad_Special_Left,      INPUT_SELECT },
		{ EKeys::Gamepad_LeftThumbstick,    INPUT_LEFT_STICK_CLICK },
		{ EKeys::Gamepad_RightThumbstick,   INPUT_RIGHT_STICK_CLICK },
		{ EKeys::S,							INPUT_DOWN },
		{ EKeys::SpaceBar,					INPUT_UP },
		{ EKeys::W,							INPUT_UP },
		{ EKeys::A,							INPUT_LEFT },
		{ EKeys::D,							INPUT_RIGHT },
		{ EKeys::U,							INPUT_LIGHT },
		{ EKeys::I,							INPUT_MEDIUM },
		{ EKeys::K,							INPUT_HEAVY },
		{ EKeys::J,							INPUT_BREAK },
		{ EKeys::O,							INPUT_LM_COMBO },
		{ EKeys::L,							INPUT_HB_COMBO },
		{ EKeys::P,							INPUT_LB_COMBO },
		{ EKeys::Semicolon,					INPUT_MH_COMBO },
		{ EKeys::Escape,					INPUT_START },
		{ EKeys::BackSpace,					INPUT_SELECT },
		{ EKeys::Comma,						INPUT_LEFT_STICK_CLICK },
		{ EKeys::Period,					INPUT_RIGHT_STICK_CLICK },
	};

	virtual void BeginPlay() override; //will be mainly used to switch out default settings with any custom input bindings

public:

	/** Get the player inputs as bit flags in an int32 */
	int32 GetPlayerInput();
	
};
