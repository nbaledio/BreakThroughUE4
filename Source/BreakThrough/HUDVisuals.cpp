// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "HUDVisuals.h"

void UHUDVisuals::SetCharacterNames(FString P1Character, FString P2Character)
{
	//Read character info and set name here
}

void UHUDVisuals::UpdateGraphics(int32 time, int32 P1Health, int32 P1MaxHealth, int32 P1Resolve, int32 P1Durability, int32 P2Health, int32 P2MaxHealth, int32 P2Resolve, int32 P2Durability)
{
	TimerText->SetText(FText::AsNumber(time));
	P1HealthBar->SetPercent(static_cast<float>(P1Health) / static_cast<float>(P1MaxHealth));
	P2HealthBar->SetPercent(static_cast<float>(P2Health) / static_cast<float>(P2MaxHealth));
	P1ResolveBar->SetPercent((static_cast<float>(P1Resolve) - 1) * 1000.0f + static_cast<float>(P1Durability) / 4000.0f);
	P2ResolveBar->SetPercent((static_cast<float>(P2Resolve) - 1) * 1000.0f + static_cast<float>(P2Durability) / 4000.0f);
}

void UHUDVisuals::UpdateUpperHUD(int32 time, int32 P1Health, int32 P1MaxHealth, int32 P2Health, int32 P2MaxHealth)
{
	//Update timer
	TimerText->SetText(FText::AsNumber(time));

	//Update health bars
	P1HealthBar->SetPercent(static_cast<float>(P1Health) / static_cast<float>(P1MaxHealth));
	P2HealthBar->SetPercent(static_cast<float>(P2Health) / static_cast<float>(P2MaxHealth));

	//Calculate current health percents for health colors 
	float P1CurrentHealthPercent = static_cast<float>(P1Health) / static_cast<float>(P1MaxHealth);
	float P2CurrentHealthPercent = static_cast<float>(P2Health) / static_cast<float>(P2MaxHealth);

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


	//Set combo count
};
void UHUDVisuals::UpdateLowerHUD(int32 P1Resolve, int32 P1Durability, int32 P2Resolve, int32 P2Durability)
{
	UE_LOG(LogTemp, Warning, TEXT("%f"), ((static_cast<float>(P2Resolve) * 1000.0f + static_cast<float>(P2Durability)) / 4000.0f));
	P1ResolveBar->SetPercent((static_cast<float>((static_cast<float>(P1Resolve) - 1) * 1000.0f + static_cast<float>(P1Durability)) / 4000.0f));
	P2ResolveBar->SetPercent((static_cast<float>((static_cast<float>(P2Resolve) - 1) * 1000.0f + static_cast<float>(P2Durability)) / 4000.0f));
};
