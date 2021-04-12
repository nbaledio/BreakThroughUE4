// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "BTProjectileBase.generated.h"

struct FHitbox;
class ABTCharacterBase;

USTRUCT(BlueprintType)
struct FProjectileState
{
	GENERATED_BODY()

	bool bIsActive;
	bool bIsReflected;
	bool bCanHurtOwner;

	FVector2D Position;
	FVector2D Velocity;
	int32 CurrentLife;
	int32 CurrentHits;

	TArray<FHitbox>* CurrentHitbox;
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
		bool bAttackOwner = true; //hurt the owner of the projectile if the projectile is hit by an opponent's attack

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxHits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxLife;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float LightProximity; //if > 0, lights the character with the color of the projectile when within this distance

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector LightColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float LightVectorY;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ABTCharacterBase* Owner;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
