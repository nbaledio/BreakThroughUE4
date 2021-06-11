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
#include "Components/WidgetComponent.h"
#include "RoundManager.generated.h"

USTRUCT(BlueprintType)
struct FRoundManagerState
{
	GENERATED_BODY()

	FVector Position;
	FVector CameraPosition;
	FRotator CameraRotation;

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
		UWidgetComponent* HUDWidgetComponent;

	FRoundManagerState CurrentState;

	//References for character variables
	ABTCharacterBase* Player1Base;
	ABTCharacterBase* Player2Base;

	//HUD Widgets
	UHUDVisuals* UpperHUD;
	UHUDVisuals* LowerHUD;

	//Functions
	void UpdateCameraPosition(FVector Position, FRotator Rotation);
	void UpdateTimer();
	void DrawScreen();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Round variables
	int32 roundCount;
	int32 roundTimer;
	int32 frameCount;
	int32 p1Wins;
	int32 p2Wins;
	int32 maxRounds;

	int32 gameTime;

	float yOffset;
	float zPosMax;
	float zPosMin;
	float zPos;

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
};
