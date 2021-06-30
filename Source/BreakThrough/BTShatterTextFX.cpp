// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTShatterTextFX.h"

// Sets default values
ABTShatterTextFX::ABTShatterTextFX()
{
	Text = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Text"));
	Text->SetupAttachment(RootComponent);
}

void ABTShatterTextFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight);

	CurrentState.Position.Y = FMath::Max(60.f, CurrentState.Position.Y);
}

void ABTShatterTextFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive)
	{
		if (CurrentState.FramePlayTime > 40)
			CurrentState.bIsActive = false;

		if (CurrentState.FramePlayTime > 30)
		{
			if (CurrentState.bFacingRight)
				CurrentState.Position.X += 20;
			else
				CurrentState.Position.X -= 20;
		}
		else
			if (CurrentState.bFacingRight)
				CurrentState.Position.X += 1;
			else
				CurrentState.Position.X -= 1;

	}
}

void ABTShatterTextFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();

	if (CurrentState.bIsActive)
	{
		Text->SetVisibility(true);

		if (CurrentState.FramePlayTime < 4)
		{
			if (CurrentState.bFacingRight)
				Transform->SetWorldLocation(FVector(CurrentState.Position.X - 20 * (10 - CurrentState.FramePlayTime), 0, CurrentState.Position.Y));
			else
				Transform->SetWorldLocation(FVector(CurrentState.Position.X + 20 * (10 - CurrentState.FramePlayTime), 0, CurrentState.Position.Y));
		}
		else 
		{
			Transform->SetWorldLocation(FVector(CurrentState.Position.X, 0, CurrentState.Position.Y));
		}


		FVector Scale = FVector(.15);

		Transform->SetRelativeScale3D(Scale);
	}
	else
		Text->SetVisibility(false);
}
