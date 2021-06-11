// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTVFXBase.h"
#include "BTGuardVFX.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTGuardVFX : public ABTVFXBase
{
	GENERATED_BODY()
	
public:

	ABTGuardVFX();

	virtual void Activate(FVector2D Location, bool bFacingRight, int32 HitInfo = 0, uint8 InteractType = 0) override;

	virtual void Update() override;

	virtual void DrawEffect() override;

	virtual void CreateMaterials() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* GuardMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* WaveMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* FlashMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* AirMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* FlashTexture;

	UMaterialInstanceDynamic* DynamicWaveMaterial;
	UMaterialInstanceDynamic* DynamicFlashMaterial;
	UMaterialInstanceDynamic* DynamicAirMaterial;
};
