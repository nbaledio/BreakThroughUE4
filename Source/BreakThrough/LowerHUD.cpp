// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "LowerHUD.h"
#include "Slate.h"


void ULowerHUD::UpdateLowerHUD(ABTCharacterBase* Player1, ABTCharacterBase* Player2)
{
	//Update resolve bars
	P1DurabilityBar->SetPercent((static_cast<float>((static_cast<float>(Player1->CurrentState.Resolve) - 1) * 1000.0f + static_cast<float>(Player1->CurrentState.Durability)) / 4000.0f));
	P2DurabilityBar->SetPercent((static_cast<float>((static_cast<float>(Player2->CurrentState.Resolve) - 1) * 1000.0f + static_cast<float>(Player2->CurrentState.Durability)) / 4000.0f));

	//Set resolve bars color based on amount
	//Set Player1 resolve color
	if (Player1->CurrentState.Resolve == 4 && Player1->CurrentState.Durability == 1000)
	{
		P1DurabilityBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 200.0f / 255.0f, 30.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (Player1->CurrentState.Resolve >= 2)
	{
		P1DurabilityBar->SetFillColorAndOpacity(FLinearColor(0.0f / 255.0f, 143.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (Player1->CurrentState.Resolve <= 1)
	{
		P1DurabilityBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 0.0f / 255.0f, 85.0f / 255.0f, 255.0f / 255.0f));
	}

	//Set Player2 resolve color
	if (Player2->CurrentState.Resolve == 4 && Player2->CurrentState.Durability == 1000)
	{
		P2DurabilityBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 200.0f / 255.0f, 30.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (Player2->CurrentState.Resolve >= 2)
	{
		P2DurabilityBar->SetFillColorAndOpacity(FLinearColor(0.0f / 255.0f, 143.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
	}
	else if (Player2->CurrentState.Resolve <= 1)
	{
		P2DurabilityBar->SetFillColorAndOpacity(FLinearColor(255.0f / 255.0f, 0.0f / 255.0f, 85.0f / 255.0f, 255.0f / 255.0f));
	}

}

void ULowerHUD::PlayBlackScreenFade(uint8 FramePlayTime, bool bReverse) 
{
	//Play fade in
	if (FramePlayTime >= 120 && !bReverse)
	{
		BlackScreen->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, ((static_cast<float>(FramePlayTime) - 120.0f) / 60.0f)));
	}
	//Play fade out
	else if (FramePlayTime >= 120 && bReverse)
	{
		BlackScreen->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, ((180.0f - static_cast<float>(FramePlayTime)) / 60.0f)));
	}
}


