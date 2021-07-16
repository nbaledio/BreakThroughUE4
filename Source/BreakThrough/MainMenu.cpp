// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "MainMenu.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//Set controllers
	P1Controller = UGameplayStatics::GetPlayerController((UObject*)GetWorld(), 0);
	P2Controller = UGameplayStatics::GetPlayerController((UObject*)GetWorld(), 1);

	//Play Animations
	PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	PlayAnimation(FlowerAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward, 0.1f, false);
	PlayAnimation(CloudAnimation, 0.0f, 0, EUMGSequencePlayMode::Forward, 0.01f, false);
}

void UMainMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

	//Set menu interactions based on current menu
	switch (menuIndex) 
	{
	case 0:
		MainMenuInteractions();
		break;
	case 1:
		PlayLocalMenuInteractions();
		break;
	case 2:
		OptionsMenuInteractions();
		break;
	case 3:
		SideSelectMenuInteractions();
		break;
	}
}

void UMainMenu::GetP1Inputs()
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

void UMainMenu::GetP2Inputs()
{

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
}

void UMainMenu::MainMenuInteractions() 
{
	//Scroll through Options
	if (P1_INPUT_DOWN && P1AcceptVerticalInput) 
	{
		P1AcceptVerticalInput = false;
		optionIndex++;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (P1_INPUT_UP && P1AcceptVerticalInput) 
	{
		P1AcceptVerticalInput = false;
		optionIndex--;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if(!P1_INPUT_DOWN && !P1_INPUT_UP)
	{
		P1AcceptVerticalInput = true;
	}

	//Loop option index
	if (optionIndex > 3)
	{
		optionIndex = 0;
	}
	else if (optionIndex < 0)
	{
		optionIndex = 3;
	}

	//Check for confirm input
	if (P1_INPUT_ACCEPT && P1AcceptConfirmInput)
	{
		P1AcceptConfirmInput = false;
		switch (optionIndex) 
		{
		case 0:
			PlayLocalMenu->SetVisibility(ESlateVisibility::Visible);
			MainMenu->SetVisibility(ESlateVisibility::Hidden);
			menuIndex = 1;
			optionIndex = 0;
			PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
			break;
		case 1:
			//Enter online mode
			break;
		case 2:
			menuIndex = 2;
			OptionsMenu->SetVisibility(ESlateVisibility::Visible);
			MainMenu->SetVisibility(ESlateVisibility::Hidden);
			optionIndex = 0;
			PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
			break;
		case 3:
			//Return to title
			break;
		}
	}
	else if (!P1_INPUT_ACCEPT) 
	{
		P1AcceptConfirmInput = true;
	}

	//Set marker
	SetMarker(optionIndex);
}

void UMainMenu::PlayLocalMenuInteractions()
{
	//Scroll through Options
	if (P1_INPUT_DOWN && P1AcceptVerticalInput)
	{
		P1AcceptVerticalInput = false;
		optionIndex++;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (P1_INPUT_UP && P1AcceptVerticalInput)
	{
		P1AcceptVerticalInput = false;
		optionIndex--;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (!P1_INPUT_DOWN && !P1_INPUT_UP)
	{
		P1AcceptVerticalInput = true;
	}

	//Loop option index
	if (optionIndex > 4)
	{
		optionIndex = 0;
	}
	else if (optionIndex < 0)
	{
		optionIndex = 4;
	}

	//Check for confirm input
	if (P1_INPUT_ACCEPT && P1AcceptConfirmInput)
	{
		P1AcceptConfirmInput = false;
		switch (optionIndex)
		{
		case 0:
			StopAnimation(SideSelectSlide);
			PlayAnimation(SideSelectSlide, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
			P1ControllerIcon->SetVisibility(ESlateVisibility::Visible);
			P2ControllerIcon->SetVisibility(ESlateVisibility::Visible);
			P1CPUText->SetVisibility(ESlateVisibility::Hidden);
			P2CPUText->SetVisibility(ESlateVisibility::Hidden);
			StopAnimation(P1CToL);
			StopAnimation(P1CToR);
			StopAnimation(P2CToL);
			StopAnimation(P2CToR);
			PlayAnimation(ResetControllerPositions, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f, false);
			menuIndex = 3;
			Gamemode = "VS";
			break;
		case 1:
			StopAnimation(SideSelectSlide);
			PlayAnimation(SideSelectSlide, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
			P1ControllerIcon->SetVisibility(ESlateVisibility::Visible);
			P2ControllerIcon->SetVisibility(ESlateVisibility::Hidden);
			P1CPUText->SetVisibility(ESlateVisibility::Visible);
			P2CPUText->SetVisibility(ESlateVisibility::Visible);
			StopAnimation(P1CToL);
			StopAnimation(P1CToR);
			StopAnimation(P2CToL);
			StopAnimation(P2CToR);
			PlayAnimation(ResetControllerPositions, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f, false);
			menuIndex = 3;
			Gamemode = "CPU";
			break;
		case 2:
			StopAnimation(SideSelectSlide);
			PlayAnimation(SideSelectSlide, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
			P1ControllerIcon->SetVisibility(ESlateVisibility::Visible);
			P2ControllerIcon->SetVisibility(ESlateVisibility::Hidden);
			P1CPUText->SetVisibility(ESlateVisibility::Visible);
			P2CPUText->SetVisibility(ESlateVisibility::Visible);
			StopAnimation(P1CToL);
			StopAnimation(P1CToR);
			StopAnimation(P2CToL);
			StopAnimation(P2CToR);
			PlayAnimation(ResetControllerPositions, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f, false);
			menuIndex = 3;
			Gamemode = "Training";
			break;
		case 3:
			//Enter Tutorial Mode
			break;
		case 4:
			PlayLocalMenu->SetVisibility(ESlateVisibility::Hidden);
			MainMenu->SetVisibility(ESlateVisibility::Visible);
			menuIndex = 0;
			optionIndex = 0;
			PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
			break;
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
		PlayLocalMenu->SetVisibility(ESlateVisibility::Hidden);
		MainMenu->SetVisibility(ESlateVisibility::Visible);
		menuIndex = 0;
		optionIndex = 0;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (!P1_INPUT_BACK) 
	{
		P1AcceptBackInput = true;
	}

	//Set marker
	SetMarker(optionIndex);
}

void UMainMenu::OptionsMenuInteractions()
{
	//Scroll through Options
	if (P1_INPUT_DOWN && P1AcceptVerticalInput)
	{
		P1AcceptVerticalInput = false;
		optionIndex++;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (P1_INPUT_UP && P1AcceptVerticalInput)
	{
		P1AcceptVerticalInput = false;
		optionIndex--;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (!P1_INPUT_DOWN && !P1_INPUT_UP)
	{
		P1AcceptVerticalInput = true;
	}

	//Loop option index
	if (optionIndex > 3)
	{
		optionIndex = 0;
	}
	else if (optionIndex < 0)
	{
		optionIndex = 3;
	}

	//Check for confirm input
	if (P1_INPUT_ACCEPT && P1AcceptConfirmInput)
	{
		P1AcceptConfirmInput = false;
		switch (optionIndex)
		{
		case 0:
			//Open open sound options
			break;
		case 1:
			//Open graphics options
			break;
		case 2:
			//Open controller settings
			break;
		case 3:
			OptionsMenu->SetVisibility(ESlateVisibility::Hidden);
			MainMenu->SetVisibility(ESlateVisibility::Visible);
			menuIndex = 0;
			optionIndex = 2;
			break;
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
		OptionsMenu->SetVisibility(ESlateVisibility::Hidden);
		MainMenu->SetVisibility(ESlateVisibility::Visible);
		menuIndex = 0;
		optionIndex = 2;
		PlayAnimation(MarkerAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
	}
	else if (!P1_INPUT_BACK)
	{
		P1AcceptBackInput = true;
	}

	//Set marker
	SetMarker(optionIndex);
}

void UMainMenu::SideSelectMenuInteractions()
{
	//Check for P1 Input
	if (P1_INPUT_LEFT && P1AcceptHorizontalInput && !loading) 
	{
		P1AcceptHorizontalInput = false;
		if (P1Side > -1 && !(P2Side == -1 && P1Side == 0))
		{
			P1Side--;
			if (P1Side == -1) 
			{
				StopAnimation(P1CToL);
				StopAnimation(P1CToR);
				PlayAnimation(P1CToL, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
			}
			else if (P1Side == 0) 
			{
				StopAnimation(P1CToL);
				StopAnimation(P1CToR);
				PlayAnimation(P1CToR, 0.0f, 1, EUMGSequencePlayMode::Reverse, 8.0f, false);
			}
		}
	}
	else if (P1_INPUT_RIGHT && P1AcceptHorizontalInput && !loading)
	{
		P1AcceptHorizontalInput = false;
		if (P1Side < 1 && !(P2Side == 1 && P1Side == 0))
		{
			P1Side++;
			if (P1Side == 1)
			{
				StopAnimation(P1CToL);
				StopAnimation(P1CToR);
				PlayAnimation(P1CToR, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
			}
			else if (P1Side == 0)
			{
				StopAnimation(P1CToL);
				StopAnimation(P1CToR);
				PlayAnimation(P1CToL, 0.0f, 1, EUMGSequencePlayMode::Reverse, 8.0f, false);
			}
		}
	}
	else if (!P1_INPUT_LEFT && !P1_INPUT_RIGHT) 
	{
		P1AcceptHorizontalInput = true;
	}

	if (Gamemode == "VS" && !loading) 
	{
		//Check for P2 Input
		if (P2_INPUT_LEFT && P2AcceptHorizontalInput)
		{
			P2AcceptHorizontalInput = false;
			if (P2Side > -1 && !(P1Side == -1 && P2Side == 0))
			{
				P2Side--;
				if (P2Side == -1)
				{
					StopAnimation(P2CToL);
					StopAnimation(P2CToR);
					PlayAnimation(P2CToL, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
				}
				else if (P2Side == 0)
				{
					StopAnimation(P2CToL);
					StopAnimation(P2CToR);
					PlayAnimation(P2CToR, 0.0f, 1, EUMGSequencePlayMode::Reverse, 8.0f, false);
				}
			}
		}
		else if (P2_INPUT_RIGHT && P2AcceptHorizontalInput)
		{
			P2AcceptHorizontalInput = false;
			if (P2Side < 1 && !(P1Side == 1 && P2Side == 0))
			{
				P2Side++;
				if (P2Side == 1)
				{
					StopAnimation(P2CToL);
					StopAnimation(P2CToR);
					PlayAnimation(P2CToR, 0.0f, 1, EUMGSequencePlayMode::Forward, 8.0f, false);
				}
				else if (P2Side == 0)
				{
					StopAnimation(P2CToL);
					StopAnimation(P2CToR);
					PlayAnimation(P2CToL, 0.0f, 1, EUMGSequencePlayMode::Reverse, 8.0f, false);
				}
			}
		}
		else if (!P2_INPUT_LEFT && !P2_INPUT_RIGHT)
		{
			P2AcceptHorizontalInput = true;
		}
	}

	//Load character select if sides are chosen
	if (P1_INPUT_ACCEPT && P1AcceptConfirmInput && !loading) 
	{
		if (Gamemode == "VS" && P1Side != 0 && P2Side != 0 && P1Side != P2Side || ((Gamemode == "CPU" || Gamemode == "Training") && P1Side != 0))
		{
			P1AcceptConfirmInput = false;
			loading = true;
			PlayAnimation(LoadingIconSpin, 0.0f, 0, EUMGSequencePlayMode::Forward, 0.75f, false);
			PlayAnimation(LoadingScreenFadeIn, 0.0f, 1, EUMGSequencePlayMode::Forward, 5.0f, false);
			UGameplayStatics::OpenLevel(GetWorld(), "CharacterSelect");
		}
	}
	else if (!P1_INPUT_ACCEPT) 
	{
		P1AcceptConfirmInput = true;
	}

	//Check for back input
	if (P1_INPUT_BACK && P1AcceptBackInput && !loading)
	{
		P1AcceptBackInput = false;
		StopAnimation(P1CToL);
		StopAnimation(P1CToR);
		StopAnimation(P2CToL);
		StopAnimation(P2CToR);
		StopAnimation(SideSelectSlide);
		PlayAnimation(SideSelectSlide, 0.0f, 1, EUMGSequencePlayMode::Reverse, 8.0f, false);
		Gamemode = "";
		P1Side = 0;
		P2Side = 0;
		menuIndex = 1;
	}
	else if (!P1_INPUT_BACK)
	{
		P1AcceptBackInput = true;
	}
}

void UMainMenu::SetMarker(int currentOptionIndex) 
{
	//Set marker position
	switch (currentOptionIndex) 
	{
	case 0:
		Cast<UCanvasPanelSlot>(Marker->Slot)->SetPosition(FVector2D(-500.0f, 30.0f));
		break;
	case 1:
		Cast<UCanvasPanelSlot>(Marker->Slot)->SetPosition(FVector2D(-500.0f, 120.0f));
		break;
	case 2:
		Cast<UCanvasPanelSlot>(Marker->Slot)->SetPosition(FVector2D(-500.0f, 210.0f));
		break;
	case 3:
		Cast<UCanvasPanelSlot>(Marker->Slot)->SetPosition(FVector2D(-500.0f, 300.0f));
		break;
	case 4:
		Cast<UCanvasPanelSlot>(Marker->Slot)->SetPosition(FVector2D(-500.0f, 390.0f));
		break;
	}

	//Set marker size
	if (menuIndex == 0) 
	{
		switch (currentOptionIndex) 
		{
		case 0:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(0.0f, 0.0f), FVector2D(0.5f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 1:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(15.0f, 0.0f), FVector2D(0.55f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 2:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-50.0f, 0.0f), FVector2D(0.4f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 3:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-115.0f, 0.0f), FVector2D(0.25f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		}
	}
	else if (menuIndex == 1) 
	{
		switch (currentOptionIndex)
		{
		case 0:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(15.0f, 0.0f), FVector2D(0.55f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 1:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-50.0f, 0.0f), FVector2D(0.4f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 2:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-50.0f, 0.0f), FVector2D(0.4f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 3:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-50.0f, 0.0f), FVector2D(0.4f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 4:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-115.0f, 0.0f), FVector2D(0.25f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		}
	}
	else if (menuIndex == 2) 
	{
		switch (currentOptionIndex)
		{
		case 0:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(75.0f, 0.0f), FVector2D(0.7f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 1:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(125.0f, 0.0f), FVector2D(0.8f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 2:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(125.0f, 0.0f), FVector2D(0.8f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		case 3:
			Marker->SetRenderTransform(FWidgetTransform(FVector2D(-115.0f, 0.0f), FVector2D(0.25f, 0.5f), FVector2D(0.0f, 0.0f), 0.0f));
			break;
		}
	}
}
