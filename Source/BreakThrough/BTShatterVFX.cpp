// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTShatterVFX.h"

// Sets default values
ABTShatterVFX::ABTShatterVFX()
{
	Glass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	Glass->SetupAttachment(RootComponent);

	GlassParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlassParticles"));
	GlassParticles->SetupAttachment(RootComponent);
}

void ABTShatterVFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);
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
}

void ABTShatterVFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();
}
