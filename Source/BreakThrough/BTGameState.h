// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "BTCharacterBase.h"

/**
 * 
 */
struct GameState
{
	void Init();
	void Update(int inputs[], int disconnect_flags);
	void LoadGameState();
	void SaveGameState();
	void DrawFrame();

public:
	int FrameNumber;
	ABTCharacterBase* Player[2];
	TArray<FCharacterState> CharacterStates;
	//RoundManagerState;
};
