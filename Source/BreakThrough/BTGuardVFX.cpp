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

	if (InteractType == Guard)
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
	}
}

void ABTGuardVFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive)
	{
		if (!(CurrentState.HitProperties & IsSpecial || CurrentState.Interaction == HeavyResolve || CurrentState.HitProperties & IsHeavy))
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

		if (CurrentState.AnimFrameIndex >= 8 && !(CurrentState.HitProperties & IsHeavy) && CurrentState.Interaction != HeavyResolve)
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
		else if (CurrentState.Interaction == Resolve)
			Scale *= .85;
		else if (!(CurrentState.HitProperties & IsHeavy))
			Scale *= .5;

		Transform->SetRelativeScale3D(Scale);

		FVector AnimIndex = FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0);

		if (CurrentState.Interaction == Guard)
			AnimIndex.Y += 2;

		if (CurrentState.AnimFrameIndex > 7)
			AnimIndex.X = -1;

		DynamicFlashMaterial->SetVectorParameterValue(FName("AnimIndex"), AnimIndex);
		DynamicFlashMaterial->SetScalarParameterValue(FName("Emissivity"), FMath::Lerp(20, 1, (float)(CurrentState.AnimFrameIndex * 2 + CurrentState.FramePlayTime) / 16));

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
	}
	else
		GuardMesh->SetVisibility(false);
}
