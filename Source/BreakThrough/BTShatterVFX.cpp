// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTShatterVFX.h"
#include "RoundManager.h"

// Sets default values
ABTShatterVFX::ABTShatterVFX()
{
	Glass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	Glass->SetupAttachment(RootComponent);

	GlassParticlesLeft = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlassParticlesLeft"));
	GlassParticlesLeft->SetupAttachment(RootComponent);

	GlassParticlesRight = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlassParticlesRight"));
	GlassParticlesRight->SetupAttachment(RootComponent);

	GlassParticlesKO = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlassParticlesKO"));
	GlassParticlesKO->SetupAttachment(RootComponent);
}

void ABTShatterVFX::BeginPlay()
{
	ABTVFXBase::BeginPlay();

	GlassParticlesLeft->Deactivate();
	GlassParticlesRight->Deactivate();
}

void ABTShatterVFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);

	FVector Scale = FVector(.16);

	if (!CurrentState.bFacingRight)
		Scale.X *= -1;
	Transform->SetRelativeScale3D(Scale);

	if (InteractType == KO)
	{

	}
	else
	{
		FRotator GlassRotation = FRotator(FMath::RandRange(-180, 180), 45, 90);

		Glass->SetRelativeRotation(GlassRotation);

		if (ShatterWholeOffsets)
			DynamicGlassMaterial->SetTextureParameterValue(FName("ReflectionOffset"), ShatterWholeOffsets);
	}
}

void ABTShatterVFX::CreateMaterials()
{
	ABTVFXBase::CreateMaterials();

	if (GlassMaterial)
		DynamicGlassMaterial = UMaterialInstanceDynamic::Create(GlassMaterial, this);

	if (DynamicGlassMaterial && Glass)
		Glass->SetMaterial(0, DynamicGlassMaterial); 
}

void ABTShatterVFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive)
	{
		if (CurrentState.Interaction != KO)
		{
			if (CurrentState.AnimFrameIndex > 0)
				CurrentState.HitStop = Owner->CurrentState.HitStop;

			if ((CurrentState.AnimFrameIndex == 0 && CurrentState.FramePlayTime > 15) || (CurrentState.AnimFrameIndex == 1 && CurrentState.FramePlayTime > 10) || (CurrentState.AnimFrameIndex == 2 && CurrentState.FramePlayTime > 20))
			{
				CurrentState.AnimFrameIndex++;
				CurrentState.FramePlayTime = 0;
			}
			else if (CurrentState.AnimFrameIndex == 3 && CurrentState.FramePlayTime > 20)
				CurrentState.bIsActive = false;

			if (CurrentState.AnimFrameIndex == 1 && CurrentState.FramePlayTime == 1 && CurrentState.HitStop == 0)
			{
				if (ShatterBrokenOffsets)
					DynamicGlassMaterial->SetTextureParameterValue(FName("ReflectionOffset"), ShatterBrokenOffsets);
				if (CurrentState.bFacingRight)
				{
					if (Owner->Opponent->CurrentState.bTouchingWall)
						GlassParticlesLeft->Activate(true);
					else
						GlassParticlesRight->Activate(true);
				}
				else
				{
					if (Owner->Opponent->CurrentState.bTouchingWall)
						GlassParticlesRight->Activate(true);
					else
						GlassParticlesLeft->Activate(true);
				}
			}
		}
	}
}

void ABTShatterVFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();

	/*FRotator EffectRotation = Owner->RoundManager->MainCamera->GetComponentRotation();
	EffectRotation.Yaw += 90;
	SetActorRotation(EffectRotation);*/

	if (CurrentState.bIsActive)
	{
		if (CurrentState.Interaction != KO)
		{
			Glass->SetVisibility(true);
			GlassParticlesLeft->SetVisibility(true);
			GlassParticlesRight->SetVisibility(true);
		}

		if (CurrentState.AnimFrameIndex < 2)
		{
			DynamicGlassMaterial->SetScalarParameterValue(FName("FadeOut"), 0);
		}
		else
		{
			DynamicGlassMaterial->SetScalarParameterValue(FName("FadeOut"), 1);
		}

		if (CurrentState.AnimFrameIndex == 0)
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(0.f, 1.f, ((float)CurrentState.FramePlayTime)/15));
		else if (CurrentState.AnimFrameIndex == 2)
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(0.f, 1.f, ((float)CurrentState.FramePlayTime) / 20));
		else
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), 1);
	}
	else
	{
		Glass->SetVisibility(false);
		GlassParticlesLeft->SetVisibility(false);
		GlassParticlesRight->SetVisibility(false);
	}
}
