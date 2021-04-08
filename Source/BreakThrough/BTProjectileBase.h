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

	FVector2D Position;
	FVector2D Velocity;
	int32 CurrentLife;
	int32 CurrentHits;

	TArray<FHitbox>* CurrentHitbox;
	uint8 AnimFrameIndex;
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

	FProjectileState CurrentState;

	ABTCharacterBase* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D InitSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bAffectedByGravity = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bHasLifeSpan = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxHits;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 MaxLife;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
