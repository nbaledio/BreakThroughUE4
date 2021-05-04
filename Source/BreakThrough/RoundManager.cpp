// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "RoundManager.h"

//Start function
void RoundManager::BeginPlay()
{
	//Assume 60 FPS. Change number if a longer/short in game second is desired
	//60 * (Real world seconds length) = Number of frames to check
	gameTime = 60;
	//Change any values based on player settings (ex: max rounds or round time)
}

//Update function to be used by GameState
void RoundManager::UpdateTimer()
{
	if (gameActive) 
	{
		if (!suddenDeath) 
		{
			frameCount++;
			//Check if one in-game second has passed and decrement timer
			if (frameCount == gameTime && !suddenDeath)
			{
				frameCount = 0;
				roundTimer--;
			}
		}
		//Check if a win condition is met
		DetermineWinMethod();
	}
}

void RoundManager::ResetPositions()
{
	roundCount++;
	roundTimer = 99;
	Player1State->Position = P1startPosition;
	Player1State->Health = Player1Base->MaxHealth;
	Player2State->Position = P2startPosition;
	Player2State->Health = Player2Base->MaxHealth;
}

void RoundManager::RoundStart()
{
	gameActive = true;
	lockInputs = false;
}

void RoundManager::RoundStop()
{
	gameActive = false;
	lockInputs = true;
}

void RoundManager::ResetGame()
{
	roundCount = 0;
	p1Wins = 0;
	p2Wins = 0;
	Player1State->Resolve = 4;
	Player1State->Durability = 100;
	Player2State->Resolve = 4;
	Player2State->Durability = 100;
	ResetPositions();
}

//Used to check if a win condition has been met 
void RoundManager::DetermineWinMethod()
{
	if (!suddenDeath && roundTimer <= 0 && Player1State->Health > 0 && Player2State->Health > 0 && Player1State->Health == Player2State->Health)
	{
		RoundStop();
		suddenDeath = true;
		//Play Sudden Death Animation
	}
	else if (!suddenDeath && roundTimer <= 0 && Player1State->Health > 0 && Player2State->Health > 0 && Player1State->Health != Player2State->Health)
	{
		RoundStop();
		//Play Time Up Animation
		//Increment win count
		if (Player1State->Health > Player2State->Health)
		{
			p1Wins++;
		}
		else
		{
			p2Wins++;
		}
	}
	else if ((Player1State->Health == Player1Base->MaxHealth && Player2State->Health <= 0) || (Player2State->Health == Player2Base->MaxHealth && Player1State->Health <= 0))
	{
		RoundStop();
		//Play Perfect KO Animation
		//Increment win count
		if (Player2State->Health <= 0)
		{
			p1Wins++;
		}
		else 
		{
			p2Wins++;
		}
	}
	else if ((Player1State->Health > 0 && Player2State->Health <= 0) || (Player2State->Health > 0 && Player1State->Health <= 0))
	{
		RoundStop();
		//Play BreakDown Animation
		//Increment win count
		if (Player2State->Health <= 0)
		{
			p1Wins++;
		}
		else
		{
			p2Wins++;
		}
	}
	else if (Player1State->Health <= 0 && Player2State->Health <= 0)
	{
		RoundStop();
		//Play Double KO Animation
	}
}