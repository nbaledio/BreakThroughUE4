// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "BlitzImageBase.h"
#include "BTCharacterBase.h"

// Sets default values
ABlitzImageBase::ABlitzImageBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	BlitzFlash = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Blitz Flash"));
	BlitzFlash->SetupAttachment(RootComponent);

	BlitzWave = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blitz Wave"));
	BlitzWave->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlitzImageBase::BeginPlay()
{
	Super::BeginPlay();
	CreateMaterials();
	//Activate(FVector2D(0), DefaultAnimation, true, 0);
}

// Called every frame
void ABlitzImageBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update();
	DrawBlitz();
}

void ABlitzImageBase::Activate(FVector2D Location, UAnimationAsset* NewPose, bool FacingRight, uint8 EffectColor)
{
	CurrentState.bIsActive = true;
	CurrentState.bFacingRight = FacingRight;
	CurrentState.Position = Location;
	CurrentState.Pose = NewPose;
	CurrentState.Alpha = 0;
	CurrentState.FlashEffectFrameIndex = 0;
	CurrentState.FlashEffectFramePlayTime = 0;
	CurrentState.EffectColor = EffectColor;

	BaseMesh->SetRelativeLocation(FVector(0));
}

void ABlitzImageBase::ActivateWave()
{
	CurrentState.bWaveIsActive = true;
	CurrentState.WaveScaleAlpha= 0;
	CurrentState.WaveAlpha = 0;
}

void ABlitzImageBase::Update()
{
	if (CurrentState.bIsActive)
	{
		CurrentState.Alpha = FMath::Min(1.f, CurrentState.Alpha + .035f);

		if (Owner != nullptr)
		{
			if (Owner->Opponent != nullptr)
			{
				if (Owner->Opponent->CurrentState.SlowMoTime > 15 || Owner->IsCurrentAnimation(Owner->ExtendBlitz))
				{
					CurrentState.Alpha = 0;
				}
			}

			if (Owner->IsCurrentAnimation(Owner->FocusBlitz) || Owner->IsCurrentAnimation(Owner->BreakerBlitz))
			{
				CurrentState.Alpha = 0;
				if (Owner->CurrentState.AnimFrameIndex > 0 && Owner->CurrentState.CurrentAnimation[Owner->CurrentState.AnimFrameIndex - 1].Pose != nullptr)
					CurrentState.Pose = Owner->CurrentState.CurrentAnimation[Owner->CurrentState.AnimFrameIndex - 1].Pose;
				else if (Owner->CurrentState.CurrentAnimation[Owner->CurrentState.AnimFrameIndex].Pose != nullptr)
					CurrentState.Pose = Owner->CurrentState.CurrentAnimation[Owner->CurrentState.AnimFrameIndex].Pose;
			}
		}

		if (CurrentState.FlashEffectFrameIndex < 7)
		{
			if (CurrentState.FlashEffectFramePlayTime >= 2)
			{
				CurrentState.FlashEffectFrameIndex++;
				CurrentState.FlashEffectFramePlayTime = 0;
			}
			CurrentState.FlashEffectFramePlayTime++;
		}
		else if (CurrentState.Alpha >= 1)
			CurrentState.bIsActive = false;

		if (CurrentState.bWaveIsActive)
		{
			if (Owner->IsCurrentAnimation(Owner->ExtendBlitz))
			{
				if (CurrentState.WaveScaleAlpha < 2)
					CurrentState.WaveScaleAlpha += .125f;

				if (CurrentState.WaveScaleAlpha > .5f && CurrentState.WaveAlpha < 1)
					CurrentState.WaveAlpha += .1f;
				else if (CurrentState.WaveAlpha >= 1)
					CurrentState.bWaveIsActive = false;
			}
			else
			{
				if (CurrentState.WaveScaleAlpha < 2)
					CurrentState.WaveScaleAlpha += .1f;

				if (CurrentState.WaveScaleAlpha > .5f && CurrentState.WaveAlpha < 1)
					CurrentState.WaveAlpha += .07f;
				else if (CurrentState.WaveAlpha >= 1)
					CurrentState.bWaveIsActive = false;
			}
		}

		if (BaseMesh != nullptr)
			if (CurrentState.Pose != nullptr)
				BaseMesh->PlayAnimation(CurrentState.Pose, false);
	}
}

