// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "BTProjectileBase.generated.h"

class ABTCharacterBase;

USTRUCT(BlueprintType)
struct FHurtbox
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hurtbox = character position + hurtbox position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position
};

USTRUCT(BlueprintType)
struct FHitbox
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hitbox = character position + hitbox position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position

	//attack properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bNewHit = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 PotentialActions; //denotes the actions that become available to the character upon the hitbox making contact using bit flags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackProperties; // denotes the properties an attack has on normal hit using bit flags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 CounterAttackProperties; // denotes the properties an attack has on counter using bit flags

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 DurabilityDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 ResolveDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackHeight = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStun = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStop = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseBlockStun = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float InitProration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float ForcedProration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialKnockBack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialAirKnockBack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialCounterKnockBack;
};

USTRUCT(BlueprintType)
struct FProjectileAnimFrame
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitboxes")
		TArray<FHitbox> Hitboxes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitboxes")
		bool bCanHit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimationAsset* Pose;

	//Changes the main lighting angle on the character, snaps to stored positions while bCinematic, otherwise lerps to new positions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FRotator MainLightRotation;
	//Changes the fill light angle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FRotator FillLightRotation;
	//overrides color of mainlight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector MainLightColor;
	//overrides color of rim light
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector RimLightColor;
	//overrides color of fill light
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector FillLightColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		USoundCue* VoiceLines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		USoundCue* SFX;
};

USTRUCT(BlueprintType)
struct FProjectileState
{
	GENERATED_BODY()

	bool bIsActive;
	bool bFacingRight;
	bool bReflected;
	bool bHitByOwner; //a trigger for certain projectiles that activate when the user hits the projectile with an attack
	bool bHitFriend; //a trigger for certain projectiles that activate when this projectile hits a friendly projectile
	bool bHitByFriend; //a trigger for certain projectiles that activate when a friendly projectile hits this projectile
	bool bHitSuccess; //triggers anything that needs to happen when the projectile hits something
	bool bAttackMadeContact;
	bool bProjectileClash;

	FVector2D Position;
	FVector2D Velocity;
	int32 CurrentLife;
	int32 CurrentHits;

	FProjectileAnimFrame CurrentAnimFrame;
	TArray<int32> SpecialVariables; //Store any unique variables here
	uint8 AnimFrameIndex; //can be used to denote where in the projectile's animation sequence we are (jump to specific numbers to transition to other animation states)
	uint8 FramePlayTime = 0;

	int32 HitStop;
};

UCLASS()
class BREAKTHROUGH_API ABTProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABTProjectileBase();

	void AssignOwner(ABTCharacterBase* NewOwner) { Owner = NewOwner; }

	FProjectileState CurrentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D InitSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bAffectedByGravity = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bHasLifeSpan = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bHasHits = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bCanTouchSurfaces; //projectile will stop upon touching walls or the floor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bAttackOwner = true; //hurt the owner of the projectile if the projectile is hit by an opponent's attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bCheckFriends; //whether to check if this projectile is hitting projectiles from the same owner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bCheckHitByOwner; //whether to check if this projectile is being hit by the owner

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D SurfaceBounds = FVector2D(0);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxHits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxLife;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxFrameIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float LightProximity; //if > 0, lights the character with the color of the projectile when within this distance

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector LightColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float LightVectorY;

	virtual void Activate(bool FacingRight);

	virtual void HitDetection();

	virtual void UpdateProjectile();

	virtual void UpdatePosition();

	virtual void DrawProjectile();  //set material parameters from child class

	virtual void CreateMaterials();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* BaseMesh;

	void AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactThrow(FHitbox Hitbox, int32 ThrowType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
