// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "HUDVisuals.h"
#include "Slate.h"

void UHUDVisuals::SetCharacterNames(FString P1Character, FString P2Character)
{
	//Read character info and set name here
}

void UHUDVisuals::UpdateUpperHUD(int32 time, ABTCharacterBase* Player1, ABTCharacterBase* Player2)
{
	//Update timer
	TimerText->SetText(FText::AsNumber(time));

	//Update health bars
	P1HealthBar->SetPercent(static_cast<float>(Player1->CurrentState.Health) / static_cast<float>(Player1->MaxHealth));
	P2HealthBar->SetPercent(static_cast<float>(Player2->CurrentState.Health) / static_cast<float>(Player2->MaxHealth));

	//Calculate current health percents for health colors 
	float P1CurrentHealthPercent = static_cast<float>(Player1->CurrentState.Health) / static_cast<float>(Player1->MaxHealth);
	float P2CurrentHealthPercent = static_cast<float>(Player2->CurrentState.Health) / static_cast<float>(Player2->MaxHealth);

	//Set health bar colors
	if (P1CurrentHealthPercent <= 0.25f)
	{
		P1HealthBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 75.0f / 255.0f, 98.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (P1CurrentHealthPercent <= 0.5f)
	{
		P1HealthBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 223.0f / 255.0f, 105.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (P1CurrentHealthPercent < 1.0f)
	{
		P1HealthBar->SetFillColorAndOpacity(FLinearColor(93.0f / 255.0f, 255.0f / 255.0f, 175.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (P1CurrentHealthPercent == 1.0f)
	{
		P1HealthBar->SetFillColorAndOpacity(FLinearColor(170.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
	}

	if (P2CurrentHealthPercent <= 0.25f)
	{
		P2HealthBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 75.0f / 255.0f, 98.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (P2CurrentHealthPercent <= 0.5f)
	{
		P2HealthBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 223.0f / 255.0f, 105.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (P2CurrentHealthPercent < 1.0f)
	{
		P2HealthBar->SetFillColorAndOpacity(FLinearColor(93.0f/255.0f, 255.0f/255.0f, 175.0f/255.0f, 255.0f / 255.0f));
	}
	else if (P2CurrentHealthPercent == 1.0f)
	{
		P2HealthBar->SetFillColorAndOpacity(FLinearColor(170.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
	}

	//Set damage health bar once a character's hitstun ends
	if (Player1->CurrentState.HitStun == 0 || Player1->CurrentState.Health == 0) 
	{
		P1HealthRedBar->SetPercent(P1CurrentHealthPercent);
	}
	if (Player2->CurrentState.HitStun == 0 || Player2->CurrentState.Health == 0)
	{
		P2HealthRedBar->SetPercent(P2CurrentHealthPercent);
	}

	//Set combo text
	//Set Player1 Combo Text
	if (Player1->CurrentState.ComboCount > 1 && Player2->CurrentState.Health != 0) 
	{
		P1ComboCountNumber->SetText(FText::AsNumber(Player1->CurrentState.ComboCount));
		P1ComboCountHitsText->SetVisibility(ESlateVisibility::Visible);
		//Set true combo colors
		if (Player1->CurrentState.bTrueCombo) 
		{
			P1ComboCountNumber->SetColorAndOpacity(FSlateColor(FLinearColor(0.625f, 0.0f, 0.0f, 1.0f)));
			P1ComboCountHitsText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
			P1ComboTimerBar->SetFillColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else 
		{
			P1ComboCountNumber->SetColorAndOpacity(FSlateColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.0f)));
			P1ComboCountHitsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.95f, 0.95f, 1.0f)));
			P1ComboTimerBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
		}
	}
	else if (Player1->CurrentState.ComboCount == 1)
	{
		P1ComboTimerBar->SetFillColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else 
	{
		P1ComboCountHitsText->SetVisibility(ESlateVisibility::Hidden);
		P1ComboCountNumber->SetText(FText::FromString(("")));
	}

	//Set Player2 Combo Text
	if (Player2->CurrentState.ComboCount > 1 && Player1->CurrentState.Health != 0)
	{
		P2ComboCountNumber->SetText(FText::AsNumber(Player2->CurrentState.ComboCount));
		P2ComboCountHitsText->SetVisibility(ESlateVisibility::Visible);
		//Set true combo colors
		if (Player2->CurrentState.bTrueCombo)
		{
			P2ComboCountNumber->SetColorAndOpacity(FSlateColor(FLinearColor(0.625f, 0.0f, 0.0f, 1.0f)));
			P2ComboCountHitsText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
			P2ComboTimerBar->SetFillColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			P2ComboCountNumber->SetColorAndOpacity(FSlateColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.0f)));
			P2ComboCountHitsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.95f, 0.95f, 1.0f)));
			P2ComboTimerBar->SetFillColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.0f));
		}
	}
	else if (Player2->CurrentState.ComboCount == 1) 
	{
		P2ComboTimerBar->SetFillColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		P2ComboCountHitsText->SetVisibility(ESlateVisibility::Hidden);
		P2ComboCountNumber->SetText(FText::FromString(("")));
	}

	//Set combo timers
	//Set Player1 combo timer
	if (Player2->CurrentState.HitStun > 0 && Player2->CurrentState.Health != 0)
	{
		P1ComboTimer->SetVisibility(ESlateVisibility::Visible);
		if (Player2->CurrentState.HitStun > 20)
		{
			P1ComboTimerBar->SetPercent(1.0f);
		}
		else 
		{
			P1ComboTimerBar->SetPercent(static_cast<float>(Player2->CurrentState.HitStun) / 20.0f);
		}
	}
	else 
	{
		P1ComboTimer->SetVisibility(ESlateVisibility::Hidden);
		P1ComboTimerBar->SetPercent(0.0f);
	}

	//Set Player2 combo timer
	if (Player1->CurrentState.HitStun > 0 && Player1->CurrentState.Health != 0)
	{
		P2ComboTimer->SetVisibility(ESlateVisibility::Visible);
		if (Player1->CurrentState.HitStun > 20)
		{
			P2ComboTimerBar->SetPercent(1.0f);
		}
		else
		{
			P2ComboTimerBar->SetPercent(static_cast<float>(Player1->CurrentState.HitStun) / 20.0f);
		}
	}
	else
	{
		P2ComboTimer->SetVisibility(ESlateVisibility::Hidden);
		P2ComboTimerBar->SetPercent(0.0f);
	}

};
void UHUDVisuals::UpdateLowerHUD(ABTCharacterBase* Player1, ABTCharacterBase* Player2)
{
	P1ResolveBar->SetPercent((static_cast<float>((static_cast<float>(Player1->CurrentState.Resolve) - 1) * 1000.0f + static_cast<float>(Player1->CurrentState.Durability)) / 4000.0f));
	P2ResolveBar->SetPercent((static_cast<float>((static_cast<float>(Player2->CurrentState.Resolve) - 1) * 1000.0f + static_cast<float>(Player2->CurrentState.Durability)) / 4000.0f));
};
