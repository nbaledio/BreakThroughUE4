// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "CharacterSelect.h"

void UCharacterSelect::NativeConstruct() 
{
	Super::NativeConstruct();
	const UObject* world = (UObject*)GetWorld();

	//Set Controllers
	if (Gamemode == "CPU" || Gamemode == "Training") 
	{
		P1Controller = UGameplayStatics::GetPlayerController(world, 0);
		P2Controller = UGameplayStatics::GetPlayerController(world, 0);
		if (P1Side == "Left" && P2Side == "Right")
		{
			P2Cursor->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (P1Side == "Right" && P2Side == "Left")
		{
			P1Cursor->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (Gamemode == "VS") 
	{
		if (P1Side == "Left" && P2Side == "Right")
		{
			P1Controller = UGameplayStatics::GetPlayerController(world, 0);
			P2Controller = UGameplayStatics::GetPlayerController(world, 1);
		}
		else if (P1Side == "Right" && P2Side == "Left")
		{
			P1Controller = UGameplayStatics::GetPlayerController(world, 1);
			P2Controller = UGameplayStatics::GetPlayerController(world, 0);
		}
	}

	SetP1CharacterPortrait(-1);
	SetP2CharacterPortrait(-1);

	Headshots.Add(ACH_Headshot);
	Headshots.Add(DHA_Headshot);
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

	if (Gamemode == "CPU" || Gamemode == "Training") 
	{
		VsCPUMenuInteractions();
	}
	else if (Gamemode == "VS") 
	{
		Vs2PMenuInteractions();
	}
}

void UCharacterSelect::GetP1Inputs() 
{
	//Set bool variables
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

	//Read P2 analog inputs
	P2Controller->GetInputAnalogStickState(EControllerAnalogStick::CAS_LeftStick, P2_HORIZONTAL_AXIS, P2_VERTICAL_AXIS);

	//Calculate P2 DPad inputs
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

void UCharacterSelect::VsCPUMenuInteractions() 
{
	// P1 INPUTS // 
	if (!P1CharacterSelected)
	{
		//P1 Cursor Input
		if (P1Side == "Left")
		{
			FVector2D P1CursorCurrentPosition = Cast<UCanvasPanelSlot>(P1Cursor->Slot)->GetPosition();
			Cast<UCanvasPanelSlot>(P1Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P1CursorCurrentPosition.X + (P1_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P1CursorCurrentPosition.Y - (P1_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));
			//Set P1 Character Portrait based on hovered character
			P1HoveredCharacter = P1CursorCollisionDetection();
		}
		else if(P1Side == "Right")
		{
			FVector2D P1CursorCurrentPosition = Cast<UCanvasPanelSlot>(P2Cursor->Slot)->GetPosition();
			Cast<UCanvasPanelSlot>(P2Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P1CursorCurrentPosition.X + (P1_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P1CursorCurrentPosition.Y - (P1_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));
			//Set P1 Character Portrait based on hovered character
			P1HoveredCharacter = P2CursorCollisionDetection();
		}

		//Confirm character if currently hovering one
		if (P1_INPUT_ACCEPT && P1HoveredCharacter != -1)
		{
			P1Character = P1HoveredCharacter;
			P1CharacterSelected = true;
			if (P1Side == "Left")
			{
				P1CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			else if (P1Side == "Right")
			{
				P2CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			P1AcceptConfirmInput = false;
			//Display P1 Character Model
		}

		//Display return to main menu
		if (P1_INPUT_BACK && P1AcceptBackInput)
		{
			
		}
		else if (!P1_INPUT_BACK)
		{
			P1AcceptBackInput = true;
		}
	}

	//P1 Color Select options
	if (P1CharacterSelected && !P1Ready)
	{
		//Check for accept input
		if (P1_INPUT_ACCEPT && P1AcceptConfirmInput)
		{
			P1Ready = true;
			P2AcceptConfirmInput = false;
			if (P1Side == "Left") 
			{
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
				P2Cursor->SetVisibility(ESlateVisibility::Visible);
			}
			else if (P1Side == "Right") 
			{
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
				P1Cursor->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else if (!P1_INPUT_ACCEPT)
		{
			P1AcceptConfirmInput = true;
		}
		//Check for back input
		if (P1_INPUT_BACK && P1AcceptBackInput)
		{
			P1AcceptBackInput = false;
			P1Color = 1;
			P1CharacterSelected = false;
			if (P1Side == "Left")
			{
				P1CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (P1Side == "Right") 
			{
				P2CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
			//Hide P1 Character Model
		}
		else if (!P1_INPUT_BACK)
		{
			P1AcceptBackInput = true;
		}

		//Cycle through colors
		if ((P1_INPUT_LEFT || P1_HORIZONTAL_AXIS < -0.9f) && P1AcceptScrollInput)
		{
			P1Color--;
			P1AcceptScrollInput = false;
		}
		else if ((P1_INPUT_RIGHT || P1_HORIZONTAL_AXIS > 0.9f) && P1AcceptScrollInput)
		{
			P1Color++;
			P1AcceptScrollInput = false;
		}
		else if (!P1_INPUT_LEFT && !P1_INPUT_RIGHT && P1_HORIZONTAL_AXIS > -0.9f && P1_HORIZONTAL_AXIS < 0.9f)
		{
			P1AcceptScrollInput = true;
		}

		//Prevent P1 from having the same character color as P2
		if (P1Character == P2Character && P1Color == P2Color && P2Ready && P1CharacterSelected)
		{
			P1Color++;
		}

		//Loop color number
		if (P1Color < 1)
		{
			P1Color = 5;
		}
		if (P1Color > 5)
		{
			P1Color = 1;
		}

		//Set P1 Color Text
		if (P1Side == "Left") 
		{
			P1ColorText->SetText(FText::FromString("< " + FString::FromInt(P1Color) + " >"));
		}
		else if (P1Side == "Right")
		{
			P2ColorText->SetText(FText::FromString("< " + FString::FromInt(P1Color) + " >"));
		}
	}

	// CPU INPUTS //
	if (!P2CharacterSelected && P1Ready)
	{
		//P2 Cursor Input
		if (P2Side == "Right") 
		{
			FVector2D P2CursorCurrentPosition = Cast<UCanvasPanelSlot>(P2Cursor->Slot)->GetPosition();
			Cast<UCanvasPanelSlot>(P2Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P2CursorCurrentPosition.X + (P1_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P2CursorCurrentPosition.Y - (P1_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));
			//Set P2 Character Portrait based on hovered character
			P2HoveredCharacter = P2CursorCollisionDetection();
		}
		else if (P2Side == "Left") 
		{
			FVector2D P2CursorCurrentPosition = Cast<UCanvasPanelSlot>(P1Cursor->Slot)->GetPosition();
			Cast<UCanvasPanelSlot>(P1Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P2CursorCurrentPosition.X + (P1_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P2CursorCurrentPosition.Y - (P1_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));
			//Set P2 Character Portrait based on hovered character
			P2HoveredCharacter = P1CursorCollisionDetection();
		}

		//Confirm character if currently hovering one
		if (P1_INPUT_ACCEPT && P2HoveredCharacter != -1 && P2AcceptConfirmInput)
		{
			P2Character = P2HoveredCharacter;
			P2CharacterSelected = true;
			if (P2Side == "Right") 
			{
				P2CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			else if (P2Side == "Left") 
			{
				P1CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			P2AcceptConfirmInput = false;
			//Display P2 Character Model
		}
		else if (!P1_INPUT_ACCEPT)
		{
			P2AcceptConfirmInput = true;
		}

		//Return to P1 Color Select if Back is input
		if (P1_INPUT_BACK && P2AcceptBackInput)
		{
			if (P2Side == "Right") 
			{
				P2Cursor->SetVisibility(ESlateVisibility::Hidden);
				SetP2CharacterPortrait(-1);
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			else if (P2Side == "Left") 
			{
				P1Cursor->SetVisibility(ESlateVisibility::Hidden);
				SetP1CharacterPortrait(-1);
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			P1AcceptBackInput = false;
			P1Ready = false;
		}
		else if (!P1_INPUT_BACK)
		{
			P2AcceptBackInput = true;
		}
	}

	//P2 Color Select Options
	if (P2CharacterSelected && !P2Ready)
	{
		//Check for accept input
		if (P1_INPUT_ACCEPT && P2AcceptConfirmInput)
		{
			P2Ready = true;
			if (P2Side == "Right") 
			{
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (P2Side == "Left") 
			{
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else if (!P1_INPUT_ACCEPT)
		{
			P2AcceptConfirmInput = true;
		}
		//Check for back input
		if (P1_INPUT_BACK && P2AcceptBackInput)
		{
			P1AcceptBackInput = false;
			P2AcceptBackInput = false;
			P2Color = 1;
			P2CharacterSelected = false;
			if (P2Side == "Right") 
			{
				P2CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (P2Side == "Left") 
			{
				P1CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			}
			//Hide P2 Character Model
		}
		else if (!P1_INPUT_BACK)
		{
			P2AcceptBackInput = true;
		}

		//Cycle through colors
		if ((P1_INPUT_LEFT || P1_HORIZONTAL_AXIS < -0.9f) && P2AcceptScrollInput)
		{
			P2Color--;
			P2AcceptScrollInput = false;
			if (P1Character == P2Character && P1Color == P2Color) 
			{
				P2Color--;
			}
		}
		else if ((P1_INPUT_RIGHT || P1_HORIZONTAL_AXIS > 0.9f) && P2AcceptScrollInput)
		{
			P2Color++;
			P2AcceptScrollInput = false;
		}
		else if (!P1_INPUT_LEFT && !P1_INPUT_RIGHT && P1_HORIZONTAL_AXIS > -0.9f && P1_HORIZONTAL_AXIS < 0.9f)
		{
			P2AcceptScrollInput = true;
		}

		//Prevent P2 from having the same character color as P1
		if (P1Character == P2Character && P1Color == P2Color && P1Ready && P2CharacterSelected)
		{
			P2Color++;
		}

		//Loop color number
		if (P2Color < 1)
		{
			P2Color = 5;
			if (P1Color == P2Color && P1Character == P2Character && P1Ready) 
			{
				P2Color--;
			}
		}
		if (P2Color > 5)
		{
			P2Color = 1;
			if (P1Color == P2Color && P1Character == P2Character && P1Ready)
			{
				P2Color++;
			}
		}

		//Set P2 Color Text
		if (P2Side == "Right") 
		{
			P2ColorText->SetText(FText::FromString("< " + FString::FromInt(P2Color) + " >"));
		}
		else if (P2Side == "Left") 
		{
			P1ColorText->SetText(FText::FromString("< " + FString::FromInt(P2Color) + " >"));
		}
	}

	if (P2Ready)
	{
		//Return to P2 Color Select
		if (P1_INPUT_BACK)
		{
			P2AcceptBackInput = false;
			P2Ready = false;
			if (P2Side == "Right") 
			{
				P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
			else if (P2Side == "Left") 
			{
				P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UCharacterSelect::Vs2PMenuInteractions()
{
	// P1 INPUTS //
	if (!P1CharacterSelected)
	{
		//P1 Cursor Input
		FVector2D P1CursorCurrentPosition = Cast<UCanvasPanelSlot>(P1Cursor->Slot)->GetPosition();
		Cast<UCanvasPanelSlot>(P1Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P1CursorCurrentPosition.X + (P1_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P1CursorCurrentPosition.Y - (P1_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));

		//Set P1 Character Portrait based on hovered character
		int character = P1CursorCollisionDetection();

		//Confirm character if currently hovering one
		if (P1_INPUT_ACCEPT && character != -1)
		{
			P1Character = character;
			P1CharacterSelected = true;
			P1CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
			P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			P1AcceptConfirmInput = false;
			//Display P1 Character Model
		}

		if (P1_INPUT_BACK && P1AcceptBackInput)
		{
			//Display return to main menu
		}
		else if (!P1_INPUT_BACK)
		{
			P1AcceptBackInput = true;
		}
	}

	if (P1CharacterSelected && !P1Ready)
	{
		//Check for accept input
		if (P1_INPUT_ACCEPT && P1AcceptConfirmInput)
		{
			P1Ready = true;
			P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (!P1_INPUT_ACCEPT)
		{
			P1AcceptConfirmInput = true;
		}
		//Check for back input
		if (P1_INPUT_BACK && P1AcceptBackInput)
		{
			P1AcceptBackInput = false;
			P1Character = -1;
			P1Color = 1;
			P1CharacterSelected = false;
			P1CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
			P1ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			//Hide P1 Character Model
		}
		else if (!P1_INPUT_BACK)
		{
			P1AcceptBackInput = true;
		}

		//Cycle through colors
		if ((P1_INPUT_LEFT || P1_HORIZONTAL_AXIS < -0.9f) && P1AcceptScrollInput)
		{
			P1Color--;
			P1AcceptScrollInput = false;
			if (P1Character == P2Character && P1Color == P2Color && P2Ready)
			{
				P1Color--;
			}
		}
		else if ((P1_INPUT_RIGHT || P1_HORIZONTAL_AXIS > 0.9f) && P1AcceptScrollInput)
		{
			P1Color++;
			P1AcceptScrollInput = false;
		}
		else if (!P1_INPUT_LEFT && !P1_INPUT_RIGHT && P1_HORIZONTAL_AXIS > -0.9f && P1_HORIZONTAL_AXIS < 0.9f)
		{
			P1AcceptScrollInput = true;
		}

		//Prevent P1 from having the same character color as P2
		if (P1Character == P2Character && P1Color == P2Color && P2Ready)
		{
			P1Color++;
		}

		//Loop color number
		if (P1Color < 1)
		{
			P1Color = 5;
			if (P1Color == P2Color && P1Character == P2Character && P2Ready)
			{
				P1Color--;
			}
		}
		if (P1Color > 5)
		{
			P1Color = 1;
			if (P1Character == P2Character && P1Color == P2Color && P2Ready)
			{
				P2Color++;
			}
		}

		//Set P1 Color Text
		P1ColorText->SetText(FText::FromString("< " + FString::FromInt(P1Color) + " >"));
	}

	if (P1Ready)
	{
		if (P1_INPUT_BACK)
		{
			P1AcceptBackInput = false;
			P1Ready = false;
			P1ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// P2 INPUTS //
	if (!P2CharacterSelected)
	{
		//P2 Cursor Input
		FVector2D P2CursorCurrentPosition = Cast<UCanvasPanelSlot>(P2Cursor->Slot)->GetPosition();
		Cast<UCanvasPanelSlot>(P2Cursor->Slot)->SetPosition(FVector2D(FMath::Clamp(P2CursorCurrentPosition.X + (P2_HORIZONTAL_AXIS * CursorSpeed), -1050.0f, 750.0f), FMath::Clamp(P2CursorCurrentPosition.Y - (P2_VERTICAL_AXIS * CursorSpeed), -650.0f, 325.0f)));

		//Set P2 Character Portrait based on hovered character
		int character = P2CursorCollisionDetection();

		//Confirm character if currently hovering one
		if (P2_INPUT_ACCEPT && character != -1)
		{
			P2Character = character;
			P2CharacterSelected = true;
			P2CharacterPortrait->SetVisibility(ESlateVisibility::Hidden);
			P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
			P2AcceptConfirmInput = false;
			//Display P2 Character Model
		}

		if (P2_INPUT_BACK && P2AcceptBackInput)
		{
			//Display return to main menu
		}
		else if (!P2_INPUT_BACK)
		{
			P2AcceptBackInput = true;
		}
	}

	if (P2CharacterSelected && !P2Ready)
	{
		//Check for accept input
		if (P2_INPUT_ACCEPT && P2AcceptConfirmInput)
		{
			P2Ready = true;
			P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (!P2_INPUT_ACCEPT)
		{
			P2AcceptConfirmInput = true;
		}
		//Check for back input
		if (P2_INPUT_BACK && P2AcceptBackInput)
		{
			P2AcceptBackInput = false;
			P2Character = -1;
			P2Color = 1;
			P2CharacterSelected = false;
			P2CharacterPortrait->SetVisibility(ESlateVisibility::Visible);
			P2ColorSelectMenu->SetVisibility(ESlateVisibility::Hidden);
			//Hide P2 Character Model
		}
		else if (!P2_INPUT_BACK)
		{
			P2AcceptBackInput = true;
		}

		//Cycle through colors
		if ((P2_INPUT_LEFT || P2_HORIZONTAL_AXIS < -0.9f) && P2AcceptScrollInput)
		{
			P2Color--;
			P2AcceptScrollInput = false;
			if (P1Character == P2Character && P1Color == P2Color && P1Ready)
			{
				P2Color--;
			}
		}
		else if ((P2_INPUT_RIGHT || P2_HORIZONTAL_AXIS > 0.9f) && P2AcceptScrollInput)
		{
			P2Color++;
			P2AcceptScrollInput = false;
		}
		else if (!P2_INPUT_LEFT && !P2_INPUT_RIGHT && P2_HORIZONTAL_AXIS > -0.9f && P2_HORIZONTAL_AXIS < 0.9f)
		{
			P2AcceptScrollInput = true;
		}

		//Prevent P2 from having the same character color as P1
		if (P1Character == P2Character && P1Color == P2Color && P1Ready)
		{
			P2Color++;
		}

		//Loop color number
		if (P2Color < 1)
		{
			P2Color = 5;
			if (P1Color == P2Color && P1Character == P2Character && P1Ready)
			{
				P2Color--;
			}
		}
		if (P2Color > 5)
		{
			P2Color = 1;
			if (P1Character == P2Character && P1Color == P2Color && P1Ready)
			{
				P2Color++;
			}
		}

		//Set P1 Color Text
		P2ColorText->SetText(FText::FromString("< " + FString::FromInt(P2Color) + " >"));
	}

	if (P2Ready)
	{
		if (P2_INPUT_BACK)
		{
			P2AcceptBackInput = false;
			P2Ready = false;
			P2ColorSelectMenu->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

int UCharacterSelect::P1CursorCollisionDetection()
{
	FVector2D P1CursorCurrentPosition = Cast<UCanvasPanelSlot>(P1Cursor->Slot)->GetPosition();
	for (int i = 0; i < Headshots.Num(); i++) 
	{
		FVector2D HeadshotPos = Cast<UCanvasPanelSlot>(Headshots[i]->Slot)->GetPosition();
		float dx = P1CursorCurrentPosition.X - HeadshotPos.X - 250;
		float dy = P1CursorCurrentPosition.Y - HeadshotPos.Y - 250;
		float distance = FMath::Sqrt(dx * dx + dy * dy);
		if (distance < 28.0f + 50.0f)
		{
			SetP1CharacterPortrait(i);
			return i;
		}
	}
	SetP1CharacterPortrait(-1);
	return -1;
}

int UCharacterSelect::P2CursorCollisionDetection()
{
	FVector2D P2CursorCurrentPosition = Cast<UCanvasPanelSlot>(P2Cursor->Slot)->GetPosition();
	for (int i = 0; i < Headshots.Num(); i++)
	{
		FVector2D HeadshotPos = Cast<UCanvasPanelSlot>(Headshots[i]->Slot)->GetPosition();
		float dx = P2CursorCurrentPosition.X - HeadshotPos.X - 250;
		float dy = P2CursorCurrentPosition.Y - HeadshotPos.Y - 250;
		float distance = FMath::Sqrt(dx * dx + dy * dy);
		if (distance < 28.0f + 50.0f)
		{
			SetP2CharacterPortrait(i);
			return i;
		}
	}
	SetP2CharacterPortrait(-1);
	return -1;
}

void UCharacterSelect::SetP1CharacterPortrait(int CharacterCode) 
{
	switch (CharacterCode)
	{
	case 0:
		P1CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		P1CharacterPortrait->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *FString("/Game/CharacterSelect/Textures/Achealis"))));
		P1CharacterName->SetText(FText::FromString("Achealis Thorne"));
		break;
	case 1:
		P1CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		P1CharacterPortrait->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *FString("/Game/CharacterSelect/Textures/Dhalia"))));
		P1CharacterName->SetText(FText::FromString("Dhalia Thorne"));
		break;
	default:
		P1CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		P1CharacterPortrait->SetBrushFromTexture(NULL);
		P1CharacterName->SetText(FText::FromString(""));
	}
}

void UCharacterSelect::SetP2CharacterPortrait(int CharacterCode)
{
	switch (CharacterCode)
	{
	case 0:
		P2CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		P2CharacterPortrait->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *FString("/Game/CharacterSelect/Textures/Achealis"))));
		P2CharacterName->SetText(FText::FromString("Achealis Thorne"));
		break;
	case 1:
		P2CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		P2CharacterPortrait->SetBrushFromTexture(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *FString("/Game/CharacterSelect/Textures/Dhalia"))));
		P2CharacterName->SetText(FText::FromString("Dhalia Thorne"));
		break;
	default:
		P2CharacterPortrait->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		P2CharacterPortrait->SetBrushFromTexture(NULL);
		P2CharacterName->SetText(FText::FromString(""));
	}
}