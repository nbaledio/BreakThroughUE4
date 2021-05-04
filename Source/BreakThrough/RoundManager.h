// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTCharacterBase.h"


/**
-Add A camera manager at some point
 **/
enum RoundData
{
	frameNumber,
};

class BREAKTHROUGH_API RoundManager
{
public:
	//References for character variables
	ABTCharacterBase* Player1Base;
	ABTCharacterBase* Player2Base;
	FCharacterState* Player1State;
	FCharacterState* Player2State;

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
	void BeginPlay();
	void UpdateTimer();
	void ResetPositions();
	void RoundStart();
	void RoundStop();
	void ResetGame();
	void DetermineWinMethod();
};
