// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTShatterVFX.h"
#include "RoundManager.h"

// Sets default values
ABTShatterVFX::ABTShatterVFX()
{
	Glass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	Glass->SetupAttachment(RootComponent);

	KOEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KOEffect"));
	KOEffect->SetupAttachment(RootComponent);
	
	KORing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KO Ring"));
	KORing->SetupAttachment(KOEffect);

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
	GlassParticlesKO->Deactivate();
}

void ABTShatterVFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);

	FVector Scale = FVector(.16);

	if (!CurrentState.bFacingRight)
		Scale.X *= -1;
	Transform->SetRelativeScale3D(Scale);

	Transform->SetWorldRotation(FRotator(0));

	if (InteractType == KO)
	{
		KOEffect->SetRelativeRotation(FRotator(0, FMath::RandRange(-180.f, 180.f), 0));
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

	if (EffectMaterial)
		DynamicEffectMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);

	if (RingMaterial)
		DynamicRingMaterial = UMaterialInstanceDynamic::Create(RingMaterial, this);

	if (DynamicGlassMaterial && Glass)
		Glass->SetMaterial(0, DynamicGlassMaterial); 

	if (DynamicEffectMaterial && KOEffect)
		KOEffect->SetMaterial(0, DynamicEffectMaterial);

	if (DynamicRingMaterial && KORing)
	{
		KORing->SetMaterial(0, DynamicRingMaterial);
		
		DynamicRingMaterial->SetVectorParameterValue(FName("Color"), FVector(.25, .7, 1));
	}
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

			if ((CurrentState.AnimFrameIndex == 0 && CurrentState.FramePlayTime > 20) || (CurrentState.AnimFrameIndex == 1 && CurrentState.FramePlayTime > 10) || (CurrentState.AnimFrameIndex == 2 && CurrentState.FramePlayTime > 20))
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
		else
		{
			/*if (CurrentState.FramePlayTime > 1)
				CurrentState.HitStop = Owner->CurrentState.HitStop;*/

			if (CurrentState.FramePlayTime == 160)
				CurrentState.bIsActive = false;

			if (Owner->CurrentState.HitStop > 80)
				GlassParticlesKO->CustomTimeDilation = .1;
			else if (Owner->CurrentState.HitStop > 0 && Owner->RoundManager->CurrentState.KOFramePlayTime > 0)
				GlassParticlesKO->CustomTimeDilation = .03;
			else
				GlassParticlesKO->CustomTimeDilation = 1;
		}
	}
}

void ABTShatterVFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();

	if (CurrentState.bIsActive && CurrentState.Interaction != KO)
	{
		Glass->SetVisibility(true);
		GlassParticlesLeft->SetVisibility(true);
		GlassParticlesRight->SetVisibility(true);

		if (CurrentState.AnimFrameIndex < 2)
		{
			DynamicGlassMaterial->SetScalarParameterValue(FName("FadeOut"), 0);
		}
		else
		{
			DynamicGlassMaterial->SetScalarParameterValue(FName("FadeOut"), 1);
		}

		if (CurrentState.AnimFrameIndex == 0)
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(0.f, 1.f, ((float)CurrentState.FramePlayTime) / 20));
		else if (CurrentState.AnimFrameIndex == 2)
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(0.f, 1.f, ((float)CurrentState.FramePlayTime) / 20));
		else
			DynamicGlassMaterial->SetScalarParameterValue(FName("Alpha"), 1);
	}
	else if (CurrentState.bIsActive && CurrentState.Interaction == KO)
	{
		GlassParticlesKO->SetVisibility(true);
		KOEffect->SetVisibility(true);
		KORing->SetVisibility(true);

		if (Owner)
		{
			if (Owner->RoundManager)
			{
				FRotator EffectRotation = Owner->RoundManager->MainCamera->GetComponentRotation();
				EffectRotation.Yaw += 90;
				EffectRotation.Roll += 90;
				KORing->SetWorldRotation(EffectRotation);
			}
		}

		if (CurrentState.FramePlayTime <= 85)
		{
			if (CurrentState.FramePlayTime < 6)
				KOEffect->SetRelativeScale3D(FVector(FMath::Lerp(.5f, 1.f, (float)(CurrentState.FramePlayTime) / 6.f)));
			else
				KOEffect->SetRelativeScale3D(FVector(FMath::Lerp(1.f, 1.5f, (float)(CurrentState.FramePlayTime - 6) / 80.f)));

			KORing->SetRelativeScale3D(FVector(FMath::Lerp(.0f, 2.5f, FMath::Min(1.f, (float)(CurrentState.FramePlayTime) / 15.f))));
			
			DynamicEffectMaterial->SetScalarParameterValue(FName("Alpha"), 1);
			DynamicRingMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Min(1.f, (float)(CurrentState.FramePlayTime) / 30.f));
		}
		else
		{
			KORing->SetVisibility(false);
			KOEffect->SetRelativeScale3D(FVector(FMath::Lerp(1.5f, 2.f, FMath::Min(1.f, (float)(CurrentState.FramePlayTime - 85) / 10.f))));

			DynamicEffectMaterial->SetScalarParameterValue(FName("Alpha"), FMath::Lerp(1.f, 0.f, FMath::Min(1.f, (float)(CurrentState.FramePlayTime - 85) / 15.f)));
		}

		if (CurrentState.AnimFrameIndex == 0 && CurrentState.FramePlayTime == 1)
			GlassParticlesKO->Activate(true);

		Glass->SetVisibility(false);
		GlassParticlesLeft->SetVisibility(false);
		GlassParticlesRight->SetVisibility(false);
	}
	else
	{
		Glass->SetVisibility(false);
		GlassParticlesLeft->SetVisibility(false);
		GlassParticlesRight->SetVisibility(false);
		KOEffect->SetVisibility(false);
		KORing->SetVisibility(false);
		GlassParticlesKO->SetVisibility(false);
	}
}
