// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "CharacterSelect.h"

void UCharacterSelect::NativeConstruct() 
{
	Super::NativeConstruct();
	const UObject* world = (UObject*)GetWorld();
	P1Controller = UGameplayStatics::GetPlayerController(world, 0);
	P2Controller = UGameplayStatics::GetPlayerController(world, 1);
}

void UCharacterSelect::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//Read P1 inputs
	if (P1Controller)
	{
		GetP1Inputs();
	}

	//Read P2Inputs
	if (P2Controller)
	{
		GetP2Inputs();
	}
}

void UCharacterSelect::GetP1Inputs() 
{
	//Set bool variables
	SetP1ButtonInputs();

	//Read P1 analog inputs
	P1Controller->GetInputAnalogStickState(EControllerAnalogStick::CAS_LeftStick, P1_HORIZONTAL_AXIS, P1_VERTICAL_AXIS);

	//Calculate P1 DPad inputs
	if (P1_INPUT_LEFT) 
	{
		P1_HORIZONTAL_AXIS -= 1.0f;
		if (P1_HORIZONTAL_AXIS < -1.0f)
		{
			P1_HORIZONTAL_AXIS = -1.0f;
		}
	}
	if (P1_INPUT_RIGHT)
	{
		P1_HORIZONTAL_AXIS += 1.0f;
		if (P1_HORIZONTAL_AXIS > 1.0f)
		{
			P1_HORIZONTAL_AXIS = 1.0f;
		}
	}
	if (P1_INPUT_UP)
	{
		P1_VERTICAL_AXIS += 1.0f;
		if (P1_VERTICAL_AXIS > 1.0f)
		{
			P1_VERTICAL_AXIS = 1.0f;
		}
	}
	if (P1_INPUT_DOWN)
	{
		P1_VERTICAL_AXIS -= 1.0f;
		if (P1_VERTICAL_AXIS < -1.0f)
		{
			P1_VERTICAL_AXIS = -1.0f;
		}
	}
}

void UCharacterSelect::GetP2Inputs()
{
	//Set bool variables
	SetP2ButtonInputs();

	//Read P1 analog inputs
	P2Controller->GetInputAnalogStickState(EControllerAnalogStick::CAS_LeftStick, P2_HORIZONTAL_AXIS, P2_VERTICAL_AXIS);

	//Calculate P1 DPad inputs
	if (P2_INPUT_LEFT)
	{
		P2_HORIZONTAL_AXIS -= 1.0f;
		if (P2_HORIZONTAL_AXIS < -1.0f)
		{
			P2_HORIZONTAL_AXIS = -1.0f;
		}
	}
	if (P2_INPUT_RIGHT)
	{
		P2_HORIZONTAL_AXIS += 1.0f;
		if (P2_HORIZONTAL_AXIS > 1.0f)
		{
			P2_HORIZONTAL_AXIS = 1.0f;
		}
	}
	if (P2_INPUT_UP)
	{
		P2_VERTICAL_AXIS += 1.0f;
		if (P2_VERTICAL_AXIS > 1.0f)
		{
			P2_VERTICAL_AXIS = 1.0f;
		}
	}
	if (P2_INPUT_DOWN)
	{
		P2_VERTICAL_AXIS -= 1.0f;
		if (P2_VERTICAL_AXIS < -1.0f)
		{
			P2_VERTICAL_AXIS = -1.0f;
		}
	}
}

void UCharacterSelect::SetP1ButtonInputs() 
{
	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Down) || P1Controller->IsInputKeyDown(EKeys::S))
	{
		P1_INPUT_DOWN = true;
	}
	else 
	{
		P1_INPUT_DOWN = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Up) || P1Controller->IsInputKeyDown(EKeys::W))
	{
		P1_INPUT_UP = true;
	}
	else
	{
		P1_INPUT_UP = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Left) || P1Controller->IsInputKeyDown(EKeys::A))
	{
		P1_INPUT_LEFT = true;
	}
	else
	{
		P1_INPUT_LEFT = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Right) || P1Controller->IsInputKeyDown(EKeys::D))
	{
		P1_INPUT_RIGHT = true;
	}
	else
	{
		P1_INPUT_RIGHT = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_FaceButton_Right) || P1Controller->IsInputKeyDown(EKeys::K))
	{
		P1_INPUT_BACK = true;
	}
	else
	{
		P1_INPUT_BACK = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom) || P1Controller->IsInputKeyDown(EKeys::J))
	{
		P1_INPUT_ACCEPT = true;
	}
	else
	{
		P1_INPUT_ACCEPT = false;
	}

	if (P1Controller->IsInputKeyDown(EKeys::Gamepad_Special_Right) || P1Controller->IsInputKeyDown(EKeys::Escape))
	{
		P1_INPUT_START = true;
	}
	else
	{
		P1_INPUT_START = false;
	}
}

void UCharacterSelect::SetP2ButtonInputs()
{
	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Down) || P2Controller->IsInputKeyDown(EKeys::Down))
	{
		P2_INPUT_DOWN = true;
	}
	else
	{
		P2_INPUT_DOWN = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Up) || P2Controller->IsInputKeyDown(EKeys::Up))
	{
		P2_INPUT_UP = true;
	}
	else
	{
		P2_INPUT_UP = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Left) || P2Controller->IsInputKeyDown(EKeys::Left))
	{
		P2_INPUT_LEFT = true;
	}
	else
	{
		P2_INPUT_LEFT = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_DPad_Right) || P2Controller->IsInputKeyDown(EKeys::Right))
	{
		P2_INPUT_RIGHT = true;
	}
	else
	{
		P2_INPUT_RIGHT = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_FaceButton_Right) || P2Controller->IsInputKeyDown(EKeys::N))
	{
		P2_INPUT_BACK = true;
	}
	else
	{
		P2_INPUT_BACK = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom) || P2Controller->IsInputKeyDown(EKeys::M))
	{
		P2_INPUT_ACCEPT = true;
	}
	else
	{
		P2_INPUT_ACCEPT = false;
	}

	if (P2Controller->IsInputKeyDown(EKeys::Gamepad_Special_Right) || P2Controller->IsInputKeyDown(EKeys::Enter))
	{
		P2_INPUT_START = true;
	}
	else
	{
		P2_INPUT_START = false;
	}
}