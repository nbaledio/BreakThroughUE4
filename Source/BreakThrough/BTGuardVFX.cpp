// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTGuardVFX.h"
#include "BTCharacterBase.h"

// Sets default values
ABTGuardVFX::ABTGuardVFX()
{
	GuardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GuardMesh"));
	GuardMesh->SetupAttachment(RootComponent);
}

void ABTGuardVFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);

	if (!(HitInfo & IsSpecial || HitInfo & IsHeavy || InteractType == HeavyResolve))
		Transform->SetRelativeScale3D(Transform->GetRelativeScale3D() * .75);

	if (InteractType == Guard || InteractType == JustGuard)
	{
		DynamicWaveMaterial->SetVectorParameterValue(FName("Color"), FVector(0, .1, .65));
		DynamicFlashMaterial->SetVectorParameterValue(FName("Color"), FVector(0, .5, 1));
	}
	else
	{
		DynamicWaveMaterial->SetVectorParameterValue(FName("Color"), FVector(.85, 0, .05));
		DynamicFlashMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .15, .5));
	}
}

void ABTGuardVFX::CreateMaterials()
{
	ABTVFXBase::CreateMaterials();

	if (WaveMaterial)
	{
		DynamicWaveMaterial = UMaterialInstanceDynamic::Create(WaveMaterial, this);
	}

	if (FlashMaterial)
	{
		DynamicFlashMaterial = UMaterialInstanceDynamic::Create(FlashMaterial, this);
	}

	if (AirMaterial)
	{
		DynamicAirMaterial = UMaterialInstanceDynamic::Create(AirMaterial, this);
	}

	if (GuardMesh)
	{
		if (DynamicWaveMaterial)
		{
			GuardMesh->SetMaterial(0, DynamicWaveMaterial);
		}

		if (DynamicFlashMaterial)
		{
			GuardMesh->SetMaterial(1, DynamicFlashMaterial);
			DynamicFlashMaterial->SetTextureParameterValue(FName("SpriteSheet"), FlashTexture);
		}

		if (DynamicAirMaterial)
		{
			GuardMesh->SetMaterial(2, DynamicAirMaterial);
		}
	}
}

void ABTGuardVFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive && CurrentState.HitStop == 0)
	{
		if (!(CurrentState.HitProperties & IsSpecial || CurrentState.Interaction == HeavyResolve || CurrentState.HitProperties & IsHeavy || 
			CurrentState.Interaction == AirDash || CurrentState.Interaction == JustGuard))
		{
			if (CurrentState.FramePlayTime == 1)
			{
				CurrentState.AnimFrameIndex++;
				CurrentState.FramePlayTime = 0;
			}
		}
		else
		{
			if (CurrentState.FramePlayTime == 2)
			{
				CurrentState.AnimFrameIndex++;
				CurrentState.FramePlayTime = 0;
			}
		}

		if (CurrentState.AnimFrameIndex >= 4 && CurrentState.Interaction == AirDash)
			CurrentState.bIsActive = false;
		else if (CurrentState.AnimFrameIndex >= 8 && !(CurrentState.HitProperties & IsHeavy) && CurrentState.Interaction != HeavyResolve)
			CurrentState.bIsActive = false;
		else if (CurrentState.AnimFrameIndex >= 10)
			CurrentState.bIsActive = false;
	}
}

void ABTGuardVFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();

	if (CurrentState.bIsActive)
	{
		GuardMesh->SetVisibility(true);

		FVector Scale = FVector(.15);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		if (CurrentState.HitProperties & IsSpecial || CurrentState.Interaction == HeavyResolve)
			Scale *= 1.35;
		else if (CurrentState.Interaction == Resolve || CurrentState.Interaction == AirDash)
			Scale *= .85;
		else if (!(CurrentState.HitProperties & IsHeavy))
			Scale *= .5;

		Transform->SetRelativeScale3D(Scale);

		FVector AnimIndex = FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0);

		if (CurrentState.Interaction == Guard || CurrentState.Interaction == JustGuard)
			AnimIndex.Y += 2;

		if (CurrentState.AnimFrameIndex > 7 || CurrentState.Interaction == AirDash)
			AnimIndex.X = -1;

		DynamicFlashMaterial->SetVectorParameterValue(FName("AnimIndex"), AnimIndex);
		DynamicFlashMaterial->SetScalarParameterValue(FName("Emissivity"), FMath::Lerp(20.f, 1.f, (float)(CurrentState.AnimFrameIndex * 2 + CurrentState.FramePlayTime) * .0625));

		if (CurrentState.HitProperties & IsSpecial || CurrentState.Interaction == HeavyResolve)
		{
			float Alpha = (float)(CurrentState.AnimFrameIndex * 2 + CurrentState.FramePlayTime) / 20;

			DynamicWaveMaterial->SetScalarParameterValue(FName("Alpha"), Alpha);
			DynamicWaveMaterial->SetVectorParameterValue(FName("UVScale"), FVector(FMath::Lerp(.25, 1., 1.5 * Alpha)));
		}
		else
		{
			DynamicWaveMaterial->SetScalarParameterValue(FName("Alpha"), 1);
		}

		if ((CurrentState.Interaction == AirDash || CurrentState.Interaction == JustGuard) && CurrentState.AnimFrameIndex < 4)
		{
			DynamicAirMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 2, CurrentState.AnimFrameIndex / 2, 0));
			DynamicAirMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(1.f, 0.f, (float)((CurrentState.AnimFrameIndex * 2 + CurrentState.FramePlayTime) * .125)));
		}
		else
		{
			DynamicAirMaterial->SetScalarParameterValue(FName("Alpha"), 0);
		}
	}
	else
		GuardMesh->SetVisibility(false);
}
