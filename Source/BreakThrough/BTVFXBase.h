// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTVFXBase.generated.h"

enum Interactions
{
	Hit = 0,
	CounterHit,
	Guard,
	JustGuard,
	Resolve,
	HeavyResolve,
	Clash,
	Deflect,
	AirDash,
	KO,
};

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
	int32 HitProperties = 0;
	uint8 Interaction = 0;

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

	virtual void Activate(FVector2D Location, bool bFacingRight, int32 HitInfo = 0, uint8 InteractType = 0);

	virtual void Update();

	virtual void DrawEffect();

	virtual void CreateMaterials();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bAffectedBySlowMo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		TArray<uint8> AnimFrames;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
