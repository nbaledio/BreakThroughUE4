// Copyright 2021 ShatterPoint Games. All Rights Reserved.


#include "BreakThroughPlayerController.h"

void ABreakThroughPlayerController::BeginPlay()
{

}

int32 ABreakThroughPlayerController::GetPlayerInput()
{
    /*KeyBinding InputTable[] =
    {
        { EKeys::S,            INPUT_DOWN },
        { EKeys::SpaceBar,     INPUT_UP },
        { EKeys::A,            INPUT_LEFT },
        { EKeys::D,            INPUT_RIGHT },
        { EKeys::U,            INPUT_LIGHT },
        { EKeys::I,            INPUT_MEDIUM },
        { EKeys::K,            INPUT_HEAVY },
        { EKeys::J,            INPUT_BREAK },
        { EKeys::O,            INPUT_LM_COMBO },
        { EKeys::L,            INPUT_HB_COMBO },
        { EKeys::P,            INPUT_LB_COMBO },
        { EKeys::Semicolon,    INPUT_MH_COMBO },
        { EKeys::Escape,       INPUT_START },
        { EKeys::BackSpace,    INPUT_SELECT },
        { EKeys::Comma,        INPUT_LEFT_STICK_CLICK },
        { EKeys::Period,       INPUT_RIGHT_STICK_CLICK },
    }; // default keyboard bindings */

    /*KeyBinding InputTable[] =
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
    };*/

    int32 inputs = 0;

    for (int32 i = 0; i < sizeof(InputTable) / sizeof(InputTable[0]); i++) {
        if (IsInputKeyDown(InputTable[i].key)) {
            inputs |= InputTable[i].input;
        }
    }
    return inputs;
}