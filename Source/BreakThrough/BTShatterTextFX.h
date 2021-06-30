// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTVFXBase.h"
#include "BTShatterTextFX.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTShatterTextFX : public ABTVFXBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABTShatterTextFX();

	virtual void Activate(FVector2D Location, bool bFacingRight, int32 HitInfo = 0, uint8 InteractType = 0) override;

	virtual void Update() override;

	virtual void DrawEffect() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Text;
};
