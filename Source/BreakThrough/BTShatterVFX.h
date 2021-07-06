// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTVFXBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "BTShatterVFX.generated.h"

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTShatterVFX : public ABTVFXBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABTShatterVFX();

	virtual void BeginPlay() override;

	virtual void Activate(FVector2D Location, bool bFacingRight, int32 HitInfo = 0, uint8 InteractType = 0) override;

	virtual void Update() override;

	virtual void DrawEffect() override;

	virtual void CreateMaterials() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UParticleSystemComponent* GlassParticlesLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UParticleSystemComponent* GlassParticlesRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UParticleSystemComponent* GlassParticlesKO;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Glass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* KOEffect;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* KORing;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* GlassMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* EffectMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
		UMaterialInterface* RingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* ShatterWholeOffsets;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* ShatterBrokenOffsets;

	UMaterialInstanceDynamic* DynamicGlassMaterial;
	UMaterialInstanceDynamic* DynamicEffectMaterial;
	UMaterialInstanceDynamic* DynamicRingMaterial;
	
};
