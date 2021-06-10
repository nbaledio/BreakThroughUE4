// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTVFXBase.h"
#include "BTHitFX.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTHitFX : public ABTVFXBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABTHitFX();

	virtual void Activate(FVector2D Location, bool bFacingRight, int32 HitInfo = 0, uint8 InteractType = 0) override;

	virtual void Update() override;

	virtual void DrawEffect() override;

	virtual void CreateMaterials() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Billboard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Spark;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Cross;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* EffectMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* BillboardTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* SparkTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* CrossTexture00;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* CrossTexture01;

	UMaterialInstanceDynamic* DynamicBillboardMaterial;
	UMaterialInstanceDynamic* DynamicSparkMaterial;
	UMaterialInstanceDynamic* DynamicCrossMaterial;

};
