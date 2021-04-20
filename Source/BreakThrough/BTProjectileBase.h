// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTProjectileBase.generated.h"

struct FHitbox;
class ABTCharacterBase;

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

	TArray<FHitbox>* CurrentHitbox;
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
		bool bCanTouchSurfaces; //projectile will stop upon touching walls of the floor
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	void AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactThrow(FHitbox Hitbox, int32 ThrowType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
