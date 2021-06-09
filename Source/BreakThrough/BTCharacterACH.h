// Copyright 2021 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTCharacterBase.h"
#include "BlitzImageACH.h"
#include "BTCharacterACH.generated.h"

enum Moves  //keeps track of what moves have already been used in a string using bit flags, may not have to include supers since they cant cancel into anything
{
	n5M = (1 << 0), //used for both air and ground normals, light attacks not listed since they can be used whenever available
	n5H = (1 << 1),
	n5B = (1 << 2),
	n2M = (1 << 3),
	n2H = (1 << 4),
	n2B = (1 << 5),
	n6L = (1 << 6),
	n6M = (1 << 7),
	n6B = (1 << 8), 
	nJM = (1 << 9),
	nJH = (1 << 10),
	nJB = (1 << 11),
	SpTowerLeap = (1 << 12), //need to keep track of which Specials Achealis has used since she can enter a state where she can cancel specials into each other
	SpLevelHell = (1 << 13), 
	SpHeavenClimber= (1 << 14),
	SpGenesisEdge = (1 << 15),
	SpStarfall = (1 << 16),
	/*IsSlash = (1 << 17),
	IsVertical = (1 << 18),
	IsHorizontal = (1 << 19),
	LowerBodyHit = (1 << 20),*/
};

enum ACHValues  //keeps track of index for Achealis' unique variables
{
	SpearGlow,
	MTowerLeap,
	WCActive,
	WCDuration,
};

UCLASS()
class BREAKTHROUGH_API ABTCharacterACH : public ABTCharacterBase
{
	GENERATED_BODY()

public:
	virtual void HitDetection() override;

	virtual void UpdateCharacter(int32 CurrentInputs, int32 FrameNumber) override;

	virtual void UpdatePosition() override;

	virtual void DrawCharacter() override;

	virtual void SetColor(uint8 ColorID) override;

protected:
	virtual bool NonKnockdownLanding() override; //transitions triggered by touching the ground while not in a hitstun animation

	virtual bool ConditionalTransitions() override; //Animation transitions triggered by fulfilling some condition within certain animations

	virtual bool PassiveTransitions() override; //Transitions that occur without the influence of outside factors or player input

	virtual bool ExitTimeTransitions() override; //Animation transitions triggered by finishing an animation, called from within PassiveTransitions

	virtual void AnimationEvents() override;

	virtual void CreateMaterials() override;

	virtual void LightSettings() override;

	virtual void SpawnPBS() override; //spawn in character's projectiles, blitz image, and sigils

	virtual void ResetSmear() override;

	virtual void DrawSmear() override;

	virtual bool NormalAttacks() override;

	virtual bool SpecialAttacks() override;

	virtual bool SuperAttacks() override;

	virtual void CreateVariables() override;

	FVector EffectColor = FVector(.85, .1, 1);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SpearBC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SpearSSS;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SpearILM;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SpearLines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SealsBC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* AltBodyBC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* Main;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* Metallic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* Specular;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* Seals;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* RightEye;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* SpearEdge;

	UMaterialInstanceDynamic* DynamicBodyMain;
	UMaterialInstanceDynamic* DynamicBodyMetallic;
	UMaterialInstanceDynamic* DynamicBodySpec;
	UMaterialInstanceDynamic* DynamicSeals;
	UMaterialInstanceDynamic* DynamicRightEye;
	UMaterialInstanceDynamic* DynamicSpearMetallic;
	UMaterialInstanceDynamic* DynamicSpearSpec;
	UMaterialInstanceDynamic* DynamicSpearEdge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> NormalJL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> NormalJM;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> NormalJH;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> NormalJB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal5L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal5M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal5H;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal5B;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal2M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal2H;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal2B;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal6L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal6M;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NormalAttacks")
		TArray<FAnimationFrame> Normal6B;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpecialAttacks")
		TArray<FAnimationFrame> TowerLeap;
};
