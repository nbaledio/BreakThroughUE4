// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "Sigil.h"

// Sets default values
ASigil::ASigil()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	MainMesh->SetupAttachment(RootComponent);

	Echo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Echo"));
	Echo->SetupAttachment(MainMesh);
}

// Called when the game starts or when spawned
void ASigil::BeginPlay()
{
	Super::BeginPlay();

	if (BaseSigilMaterial != NULL)
	{
		DynamicSigilMaterial = UMaterialInstanceDynamic::Create(BaseSigilMaterial, this);
	}
	if (EchoSigilMaterial != NULL)
	{
		DynamicEchoMaterial = UMaterialInstanceDynamic::Create(EchoSigilMaterial, this);
	}

	if (DynamicSigilMaterial != NULL)
	{
		if (DynamicEchoMaterial != NULL)
		{
			if (SigilImage != NULL)
			{
				DynamicSigilMaterial->SetTextureParameterValue(FName("SigilTexture"), SigilImage);
				DynamicEchoMaterial->SetTextureParameterValue(FName("SigilTexture"), SigilImage);
			}
		}
	}
	if (DynamicSigilMaterial != NULL)
		MainMesh->SetMaterial(0, DynamicSigilMaterial);
	if (DynamicEchoMaterial != NULL)
		Echo->SetMaterial(0, DynamicEchoMaterial);

	Activate(FVector2D(0), FRotator(-15, 0, 20));
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

	MainMesh->SetRelativeRotation(FRotator(0, 0, 0));

	CurrentState.bIsActive = true;
	CurrentState.MainEmissiveAlpha = 0;
	CurrentState.MainScaleAlpha = 0;
	CurrentState.EchoScaleAlpha = 0;
}

void ASigil::Update()
{
	if (CurrentState.bIsActive)
	{
		if (CurrentState.MainScaleAlpha < 1)
			CurrentState.MainScaleAlpha += .1f;
		else if (CurrentState.MainScaleAlpha >= 1 && CurrentState.MainEmissiveAlpha < 1)
		{
			CurrentState.MainScaleAlpha = 1;
			CurrentState.MainEmissiveAlpha += .04f;
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
		MainMesh->SetVisibility(true);
		Echo->SetVisibility(true);

		Transform->SetWorldLocation(FVector(CurrentState.Position.X, -25, CurrentState.Position.Y));
		Transform->SetWorldRotation(CurrentState.Rotation);

		//if (CurrentState.MainScaleAlpha < 1)
			MainMesh->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 1.f, FMath::Min(1.f, CurrentState.MainScaleAlpha))));
		/*else if (CurrentState.MainEmissiveAlpha >= .5)
			MainMesh->SetRelativeScale3D(FVector(FMath::Lerp(1.f, 1.3f, FMath::Min(1.f, 2 *(CurrentState.MainEmissiveAlpha - .5f)))));*/
		
		Echo->SetRelativeScale3D(FVector(FMath::Lerp(0.f, 2.f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)), FMath::Lerp(0.f, 1.75f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)), 1));

		FVector EchoLocation = FVector(0, 0, FMath::Lerp(0.f, -150.f, FMath::Min(1.f, CurrentState.EchoScaleAlpha)));
		Echo->SetRelativeLocation(EchoLocation);

		MainMesh->SetRelativeRotation(FRotator(MainMesh->GetRelativeRotation().Pitch, MainMesh->GetRelativeRotation().Yaw + 1, MainMesh->GetRelativeRotation().Roll));
		Echo->SetRelativeRotation(FRotator(MainMesh->GetRelativeRotation().Pitch, MainMesh->GetRelativeRotation().Yaw - 2, MainMesh->GetRelativeRotation().Roll));

		//set material parameters
		if (DynamicSigilMaterial != NULL)
		{
			DynamicSigilMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.MainEmissiveAlpha);
			DynamicSigilMaterial->SetVectorParameterValue(FName("SigilColor"), SigilColor);
		}
		if (DynamicEchoMaterial != NULL)
		{
			DynamicEchoMaterial->SetScalarParameterValue(FName("Alpha"), CurrentState.EchoScaleAlpha);
			DynamicEchoMaterial->SetVectorParameterValue(FName("EchoColor"), EchoColor);
		}
	}
	else
	{
		MainMesh->SetVisibility(false);
		Echo->SetVisibility(false);
	}
}

