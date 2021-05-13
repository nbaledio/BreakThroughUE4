// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "RoundManager.h"

// Sets default values
ARoundManager::ARoundManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //Gamestate will update this
}

// Called when the game starts or when spawned
void ARoundManager::BeginPlay()
{
	Super::BeginPlay();
	//Assume 60 FPS. Change number if a longer/short in game second is desired
	//60 * (Real world seconds length) = Number of frames to check
	gameTime = 60;
	roundTimer = 99;
	//Change any values based on player settings (ex: max rounds or round time)
	RoundStart(); //Temp, remove this when there's a round start animation
}

// Called every frame
void ARoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);;
}

//Update function to be used by GameState
void ARoundManager::UpdateTimer()
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

void ARoundManager::ResetPositions()
{
	roundCount++;
	roundTimer = 99;
	Player1State->Position = P1startPosition;
	Player1State->Health = Player1Base->MaxHealth;
	Player2State->Position = P2startPosition;
	Player2State->Health = Player2Base->MaxHealth;
}

void ARoundManager::RoundStart()
{
	gameActive = true;
	lockInputs = false;
}

void ARoundManager::RoundStop()
{
	gameActive = false;
	lockInputs = true;
}

void ARoundManager::ResetGame()
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
void ARoundManager::DetermineWinMethod()
{
	if (!suddenDeath && roundTimer <= 0 && Player1State->Health > 0 && Player2State->Health > 0 && Player1State->Health == Player2State->Health)
	{
		RoundStop();
		suddenDeath = true;
		//Play Sudden Death Animation
		//UE_LOG(LogTemp, Warning, TEXT("Sudden Death"));
		RoundStart();
	}
	else if (!suddenDeath && roundTimer <= 0 && Player1State->Health > 0 && Player2State->Health > 0 && Player1State->Health != Player2State->Health)
	{
		RoundStop();
		//Play Time Up Animation
		//UE_LOG(LogTemp, Warning, TEXT("Time Out"));
		//Increment win count
		if (Player1State->Health > Player2State->Health)
		{
			p1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			p2Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P2 Wins"));
		}
	}
	else if ((Player1State->Health == Player1Base->MaxHealth && Player2State->Health <= 0) || (Player2State->Health == Player2Base->MaxHealth && Player1State->Health <= 0))
	{
		RoundStop();
		//Play Perfect KO Animation
		UE_LOG(LogTemp, Warning, TEXT("Perfect"));
		//Increment win count
		if (Player2State->Health <= 0)
		{
			p1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			p2Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P2 Wins"));
		}
	}
	else if ((Player1State->Health > 0 && Player2State->Health <= 0) || (Player2State->Health > 0 && Player1State->Health <= 0))
	{
		RoundStop();
		//Play BreakDown Animation
		//UE_LOG(LogTemp, Warning, TEXT("Breakdown"));
		//Increment win count
		if (Player2State->Health <= 0)
		{
			p1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			p2Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P2 Wins"));
		}
	}
	else if (Player1State->Health <= 0 && Player2State->Health <= 0)
	{
		RoundStop();
		//Play Double KO Animation
		//UE_LOG(LogTemp, Warning, TEXT("Double KO"));
	}
}