void ABlitzImageBase::DrawBlitz()
{
	if (CurrentState.bIsActive)
	{
		BaseMesh->SetVisibility(true);

		FVector Scale = FVector(1);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		Transform->SetWorldLocation(FVector(CurrentState.Position.X, 0, CurrentState.Position.Y));
		BaseMesh->SetRelativeScale3D(Scale);

		if (Owner->IsCurrentAnimation(Owner->FocusBlitz) || Owner->IsCurrentAnimation(Owner->BreakerBlitz))
		{
			float BaseMeshOffset;
			if (CurrentState.bFacingRight)
				BaseMeshOffset = FMath::Lerp(0, -500, FMath::Min(1.f, (float)((CurrentState.FlashEffectFrameIndex * 2) + CurrentState.FlashEffectFramePlayTime) / 8));
			else
				BaseMeshOffset = FMath::Lerp(0, 500, FMath::Min(1.f, (float)((CurrentState.FlashEffectFrameIndex * 2) + CurrentState.FlashEffectFramePlayTime) / 8));

			BaseMesh->SetRelativeLocation(FVector(BaseMeshOffset, 0, 0));
		}

		DynamicLineMaterial->SetVectorParameterValue(FName("DissolveColor"), LineDissolveColor);
		DynamicLineMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.Alpha);
	}
	else
		BaseMesh->SetVisibility(false);

	if (CurrentState.FlashEffectFrameIndex < 7 && CurrentState.bIsActive && DynamicFlashMaterial)
	{
		BlitzFlash->SetVisibility(true);

		DynamicFlashMaterial->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.FlashEffectFrameIndex % 4, FMath::FloorToInt(CurrentState.FlashEffectFrameIndex / 4), 0));

		if (CurrentState.EffectColor == 1) //Focus BCs are blue
			DynamicFlashMaterial->SetVectorParameterValue(FName("Color"), FVector(0, .35f, 1));
		else if (CurrentState.EffectColor == 2) // Breaker BCs are Pink
			DynamicFlashMaterial->SetVectorParameterValue(FName("Color"), FVector(1, .3f, 1));
		else //Normal BCs are Yellow
			DynamicFlashMaterial->SetVectorParameterValue(FName("Color"), FVector(1, 1, 0));
	}
	else
	{
		BlitzFlash->SetVisibility(false);
	}

	if (CurrentState.bWaveIsActive && DynamicWaveMaterial)
	{
		BlitzWave->SetVisibility(true);

		//FVector Scale = FMath::Lerp(FVector(0), FVector(1), CurrentState.WaveScaleAlpha);
		if (CurrentState.WaveScaleAlpha < 1)
			BlitzWave->SetRelativeScale3D(FMath::Lerp(FVector(.35f), FVector(1.75f), CurrentState.WaveScaleAlpha));
		else
			BlitzWave->SetRelativeScale3D(FMath::Lerp(FVector(1.75f), FVector(2), CurrentState.WaveScaleAlpha - 1));

		DynamicWaveMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.WaveAlpha);

		if (CurrentState.EffectColor == 1) //Focus BCs are blue
			DynamicWaveMaterial->SetVectorParameterValue(FName("Color"), FVector(0, .1f, .65f));
		else if (CurrentState.EffectColor == 2) // Breaker BCs are Pink
			DynamicWaveMaterial->SetVectorParameterValue(FName("Color"), FVector(.65f, 0, .65f));
		else //Normal BCs are Yellow
			DynamicWaveMaterial->SetVectorParameterValue(FName("Color"), FVector(.7, .7, 0));
	}
	else
	{
		BlitzWave->SetVisibility(false);
	}
}

void ABlitzImageBase::CreateMaterials()
{
	if (FlashMaterial != nullptr)
	{
		DynamicFlashMaterial = UMaterialInstanceDynamic::Create(FlashMaterial, this);
	}

	if (LineMaterial != nullptr)
	{
		DynamicLineMaterial = UMaterialInstanceDynamic::Create(LineMaterial, this);
	}

	if (WaveMaterial != nullptr)
		DynamicWaveMaterial = UMaterialInstanceDynamic::Create(WaveMaterial, this);

	if (DynamicFlashMaterial != nullptr)
		BlitzFlash->SetMaterial(0, DynamicFlashMaterial);

	if (DynamicWaveMaterial != nullptr)
		BlitzWave->SetMaterial(0, DynamicWaveMaterial);
}
