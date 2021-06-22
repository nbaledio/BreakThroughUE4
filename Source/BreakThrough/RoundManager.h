// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "HUDVisuals.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/Vector.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
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
	FVector CameraPosition;
	FRotator CameraRotation;

	// Round variables
	uint8 FrameCount;
	uint8 RoundCount;
	uint8 RoundTimer;
	uint8 P1Wins;
	uint8 P2Wins;
	uint8 MaxRounds;
	uint8 UniversalHitStop;

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
		float YPosMax = 2250;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float YPosMin = 2100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float XPosBound = 710.0f;

	//HUD Widgets
	UHUDVisuals* UpperHUD;
	UHUDVisuals* LowerHUD;

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

	int32 gameTime;

	FVector2D P1startPosition;
	FVector2D P2startPosition;

	bool gameActive;
	bool lockInputs;
	bool suddenDeath;

	TSubclassOf<class UHUDVisuals> HUDWidgetClass;

	//Functions
	void ResetPositions();
	void RoundStart();
	void RoundStop();
	void ResetGame();
	void DetermineWinMethod();
	void ActivateResolveBar(uint8 index, bool bReverse);
	void UpdateResolveBar(uint8 index);
};
