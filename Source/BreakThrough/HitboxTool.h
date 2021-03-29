// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
//#include "Camera/CameraComponent.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "BTCharacterBase.h"
#include "HitboxTool.generated.h"

USTRUCT(BlueprintType)
struct FTestHurtbox
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions")
		FVector2D Position; //position of hurtbox = character position + hurtbox position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position
};

UCLASS()
class BREAKTHROUGH_API AHitboxTool : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHitboxTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCineCameraComponent* PersonalCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitboxes")
		TArray<FTestHurtbox> Hitboxes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurtboxes")
		TArray<FTestHurtbox> Hurtboxes;

	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushBoxHeight = 100;
	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushboxWidth = 50;
	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushboxVerticalOffset = 0;

	void DrawPushbox();

	void DrawHitbox(FTestHurtbox Box);

	void DrawHurtbox(FTestHurtbox Box);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
