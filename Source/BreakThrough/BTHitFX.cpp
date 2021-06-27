// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTHitFX.h"
#include "BTCharacterBase.h"
#include "RoundManager.h"

// Sets default values
ABTHitFX::ABTHitFX()
{
	Billboard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);

	Spark = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spark"));
	Spark->SetupAttachment(RootComponent);

	Cross = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cross"));
	Cross->SetupAttachment(RootComponent);

	Ring = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ring"));
	Ring->SetupAttachment(Cross);
}

void ABTHitFX::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	ABTVFXBase::Activate(Location, bFacingRight, HitInfo, InteractType);

	Billboard->SetRelativeRotation(FRotator(0, 0, 90));

	Spark->SetRelativeRotation(FRotator(FMath::RandRange(-60, 60), FMath::RandRange(-60, 60), 90));

	FRotator CrossRotation = FRotator(FMath::RandRange(-180, 180), FMath::RandRange(-60, 60), 90);
	Cross->SetRelativeRotation(CrossRotation);

	/*if (InteractType == Deflect)
		Ring->SetRelativeRotation(CrossRotation + FRotator(5));
	else
		Ring->SetRelativeRotation(CrossRotation);*/

	if (HitInfo & IsSlash)
	{
		if (HitInfo & IsVertical)
		{
			if ((HitInfo & IsUpwardSlash))
				Billboard->SetRelativeRotation(FRotator(FMath::RandRange(40, 70), 0, 90));
			else
				Billboard->SetRelativeRotation(FRotator(FMath::RandRange(-130, -150), 0, 90));
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

	/*if (CurrentState.Interaction == Deflect)
		DynamicSparkMaterial->SetScalarParameterValue(FName("Emissivity"), 0);
	else*/
		DynamicSparkMaterial->SetScalarParameterValue(FName("Emissivity"), 4);
}

void ABTHitFX::CreateMaterials()
{
	ABTVFXBase::CreateMaterials();

	if (EffectMaterial)
	{
		DynamicBillboardMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
		DynamicSparkMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
		DynamicCrossMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
		DynamicRingMaterial = UMaterialInstanceDynamic::Create(EffectMaterial, this);
	}

	if (DynamicBillboardMaterial && Billboard)
	{
		Billboard->SetMaterial(0, DynamicBillboardMaterial);
		if (BillboardTexture)
			DynamicBillboardMaterial->SetTextureParameterValue(FName("SpriteSheet"), BillboardTexture);
	}

	if (DynamicSparkMaterial && Spark)
	{
		Spark->SetMaterial(0, DynamicSparkMaterial);
		//DynamicSparkMaterial->SetScalarParameterValue(FName("Emissivity"), 4);
		DynamicSparkMaterial->SetVectorParameterValue(FName("Color"), FVector(1));
		if (SparkTexture)
			DynamicSparkMaterial->SetTextureParameterValue(FName("SpriteSheet"), SparkTexture);
	}

	if (DynamicCrossMaterial && Cross)
	{
		Cross->SetMaterial(0, DynamicCrossMaterial);
		DynamicCrossMaterial->SetScalarParameterValue(FName("Emissivity"), 0);
		DynamicCrossMaterial->SetVectorParameterValue(FName("Color"), FVector(.25, 1, .35));
		DynamicCrossMaterial->SetVectorParameterValue(FName("RowsAndColumns"), FVector(2));
	}

	if (DynamicRingMaterial && Ring)
	{
		Ring->SetMaterial(0, DynamicRingMaterial);
		DynamicRingMaterial->SetScalarParameterValue(FName("Emissivity"), 15);
		DynamicRingMaterial->SetVectorParameterValue(FName("RowsAndColumns"), FVector(2));
		DynamicRingMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .02, 0));
		//DynamicRingMaterial->SetVectorParameterValue(FName("Color"), FVector(.1, 1, .25));
	}
}

