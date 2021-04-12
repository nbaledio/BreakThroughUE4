// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "Components/MaterialBillboardComponent.h"
#include "BlitzImageBase.generated.h"

class ABTCharacterBase;

USTRUCT(BlueprintType)
struct FBlitzState
{
	GENERATED_BODY()

	bool bIsActive;
	bool bFacingRight;

	FVector2D Position;

	float Alpha;
	uint8 FlashEffectFrameIndex;
	uint8 FlashEffectFramePlayTime;
	uint8 WaveEffectFrameIndex;
	uint8 WaveEffectFramePlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		uint8 EffectColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimationAsset* Pose;
};

UCLASS()
class BREAKTHROUGH_API ABlitzImageBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlitzImageBase();

	virtual void AssignOwner(ABTCharacterBase* NewOwner) { Owner = NewOwner; }

	void Activate(FVector2D Location, UAnimationAsset* NewPose, bool FacingRight, uint8 EffectColor);

	void Update();

	virtual void DrawBlitz();

	virtual void CreateMaterials();

	FBlitzState CurrentState{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		FVector BlitzColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		FVector LineDissolveColor;

	UMaterialInstanceDynamic* DynamicFlashMaterial;
	UMaterialInstanceDynamic* DynamicLineMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UMaterialBillboardComponent* BlitzFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* InvisibleMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* BlitzMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* LineMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* FlashMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UAnimationAsset* DefaultAnimation;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
