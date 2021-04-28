// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "BTProjectileBase.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "BTCharacterBase.h"
#include "HitboxTool.generated.h"

USTRUCT(BlueprintType)
struct FTestHurtbox
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hurtbox = character position + hurtbox position
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position
};

USTRUCT(BlueprintType)
struct FTestHitbox
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hitbox = character position + hitbox position
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position

	//attack properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		bool bNewHit = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 PotentialActions; //denotes the actions that become available to the character upon the hitbox making contact using bit flags
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 AttackProperties; // denotes the properties an attack has on normal hit using bit flags
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 CounterAttackProperties; // denotes the properties an attack has on counter using bit flags

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 BaseDamage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 DurabilityDamage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 ResolveDamage = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 AttackLevel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 AttackHeight = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStun = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStop = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		uint8 BaseBlockStun = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float InitProration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		float ForcedProration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialKnockBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialAirKnockBack;
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
		TArray<FTestHitbox> Hitboxes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurtboxes")
		TArray<FTestHurtbox> Hurtboxes;

	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushBoxHeight = 100;
	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushboxWidth = 50;
	UPROPERTY(EditAnywhere, Category = "Movement Properties")
		float PushboxVerticalOffset = 0;

	void DrawPushbox();

	void DrawHitbox(FTestHitbox Box);

	void DrawHurtbox(FTestHurtbox Box);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
