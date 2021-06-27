// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "HUDVisuals.h"
#include "UpperHUD.h"
#include "LowerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/Vector.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "RoundManager.generated.h"

USTRUCT(BlueprintType)
struct FResolveBarState
{
	GENERATED_BODY()

	bool bIsActive;
	bool bReverse;

	uint8 AnimFrameIndex;
	uint8 FramePlayTime;
};

USTRUCT(BlueprintType)
struct FRoundManagerState
{
	GENERATED_BODY()

	FVector Position;
	FRotator Rotation;
	FVector CameraPosition;
	FRotator CameraRotation;

	// Round variables
	uint8 FrameCount;
	uint8 RoundCount;
	uint8 RoundTimer;
	uint8 P1Wins;
	uint8 P2Wins;
	int32 P1Health;
	int32 P2Health;
	uint8 MaxRounds;
	uint8 UniversalHitStop;

	float KOAlpha = 0;

	bool bIsGameActive;
	bool bLockInputs;
	bool bSuddenDeath;

	TArray<FResolveBarState> ResolveStates;
};

UCLASS()
class BREAKTHROUGH_API ARoundManager : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ARoundManager();

	//Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		UCineCameraComponent* MainCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneCaptureComponent2D* SceneCapture;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWidgetComponent* HUDWidgetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UParticleSystemComponent* P1Particles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UParticleSystemComponent* P2Particles;

	FRoundManagerState CurrentState;

	//References for character variables
	ABTCharacterBase* Player1Base;
	ABTCharacterBase* Player2Base;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float PlayerMaxDistance = 400;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float ZPosMin = 105;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float ZPosMax = 500;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float YPosMax = 2200;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float YPosMin = 2100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float XPosBound;

	bool bCinematicCamera;

	//HUD Widgets
	UUpperHUD* UpperHUD;
	ULowerHUD* LowerHUD;

	//ResolveBar Material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* ResolveBar;

	TArray<UMaterialInstanceDynamic*> DynamicResolve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float CameraSpeed = 1;

	//Functions
	void UpdateCameraPosition();
	void UpdateTimer();
	void SceneCaptureList();
	void DrawScreen();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Number of frames -> 1 in-game second
	int32 gameTime;

	bool bRoundReset;
	bool bRoundStart;

	//Store class of widget from constructor
	TSubclassOf<class ULowerHUD> HUDWidgetClass;

	//UI Animation Delegates
	FWidgetAnimationDynamicEvent ResetPositionsDelegate;
	FWidgetAnimationDynamicEvent RoundStartDelegate;

	//Functions
	void ResetPositions();
	void RoundStart();
	void RoundStop();
	void ResetGame();
	void DetermineWinMethod();
	void ActivateResolveBar(uint8 index, bool bReverse);
	void UpdateResolveBar(uint8 index);
	UFUNCTION()
		void NotifyRoundEnd();
	UFUNCTION()
		void NotifyRoundStart();
};
