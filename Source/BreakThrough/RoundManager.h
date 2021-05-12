// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BTCharacterBase.h"
#include "RoundManager.generated.h"

/**
-Add A camera manager at some point
 **/

enum RoundData
{
	frameNumber,
};

UCLASS()
class BREAKTHROUGH_API ARoundManager : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ARoundManager();
	//References for character variables
	ABTCharacterBase* Player1Base;
	ABTCharacterBase* Player2Base;
	FCharacterState* Player1State;
	FCharacterState* Player2State;
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

	FVector2D P1startPosition;
	FVector2D P2startPosition;

	bool gameActive;
	bool lockInputs;
	bool suddenDeath;

	//Functions
	void ResetPositions();
	void RoundStart();
	void RoundStop();
	void ResetGame();
	void DetermineWinMethod();

};
