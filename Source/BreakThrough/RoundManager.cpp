// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "RoundManager.h"


// Sets default values
ARoundManager::ARoundManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //Gamestate will update this

	//Create and add HUD widget
	static ConstructorHelpers::FClassFinder<UHUDVisuals> HUDWidget(TEXT("/Game/UI/Blueprints/HUD_Resolve"));
	HUDWidgetClass = HUDWidget.Class;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	MainCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Main Camera"));
	MainCamera->SetupAttachment(RootComponent);

	HUDWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("UpperHUD");
	HUDWidgetComponent->SetupAttachment(MainCamera);

	//Possess the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ARoundManager::BeginPlay()
{
	Super::BeginPlay();
	yOffset = 105.0f;
	zPosMax = 2500.0f;
	zPosMin = 2200.0f;
	zPos = 2200.0f;
	//Create HUDs and add it to camera/world space
	LowerHUD = CreateWidget<UHUDVisuals>(GetWorld()->GetGameInstance(), HUDWidgetClass);
	LowerHUD->AddToViewport(0);
	UpperHUD = Cast<UHUDVisuals>(HUDWidgetComponent->GetUserWidgetObject());

	/*Assume 60 FPS. Change number if a longer/short in game second is desired
	60 * (Real world seconds length) = Number of frames to check*/
	gameTime = 60;
	roundTimer = 99;
	//Change any values here based on player settings (ex: max rounds or round time)
	RoundStart(); //Temporary, remove this when there's a round start animation
}

// Called every frame
void ARoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoundManager::UpdateCameraPosition(FVector Position, FRotator Rotation) 
{
	MainCamera->SetRelativeLocation(Position);
	MainCamera->SetRelativeRotation(Rotation);
}

//Update function to be used by GameState
void ARoundManager::UpdateTimer()
{
	if (gameActive)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GameActive"));
		if (!suddenDeath)
		{
			if (Player1Base && Player2Base)
				if (Player1Base->CurrentState.SlowMoTime % 2 == 0 && Player2Base->CurrentState.SlowMoTime % 2 == 0)
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

	//Update camera position
	if (abs(Player1Base->CurrentState.Position.X - Player2Base->CurrentState.Position.X) > 250.0f)
	{
		if (zPos < zPosMax) 
		{
			zPos += 25.0f;
		}
	}
	else 
	{
		if (zPos > zPosMin)
		{
			zPos -= 25.0f;
		}
	}


	UpdateCameraPosition(FVector((Player1Base->CurrentState.Position.X + Player2Base->CurrentState.Position.X) / 2, zPos, (Player1Base->CurrentState.Position.Y + Player2Base->CurrentState.Position.Y) / 2 + yOffset), FRotator(0.0f, -90.0f, 0.0f));

	
}

void ARoundManager::DrawScreen()
{
	//Update HUD
	UpperHUD->UpdateUpperHUD(roundTimer, Player1Base, Player2Base);
	LowerHUD->UpdateLowerHUD(Player1Base, Player2Base);

	//update camera/transform position from here
}

void ARoundManager::ResetPositions()
{
	roundCount++;
	roundTimer = 99;
	Player1Base->CurrentState.Position = P1startPosition;
	Player1Base->CurrentState.Health = Player1Base->MaxHealth;
	Player2Base->CurrentState.Position = P2startPosition;
	Player2Base->CurrentState.Health = Player2Base->MaxHealth;
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
	Player1Base->CurrentState.Resolve = 4;
	Player1Base->CurrentState.Durability = 100;
	Player2Base->CurrentState.Resolve = 4;
	Player2Base->CurrentState.Durability = 100;
	ResetPositions();
}

//Used to check if a win condition has been met 
void ARoundManager::DetermineWinMethod()
{
	if (!suddenDeath && roundTimer <= 0 && Player1Base->CurrentState.Health > 0 && Player2Base->CurrentState.Health > 0 && Player1Base->CurrentState.Health == Player2Base->CurrentState.Health)
	{
		RoundStop();
		suddenDeath = true;
		//Play Sudden Death Animation
		//UE_LOG(LogTemp, Warning, TEXT("Sudden Death"));
		RoundStart();
	}
	else if (!suddenDeath && roundTimer <= 0 && Player1Base->CurrentState.Health > 0 && Player2Base->CurrentState.Health > 0 && Player1Base->CurrentState.Health != Player2Base->CurrentState.Health)
	{
		RoundStop();
		//Play Time Up Animation
		//UE_LOG(LogTemp, Warning, TEXT("Time Out"));
		//Increment win count
		if (Player1Base->CurrentState.Health > Player2Base->CurrentState.Health)
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
	else if ((Player1Base->CurrentState.Health == Player1Base->MaxHealth && Player2Base->CurrentState.Health <= 0) || 
		(Player2Base->CurrentState.Health == Player2Base->MaxHealth && Player1Base->CurrentState.Health <= 0))
	{
		RoundStop();
		//Play Perfect KO Animation
		//UE_LOG(LogTemp, Warning, TEXT("Perfect"));
		//Increment win count
		if (Player2Base->CurrentState.Health <= 0)
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
	else if ((Player1Base->CurrentState.Health > 0 && Player2Base->CurrentState.Health <= 0) || (Player2Base->CurrentState.Health > 0 && Player1Base->CurrentState.Health <= 0))
	{
		RoundStop();
		//Play BreakDown Animation
		//UE_LOG(LogTemp, Warning, TEXT("Breakdown"));
		//Increment win count
		if (Player2Base->CurrentState.Health <= 0)
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
	else if (Player1Base->CurrentState.Health <= 0 && Player2Base->CurrentState.Health <= 0)
	{
		RoundStop();
		//Play Double KO Animation
		//UE_LOG(LogTemp, Warning, TEXT("Double KO"));
	}
}

