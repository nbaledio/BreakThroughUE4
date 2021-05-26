// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/MaterialBillboardComponent.h"
#include "BTVFXBase.generated.h"

class ABTCharacterBase;

USTRUCT(BlueprintType)
struct FEffectState
{
	GENERATED_BODY()

	bool bIsActive;
	bool bFacingRight;

	FVector2D Position;

	uint8 AnimFrameIndex; //can be used to denote where in the projectile's animation sequence we are (jump to specific numbers to transition to other animation states)
	uint8 FramePlayTime = 0;

	int32 HitStop;
};

UCLASS()
class BREAKTHROUGH_API ABTVFXBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABTVFXBase();

	void AssignOwner(ABTCharacterBase* NewOwner) { Owner = NewOwner; }

	FEffectState CurrentState;

	virtual void Activate(FVector2D Location, bool bFacingRight);

	virtual void Update();

	virtual void DrawEffect();

	virtual void CreateMaterials();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		TArray<uint8> AnimFrames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UMaterialBillboardComponent* Billboard;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