void ABTHitFX::Update()
{
	ABTVFXBase::Update();

	if (CurrentState.bIsActive && CurrentState.HitStop == 0)
	{
		FVector Scale = FVector(.15);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		if (CurrentState.HitProperties & IsSuper)
			Scale *= 2;
		else if (CurrentState.HitProperties & IsSpecial)
			Scale *= 1.75;
		else if (CurrentState.HitProperties & IsHeavy || CurrentState.Interaction == Deflect)
			Scale *= 1.35;

		Transform->SetRelativeScale3D(Scale);

		if ((CurrentState.HitProperties & IsSpecial && CurrentState.AnimFrameIndex % 2 == 0) || CurrentState.HitProperties & IsSuper)
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

		if (CurrentState.Interaction == Hit)
		{
			if ((CurrentState.HitProperties & IsSlash || CurrentState.Interaction == Clash || CurrentState.Interaction == Deflect) && CurrentState.AnimFrameIndex >= 8)
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
		else if (CurrentState.AnimFrameIndex >= 8)
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

		if (Owner)
		{
			if (Owner->RoundManager)
			{
				FRotator EffectRotation = Owner->RoundManager->MainCamera->GetComponentRotation();
				EffectRotation.Yaw += 90;
				SetActorRotation(EffectRotation);
			}
		}

		Billboard->SetMaterial(0, DynamicBillboardMaterial);

		if (CurrentState.HitProperties & IsSlash)
		{
			DynamicBillboardMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, 2 + CurrentState.AnimFrameIndex / 4, 0));
			DynamicBillboardMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .075, 0));
		}
		else
		{
			DynamicBillboardMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0));
			if (CurrentState.Interaction == Hit)
				DynamicBillboardMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .1, 0));
			else
				DynamicBillboardMaterial->SetVectorParameterValue(FName("Color"), FVector(.25, 1, .35));
		}

		if (!(CurrentState.HitProperties & IsSpecial) && !(CurrentState.HitProperties & IsHeavy) && !(CurrentState.HitProperties & IsSuper) && CurrentState.Interaction != Deflect)
			DynamicSparkMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0));
		else
		{
			DynamicSparkMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, 2 + CurrentState.AnimFrameIndex / 4, 0));
		}

		if (CurrentState.HitProperties & IsSpecial || CurrentState.HitProperties & IsSuper || CurrentState.Interaction == Clash || CurrentState.Interaction == Deflect)
		{
			Cross->SetVisibility(true);

			if (CurrentState.FramePlayTime == 0 && CurrentState.Interaction == Hit)
			{
				FRotator CrossRotation = FRotator(Cross->GetRelativeRotation());
				CrossRotation += FRotator(3, 0, 0);
				Cross->SetRelativeRotation(CrossRotation);
			}

			if (CurrentState.Interaction != Clash)
				Ring->SetVisibility(true);

			if (CurrentState.Interaction != Hit)
			{
				DynamicCrossMaterial->SetScalarParameterValue(FName("Emissivity"), 10);
				DynamicRingMaterial->SetScalarParameterValue(FName("Emissivity"), 0);
			}
			else
			{
				DynamicCrossMaterial->SetScalarParameterValue(FName("Emissivity"), 0);
				DynamicRingMaterial->SetScalarParameterValue(FName("Emissivity"), 10);
			}

			if (CrossTexture00 && CurrentState.AnimFrameIndex < 4)
			{
				DynamicCrossMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture00);
				DynamicCrossMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 2, CurrentState.AnimFrameIndex / 2, 0));
				DynamicRingMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture00);
				DynamicRingMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 2, CurrentState.AnimFrameIndex / 2, 0));
			}
			else if (CrossTexture01)
			{
				DynamicCrossMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture01);
				DynamicCrossMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector((CurrentState.AnimFrameIndex - 4) % 2, (CurrentState.AnimFrameIndex - 4) / 2, 0));
				DynamicRingMaterial->SetTextureParameterValue(FName("SpriteSheet"), CrossTexture01);
				DynamicRingMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 2, CurrentState.AnimFrameIndex / 2, 0));
			}
		}
		else
		{
			Cross->SetVisibility(false);
			Ring->SetVisibility(false);
		}

		/*if (CurrentState.Interaction == Deflect)
		{
			Ring->SetVisibility(true);
			DynamicRingMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.AnimFrameIndex % 4, CurrentState.AnimFrameIndex / 4, 0));
		}
		else
		{
			Ring->SetVisibility(false);
		}*/
	}
	else
	{
		Billboard->SetVisibility(false);
		Spark->SetVisibility(false);
		Cross->SetVisibility(false);
		Ring->SetVisibility(false);
	}
}