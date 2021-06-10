// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTHitFX.h"
#include "BTCharacterBase.h"

// Sets default values
ABTHitFX::ABTHitFX()
{
	Billboard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	Spark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spark"));
	Spark->SetupAttachment(RootComponent);

	Cross = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cross"));
	Cross->SetupAttachment(RootComponent);
}

void ABTHitFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);

	Billboard->SetRelativeRotation(FRotator(0, 0, 90));

	Spark->SetRelativeRotation(FRotator(FMath::RandRange(-60, 60), FMath::RandRange(-60, 60), 90));

	Cross->SetRelativeRotation(FRotator(FMath::RandRange(-180, 180), FMath::RandRange(-60, 60), 90));

	if (HitInfo & IsSlash)
	{
		if (HitInfo & IsVertical)
		{
			if ((HitInfo & IsUpwardSlash))
				Billboard->SetRelativeRotation(FRotator(FMath::RandRange(10, 55), 0, 90));
			else
				Billboard->SetRelativeRotation(FRotator(FMath::RandRange(-110, -155), 0, 90));
		}
		else if (HitInfo & IsHorizontal)
		{
			Billboard->SetRelativeRotation(FRotator(FMath::RandRange(-55, -100), 0, 90));
		}
		else
			Billboard->SetRelativeRotation(FRotator(FMath::RandRange(-90, 90), 0, 90));
	}
	else
	{
		Billboard->SetRelativeRotation(FRotator(FMath::RandRange(-180, 180), 0, 90));
	}
}

void ABTHitFX::CreateMaterials()
{
	ABTVFXBase::CreateMaterials();

	if (EffectMaterial)
	{
		DynamicBillboardMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
		DynamicSparkMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
		DynamicCrossMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);

		if (BillboardTexture)
			DynamicBillboardMaterial->SetTextureParameterValue(FName("SpriteSheet"), BillboardTexture);

		if (SparkTexture)
			DynamicSparkMaterial->SetTextureParameterValue(FName("SpriteSheet"), SparkTexture);

		if (CrossTexture00)
		{
			DynamicCrossMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture00);
		}
	}

	if (DynamicBillboardMaterial && Billboard)
	{
		Billboard->SetMaterial(0, DynamicBillboardMaterial);
	}

	if (DynamicSparkMaterial && Spark)
	{
		Spark->SetMaterial(0, DynamicSparkMaterial);
		DynamicSparkMaterial->SetScalarParameterValue(FName("Emissivity"), 4);
		DynamicSparkMaterial->SetVectorParameterValue(FName("Color"), FVector(1));
	}

	if (DynamicCrossMaterial && Cross)
	{
		Cross->SetMaterial(0, DynamicCrossMaterial);
		DynamicCrossMaterial->SetScalarParameterValue(FName("Emissivity"), 0);
		DynamicCrossMaterial->SetVectorParameterValue(FName("RowsAndColumns"), FVector(2));
	}
}

void ABTHitFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive)
	{
		FVector Scale = FVector(.15);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		if (CurrentState.HitProperties & IsSpecial)
			Scale *= 1.75;
		else if (CurrentState.HitProperties & IsHeavy)
			Scale *= 1.35;

		Transform->SetRelativeScale3D(Scale);

		if (CurrentState.HitProperties & IsSpecial && CurrentState.AnimFrameIndex % 2 == 0)
		{
			if (CurrentState.FramePlayTime == 3)
			{
				CurrentState.AnimFrameIndex++;
				CurrentState.FramePlayTime = 0;
			}
		}
		else if (CurrentState.FramePlayTime == 2)
			{
				CurrentState.AnimFrameIndex++;
				CurrentState.FramePlayTime = 0;
			}

		if ((CurrentState.HitProperties & IsSlash) && CurrentState.AnimFrameIndex >= 8)
		{
			CurrentState.bIsActive = false;
		}
		else if ((CurrentState.HitProperties & IsHeavy || CurrentState.HitProperties & IsSpecial || CurrentState.HitProperties & IsSuper) && CurrentState.AnimFrameIndex >= 7)
		{
			CurrentState.bIsActive = false;
		}
		else if (!(CurrentState.HitProperties & IsSlash) && !(CurrentState.HitProperties & IsSpecial) && !(CurrentState.HitProperties & IsHeavy) && !(CurrentState.HitProperties & IsSuper) && CurrentState.AnimFrameIndex >= 5)
		{
			CurrentState.bIsActive = false;
		}
	}
}

void ABTHitFX::DrawEffect()
{
	ABTVFXBase::DrawEffect();

	if (CurrentState.bIsActive)
	{
		Billboard->SetVisibility(true);
		Spark->SetVisibility(true);

		if (CurrentState.HitProperties & IsSlash)
		{
			DynamicBillboardMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, 2 + CurrentState.AnimFrameIndex / 4, 0));
			DynamicBillboardMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .1, 0));
		}
		else
		{
			DynamicBillboardMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0));
			DynamicBillboardMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .25, 0));
		}

		if (!(CurrentState.HitProperties & IsSpecial) && !(CurrentState.HitProperties & IsHeavy) && !(CurrentState.HitProperties & IsSuper))
			DynamicSparkMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0));
		else
		{
			DynamicSparkMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, 2 + CurrentState.AnimFrameIndex / 4, 0));
		}

		if (CurrentState.HitProperties & IsSpecial || CurrentState.HitProperties & IsSuper)
		{
			Cross->SetVisibility(true);

			if (CrossTexture00 && CurrentState.AnimFrameIndex < 4)
			{
				DynamicCrossMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture00);
				DynamicCrossMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 2, CurrentState.AnimFrameIndex / 2, 0));
			}
			else if (CrossTexture01)
			{
				DynamicCrossMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture01);
				DynamicCrossMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector((CurrentState.AnimFrameIndex - 4) % 2, (CurrentState.AnimFrameIndex - 4) / 2, 0));
			}
		}
		else
		{
			Cross->SetVisibility(false);
		}
	}
	else
	{
		Billboard->SetVisibility(false);
		Spark->SetVisibility(false);
		Cross->SetVisibility(false);
	}
}