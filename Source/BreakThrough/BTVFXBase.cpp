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
}

// Called when the game starts or when spawned
void ABTVFXBase::BeginPlay()
{
	Super::BeginPlay();
	CreateMaterials();
}

// Called every frame
void ABTVFXBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABTVFXBase::Activate(FVector2D Location, bool bFacingRight, int32 HitInfo, uint8 InteractType)
{
	CurrentState.Position = Location;
	CurrentState.bFacingRight = bFacingRight;
	CurrentState.bIsActive = true;
	CurrentState.AnimFrameIndex = 0;
	CurrentState.FramePlayTime = 0;
	CurrentState.HitProperties = HitInfo;
	CurrentState.Interaction = InteractType;
}

void ABTVFXBase::Update()
{
	if (CurrentState.bIsActive)
	{
		if (Owner)
		{
			if (Owner->CurrentState.CurrentAnimFrame.bSuperFlash || Owner->Opponent->CurrentState.CurrentAnimFrame.bSuperFlash) //|| RoundManager.UniversalHitStop
				CurrentState.HitStop = 1;

			if (CurrentState.HitStop == 0)
			{
				if (bAffectedBySlowMo)
				{
					if (Owner->CurrentState.SlowMoTime % 2 == 0 && Owner->Opponent->CurrentState.SlowMoTime % 2 == 0)
						CurrentState.FramePlayTime++;
				}
				else
					CurrentState.FramePlayTime++;

				if (AnimFrames.Num() > 0)
				{
					if (CurrentState.FramePlayTime == AnimFrames[CurrentState.AnimFrameIndex])
					{
						CurrentState.AnimFrameIndex++;

						if (CurrentState.AnimFrameIndex == AnimFrames.Num())
							CurrentState.bIsActive = false;
					}
				}
			}
		}
	}
}

void ABTVFXBase::DrawEffect()
{
	if (CurrentState.bIsActive)
	{
		Transform->SetWorldLocation(FVector(CurrentState.Position.X, 0, CurrentState.Position.Y));

		FVector Scale = FVector(.15);
		if (!CurrentState.bFacingRight)
			Scale.X *= -1;

		Transform->SetRelativeScale3D(Scale);
	}
}

void ABTVFXBase::CreateMaterials() { }
