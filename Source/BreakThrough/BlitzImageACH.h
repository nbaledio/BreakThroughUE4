// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlitzImageBase.h"
#include "BlitzImageACH.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABlitzImageACH : public ABlitzImageBase
{
	GENERATED_BODY()

public:

	virtual void DrawBlitz() override;

	virtual void CreateMaterials() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* SealBlitzMaterial;

	UMaterialInstanceDynamic* DynamicBodyMaterial;
	UMaterialInstanceDynamic* DynamicSpearMaterial;
	UMaterialInstanceDynamic* DynamicSealMaterial;
	
};
