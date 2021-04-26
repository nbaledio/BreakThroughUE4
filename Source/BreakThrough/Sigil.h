// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sigil.generated.h"


class ABTCharacterBase;

USTRUCT(BlueprintType)
struct FSigilState
{
	GENERATED_BODY()

	bool bIsActive;
	FVector2D Position;
	FRotator Rotation;

	float MainEmissiveAlpha;
	float MainScaleAlpha;
	float EchoScaleAlpha;
};

UCLASS()
class BREAKTHROUGH_API ASigil : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASigil();

	void Activate(FVector2D Location, FRotator Orientation);

	void Update();

	void DrawSigil();

	FSigilState CurrentState{true, FVector2D(0), FRotator(0), 0, 0, 0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		FVector SigilColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		FVector EchoColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
		UTexture* SigilImage;

	UMaterialInstanceDynamic* DynamicSigilMaterial;
	UMaterialInstanceDynamic* DynamicEchoMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* MainMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* Echo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	UMaterialInterface* BaseSigilMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parameters")
	UMaterialInterface* EchoSigilMaterial;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
