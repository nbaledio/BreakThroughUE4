// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "HUDVisuals.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Math/Vector.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "RoundManager.generated.h"

enum RoundData
{
	frameNumber,
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
		UCameraComponent* MainCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWidgetComponent* HUDWidgetComponent;

	//References for character variables
	ABTCharacterBase* Player1Base;
	ABTCharacterBase* Player2Base;
	FCharacterState* Player1State;
	FCharacterState* Player2State;

	//HUD Widgets
	UHUDVisuals* UpperHUD;
	UHUDVisuals* LowerHUD;

	//Functions
	void UpdateCameraPosition(FVector Position, FRotator Rotation);
	void UpdateTimer();

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
