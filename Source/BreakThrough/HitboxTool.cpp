// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "HitboxTool.h"

// Sets default values
AHitboxTool::AHitboxTool()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	PersonalCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Personal Camera"));
	PersonalCamera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHitboxTool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHitboxTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawPushbox();

	if (Hitboxes.Num() > 0)
	{
		for (int32 i = 0; i < Hitboxes.Num(); i++)
		{
			DrawHitbox(Hitboxes[i]);
		}
	}

	if (Hurtboxes.Num() > 0)
	{
		for (int32 i = 0; i < Hurtboxes.Num(); i++)
		{
			DrawHurtbox(Hurtboxes[i]);
		}
	}
}

void AHitboxTool::DrawPushbox()
{
	float PushboxBottom = GetActorLocation().Z + PushboxVerticalOffset;
	//Left side
	DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, 41, PushboxBottom),
		FVector(GetActorLocation().X - .5f * PushboxWidth, 41, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
	//bottom side
	DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, 41, PushboxBottom),
		FVector(GetActorLocation().X + .5f * PushboxWidth, 41, PushboxBottom), FColor(255, 255, 0), false, 0, 0, .5f);
	//right side
	DrawDebugLine(GetWorld(), FVector(GetActorLocation().X + .5f * PushboxWidth, 41, PushboxBottom),
		FVector(GetActorLocation().X + .5f * PushboxWidth, 41, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
	//top side
	DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, 41, PushboxBottom + PushBoxHeight),
		FVector(GetActorLocation().X + .5f * PushboxWidth, 41, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
}

void AHitboxTool::DrawHitbox(FTestHurtbox Box)
{
	FVector BoxCenter = FVector(GetActorLocation().X + Box.Position.X, GetActorLocation().Y + 40, GetActorLocation().Z + Box.Position.Y);
	//Left side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Bottom side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Right side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Top side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
}

void AHitboxTool::DrawHurtbox(FTestHurtbox Box)
{
	FVector BoxCenter = FVector(GetActorLocation().X + Box.Position.X, GetActorLocation().Y + 150, GetActorLocation().Z + Box.Position.Y);
	//Left side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Bottom side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Right side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Top side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
}