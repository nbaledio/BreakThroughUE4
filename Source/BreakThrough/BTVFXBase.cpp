// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTVFXBase.h"
#include "BTCharacterBase.h"

// Sets default values
ABTVFXBase::ABTVFXBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Billboard = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABTVFXBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABTVFXBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABTVFXBase::Activate(FVector2D Location, bool bFacingRight)
{
	if (Owner)
	{
		CurrentState.Position = Location;

		FVector Scale = FVector(1);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		BaseMesh->SetRelativeScale3D(Scale);
		Billboard->SetRelativeScale3D(Scale);
	}

	CurrentState.bIsActive = true;
	CurrentState.AnimFrameIndex = 0;
	CurrentState.FramePlayTime = 0;
}

void ABTVFXBase::Update()
{
	if (CurrentState.bIsActive)
	{
		if (CurrentState.HitStop == 0)
		{
			if (Owner)
			{
				if (Owner->CurrentState.SlowMoTime % 2 == 0)
					CurrentState.FramePlayTime++;
			}
			else
				CurrentState.FramePlayTime++;

			if (CurrentState.FramePlayTime == AnimFrames[CurrentState.AnimFrameIndex])
			{
				CurrentState.AnimFrameIndex++;

				if (CurrentState.AnimFrameIndex == AnimFrames.Num())
					CurrentState.bIsActive = false;
			}

		}
	}
}

void ABTVFXBase::DrawEffect()
{
	if (CurrentState.bIsActive)
	{
		BaseMesh->SetVisibility(true);
		Billboard->SetVisibility(true);

		Transform->SetWorldLocation(FVector(CurrentState.Position.X, 0, CurrentState.Position.Y));
	}
	else
	{
		BaseMesh->SetVisibility(false);
		Billboard->SetVisibility(false);
	}
}

void ABTVFXBase::CreateMaterials() { }
