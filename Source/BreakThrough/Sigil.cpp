// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "Sigil.h"
#include "BTCharacterBase.h"

// Sets default values
ASigil::ASigil()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	SigilBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sigil Back"));
	SigilBack->SetupAttachment(RootComponent);

	SigilMid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sigil Mid"));
	SigilMid ->SetupAttachment(SigilBack);

	SigilFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sigil Front"));
	SigilFront->SetupAttachment(SigilBack);
	
	Echo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Echo"));
	Echo->SetupAttachment(SigilBack);
}

// Called when the game starts or when spawned
void ASigil::BeginPlay()
{
	Super::BeginPlay();

	if (BaseSigilMaterial != nullptr)
	{
		DynamicSigilBackMaterial = UMaterialInstanceDynamic::Create(BaseSigilMaterial, this);
		DynamicSigilMidMaterial = UMaterialInstanceDynamic::Create(BaseSigilMaterial, this);
		DynamicSigilFrontMaterial = UMaterialInstanceDynamic::Create(BaseSigilMaterial, this);
	}
	if (EchoSigilMaterial != nullptr)
	{
		DynamicEchoMaterial = UMaterialInstanceDynamic::Create(EchoSigilMaterial, this);
	}

	if (DynamicSigilBackMaterial != nullptr)
	{
		if (BackSigilImage)
			DynamicSigilBackMaterial->SetTextureParameterValue(FName("SigilTexture"), BackSigilImage);
		SigilBack->SetMaterial(0, DynamicSigilBackMaterial);
	}

	if (DynamicSigilMidMaterial != nullptr)
	{
		if (MidSigilImage)
			DynamicSigilMidMaterial->SetTextureParameterValue(FName("SigilTexture"), MidSigilImage);
		SigilMid->SetMaterial(0, DynamicSigilMidMaterial);
	}

	if (DynamicSigilFrontMaterial != nullptr)
	{
		if (FrontSigilImage)
			DynamicSigilFrontMaterial->SetTextureParameterValue(FName("SigilTexture"), FrontSigilImage);
		SigilFront->SetMaterial(0, DynamicSigilFrontMaterial);
	}

	if (DynamicEchoMaterial != nullptr)
	{
		if (FullSigilImage != nullptr)
			DynamicEchoMaterial->SetTextureParameterValue(FName("SigilTexture"), FullSigilImage);
		Echo->SetMaterial(0, DynamicEchoMaterial);
	}

	CurrentState.bIsActive = false;
	SigilBack->SetVisibility(false);
	SigilMid->SetVisibility(false);
	SigilFront->SetVisibility(false);
	Echo->SetVisibility(false);
}

void ASigil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update();
	DrawSigil();
}

void ASigil::Activate(FVector2D Location, FRotator Orientation)
{
	CurrentState.Position = Location;
	CurrentState.Rotation = Orientation;

	SigilBack->SetRelativeRotation(FRotator(0, 0, 0));

	CurrentState.bIsActive = true;
	CurrentState.MainEmissiveAlpha = 0;
	CurrentState.MainScaleAlpha = 0;
	CurrentState.EchoScaleAlpha = 0;
}

void ASigil::Update()
{
	if (CurrentState.bIsActive)
	{
		//if (CurrentState.MainScaleAlpha < 3)
			CurrentState.MainScaleAlpha += .1f;
		if (CurrentState.MainScaleAlpha >= 3 && CurrentState.MainEmissiveAlpha < 1)
		{
			//CurrentState.MainScaleAlpha = 1;
			CurrentState.MainEmissiveAlpha += .1f;
		}

		if (CurrentState.MainScaleAlpha < 1)
			CurrentState.EchoScaleAlpha = .5f * CurrentState.MainScaleAlpha;
		else
			CurrentState.EchoScaleAlpha = FMath::Max(.5f, CurrentState.EchoScaleAlpha + .035f);

		if (CurrentState.MainEmissiveAlpha >= 1)
			CurrentState.bIsActive = false;
	}
}

void ASigil::DrawSigil()
{
	if (CurrentState.bIsActive)
	{
		SigilBack->SetVisibility(true);
		SigilMid->SetVisibility(true);
		SigilFront->SetVisibility(true);
		Echo->SetVisibility(true);

		Transform->SetWorldLocation(FVector(CurrentState.Position.X, -25, CurrentState.Position.Y));
		Transform->SetWorldRotation(CurrentState.Rotation);

		SigilBack->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 1.f, FMath::Min(1.f, CurrentState.MainScaleAlpha))));
		
		Echo->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 1.5f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)), FMath::Lerp(0.f, 1.75f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)), 1));

		FVector EchoLocation = FVector(0, 0, FMath::Lerp(0.f, -50.f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)));
		FVector MidLocation = FVector(0, 0, FMath::Lerp(0.f, 20.f, FMath::Min(1.f, CurrentState.MainScaleAlpha - .5f)));
		FVector FrontLocation = FVector(0, 0, FMath::Lerp(MidLocation.Z, 40.f, FMath::Min(1.f, CurrentState.MainScaleAlpha - .5f)));
		Echo->SetRelativeLocation(EchoLocation);
		SigilMid->SetRelativeLocation(MidLocation);
		SigilFront->SetRelativeLocation(FrontLocation);

		SigilBack->SetRelativeRotation(FRotator(SigilBack->GetRelativeRotation().Pitch, SigilBack->GetRelativeRotation().Yaw + .15f, SigilBack->GetRelativeRotation().Roll));
		//Echo->SetRelativeRotation(FRotator(SigilBack->GetRelativeRotation().Pitch, SigilBack->GetRelativeRotation().Yaw - 2, SigilBack->GetRelativeRotation().Roll));

		//set material parameters
		if (DynamicSigilBackMaterial != nullptr)
		{
			DynamicSigilBackMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.MainEmissiveAlpha);
			DynamicSigilBackMaterial->SetVectorParameterValue(FName("SigilColor"), SigilColor);
		}
		if (DynamicSigilMidMaterial != nullptr)
		{
			DynamicSigilMidMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.MainEmissiveAlpha);
			DynamicSigilMidMaterial->SetVectorParameterValue(FName("SigilColor"), SigilColor);
		}
		if (DynamicSigilFrontMaterial != nullptr)
		{
			DynamicSigilFrontMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.MainEmissiveAlpha);
			DynamicSigilFrontMaterial->SetVectorParameterValue(FName("SigilColor"), SigilColor);
		}
		if (DynamicEchoMaterial != nullptr)
		{
			DynamicEchoMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.EchoScaleAlpha);
			DynamicEchoMaterial->SetVectorParameterValue(FName("EchoColor"), EchoColor);
		}
	}
	else
	{
		SigilBack->SetVisibility(false);
		SigilMid->SetVisibility(false);
		SigilFront->SetVisibility(false);
		Echo->SetVisibility(false);
	}
}

