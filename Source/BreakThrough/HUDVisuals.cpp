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
	P1ResolveBar->SetPercent(static_cast<float>(P1Resolve) * 1000.0f + static_cast<float>(P1Durability) / 4000.0f);
	P2ResolveBar->SetPercent(static_cast<float>(P2Resolve) * 1000.0f + static_cast<float>(P2Durability) / 4000.0f);
}

void UHUDVisuals::UpdateUpperHUD(int32 time, int32 P1Health, int32 P1MaxHealth, int32 P2Health, int32 P2MaxHealth)
{
	TimerText->SetText(FText::AsNumber(time));
	P1HealthBar->SetPercent(static_cast<float>(P1Health) / static_cast<float>(P1MaxHealth));
	P2HealthBar->SetPercent(static_cast<float>(P2Health) / static_cast<float>(P2MaxHealth));
};
void UHUDVisuals::UpdateLowerHUD(int32 P1Resolve, int32 P1Durability, int32 P2Resolve, int32 P2Durability)
{
	P1ResolveBar->SetPercent(static_cast<float>(((static_cast<float>(P1Resolve) * 1000.0f + static_cast<float>(P1Durability)) - 1000.0f) / 4000.0f));
	P2ResolveBar->SetPercent(static_cast<float>(((static_cast<float>(P2Resolve) * 1000.0f + static_cast<float>(P2Durability)) - 1000.0f) / 4000.0f));
};
