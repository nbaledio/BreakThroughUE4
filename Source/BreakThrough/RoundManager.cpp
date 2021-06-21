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

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Scene Capture");
	SceneCapture->SetupAttachment(MainCamera);

	HUDWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("UpperHUD");
	HUDWidgetComponent->SetupAttachment(MainCamera);

	//Possess the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ARoundManager::BeginPlay()
{
	Super::BeginPlay();

	//Create HUDs and add it to camera/world space
	LowerHUD = CreateWidget<UHUDVisuals>(GetWorld()->GetGameInstance(), HUDWidgetClass);
	LowerHUD->AddToViewport(0);
	UpperHUD = Cast<UHUDVisuals>(HUDWidgetComponent->GetUserWidgetObject());

	CurrentState.CameraRotation = FRotator(0.0f, -90.0f, 0.0f);

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

void ARoundManager::SceneCaptureList()
{
	//Makes it only certain objects are picked up by the scene capture
	if (Player1Base)
	{
		SceneCapture->ShowOnlyActorComponents(Player1Base, true);
		SceneCapture->ShowOnlyActorComponents(Player1Base->BlitzImage, true);

		for (ASigil* Sigil : Player1Base->Sigils)
		{
			SceneCapture->ShowOnlyActorComponents(Sigil, true);
		}

		for (ABTProjectileBase* Projectile : Player1Base->Projectiles)
		{
			SceneCapture->ShowOnlyActorComponents(Projectile, true);
		}

		for (ABTVFXBase* VFX : Player1Base->SpecialVFX)
		{
			SceneCapture->ShowOnlyActorComponents(VFX, true);
		}
	}

	if (Player2Base)
	{
		SceneCapture->ShowOnlyActorComponents(Player2Base, true);
		SceneCapture->ShowOnlyActorComponents(Player2Base->BlitzImage, true);

		for (ASigil* Sigil : Player2Base->Sigils)
		{
			SceneCapture->ShowOnlyActorComponents(Sigil, true);
		}

		for (ABTProjectileBase* Projectile : Player2Base->Projectiles)
		{
			SceneCapture->ShowOnlyActorComponents(Projectile, true);
		}

		for (ABTVFXBase* VFX : Player2Base->SpecialVFX)
		{
			SceneCapture->ShowOnlyActorComponents(VFX, true);
		}
	}
	//add stage actor to list as well
}

void ARoundManager::UpdateCameraPosition() 
{
	CurrentState.Position = FVector(CurrentState.Position.X, 0, 0);
	
	if (FMath::Abs(Player1Base->CurrentState.Position.X - Player2Base->CurrentState.Position.X) <  PlayerMaxDistance)
	{
		CurrentState.Position.X = (Player1Base->CurrentState.Position.X + Player2Base->CurrentState.Position.X) / 2;
	}

	if (Player1Base->CurrentState.bIsAirborne || Player2Base->CurrentState.bIsAirborne)
		CurrentState.Position.Z = ((Player1Base->CurrentState.Position.Y + Player1Base->AirPushboxVerticalOffset) + (Player2Base->CurrentState.Position.Y + Player2Base->AirPushboxVerticalOffset)) * .65;
	/*else if (Player1Base->CurrentState.bIsAirborne || Player2Base->CurrentState.bIsAirborne)
		CurrentState.Position.Z = ((Player1Base->CurrentState.Position.Y + Player1Base->AirPushboxVerticalOffset) + (Player2Base->CurrentState.Position.Y + Player2Base->AirPushboxVerticalOffset)) * .85;*/

	//FRotator(0.0f, -90.0f, 0.0f)
	if (Player1Base->CurrentState.CurrentAnimFrame.CameraLocation != FVector(0)) //check if Player 1's animation has a stored camera location
	{
		if (Player1Base->CurrentState.CurrentAnimFrame.bCinematic)
		{
			CurrentState.CameraPosition = Player1Base->CurrentState.CurrentAnimFrame.CameraLocation;
		}
		else
		{
			if (Player1Base->CurrentState.PosePlayTime == 0)
				CurrentState.CameraPosition = FMath::Lerp(CurrentState.CameraPosition, Player1Base->CurrentState.CurrentAnimFrame.CameraLocation, .5f);
		}
	}
	else if (Player2Base->CurrentState.CurrentAnimFrame.CameraLocation != FVector(0)) // check if Player 2's animation has a stored camera location
	{
		if (Player2Base->CurrentState.CurrentAnimFrame.bCinematic)
		{
			CurrentState.CameraPosition = Player2Base->CurrentState.CurrentAnimFrame.CameraLocation;
		}
		else
		{
			if (Player2Base->CurrentState.PosePlayTime == 0)
				CurrentState.CameraPosition = FMath::Lerp(CurrentState.CameraPosition, Player2Base->CurrentState.CurrentAnimFrame.CameraLocation, .55f);
		}
	}
	else //otherwise normal gameplay camera
	{
		FVector TargetPosition = FVector(CurrentState.Position.X, YPosMin, CurrentState.Position.Z);

		if (TargetPosition.X < -XPosBound) //limit camera horizontal position
			TargetPosition.X = -XPosBound;
		else if (TargetPosition.X > XPosBound)
			TargetPosition.X = XPosBound;

		if (TargetPosition.Z < ZPosMin) //limit camera vertical position
			TargetPosition.Z = ZPosMin;
		else if (TargetPosition.Z > ZPosMax)
			TargetPosition.Z = ZPosMax;

		//Decide whether to zoom in or out based on characters' distance from each other
		TargetPosition.Y = FMath::Lerp(YPosMin, YPosMax, (FMath::Max((FMath::Abs(Player1Base->CurrentState.Position.X - Player2Base->CurrentState.Position.X) - 200), 0.f)/(PlayerMaxDistance - 200)));

		CurrentState.CameraPosition.X = FMath::Lerp(CurrentState.CameraPosition.X, TargetPosition.X, .45f);
		CurrentState.CameraPosition.Y = FMath::Lerp(CurrentState.CameraPosition.Y, TargetPosition.Y, .075f);
		CurrentState.CameraPosition.Z = FMath::Lerp(CurrentState.CameraPosition.Z, TargetPosition.Z, .25f);
	}

	if (Player1Base->CurrentState.CurrentAnimFrame.CameraRotation != FRotator(0)) //check if Player 1's animation has a stored camera rotation
	{
		if (Player1Base->CurrentState.CurrentAnimFrame.bCinematic)
		{
			CurrentState.CameraRotation = Player1Base->CurrentState.CurrentAnimFrame.CameraRotation;
		}
		else
		{
			if (Player1Base->CurrentState.PosePlayTime == 0)
				CurrentState.CameraRotation = FMath::Lerp(CurrentState.CameraRotation, Player1Base->CurrentState.CurrentAnimFrame.CameraRotation, .15f);
		}
	}
	else if (Player2Base->CurrentState.CurrentAnimFrame.CameraRotation != FRotator(0)) //check if Player 2's animation has a stored camera rotation
	{
		if (Player2Base->CurrentState.CurrentAnimFrame.bCinematic)
		{
			CurrentState.CameraRotation = Player2Base->CurrentState.CurrentAnimFrame.CameraRotation;
		}
		else
		{
			if (Player2Base->CurrentState.PosePlayTime == 0)
				CurrentState.CameraRotation = FMath::Lerp(CurrentState.CameraRotation, Player1Base->CurrentState.CurrentAnimFrame.CameraRotation, .15f);
		}
	}
	else //otherwise normal gameplay camera
	{
		CurrentState.CameraRotation = FMath::Lerp(CurrentState.CameraRotation, FRotator(0.0f, -90.0f, 0.0f), .15);

		if (CurrentState.CameraRotation.Yaw + 90 <= 1 && CurrentState.CameraRotation.Yaw != -90)
			CurrentState.CameraRotation.Yaw = -90;
	}
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

	UpdateCameraPosition();

	
}

void ARoundManager::DrawScreen()
{
	//Update HUD
	UpperHUD->UpdateUpperHUD(roundTimer, Player1Base, Player2Base);
	LowerHUD->UpdateLowerHUD(Player1Base, Player2Base);

	//update camera/transform position from here
	SetActorLocation(CurrentState.Position);
	MainCamera->SetWorldLocation(CurrentState.CameraPosition);
	MainCamera->SetRelativeRotation(CurrentState.CameraRotation);
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

