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

	if (ResolveBar != nullptr)
	{
		for (uint8 i = 0; i < 8; i++)
		{
			FResolveBarState BarState;
			DynamicResolve.Add(UMaterialInstanceDynamic::Create(ResolveBar, this));
			CurrentState.ResolveStates.Add(BarState);

			if (i == 7)
				UE_LOG(LogTemp, Warning, TEXT("Created all dynamic resolve bar materials"));
		}
	}

	if (DynamicResolve.Num() == 8)
	{
		LowerHUD->P1ResolveBar1->SetBrushFromMaterial(DynamicResolve[0]);
		LowerHUD->P1ResolveBar2->SetBrushFromMaterial(DynamicResolve[1]);
		LowerHUD->P1ResolveBar3->SetBrushFromMaterial(DynamicResolve[2]);
		LowerHUD->P1ResolveBar4->SetBrushFromMaterial(DynamicResolve[3]);

		LowerHUD->P2ResolveBar1->SetBrushFromMaterial(DynamicResolve[4]);
		LowerHUD->P2ResolveBar2->SetBrushFromMaterial(DynamicResolve[5]);
		LowerHUD->P2ResolveBar3->SetBrushFromMaterial(DynamicResolve[6]);
		LowerHUD->P2ResolveBar4->SetBrushFromMaterial(DynamicResolve[7]);

		UE_LOG(LogTemp, Warning, TEXT("resolve bar materials have been assigned"));
	}

	CurrentState.CameraRotation = FRotator(0.0f, -90.0f, 0.0f);

	/*Assume 60 FPS. Change number if a longer/short in game second is desired
	60 * (Real world seconds length) = Number of frames to check*/
	gameTime = 60;
	CurrentState.RoundTimer = 99;
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
	if (CurrentState.bIsGameActive)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentState.bIsGameActive"));
		if (!CurrentState.bSuddenDeath)
		{
			if (Player1Base && Player2Base)
				if (Player1Base->CurrentState.SlowMoTime % 2 == 0 && Player2Base->CurrentState.SlowMoTime % 2 == 0)
					CurrentState.FrameCount++;
			//Check if one in-game second has passed and decrement timer
			if (CurrentState.FrameCount == gameTime && !CurrentState.bSuddenDeath)
			{
				CurrentState.FrameCount = 0;
				CurrentState.RoundTimer--;
			}
		}
		//Check if a win condition is met
		DetermineWinMethod();
	}

	UpdateCameraPosition();

	if (DynamicResolve.Num() == 8)
	{
		if (Player1Base)
		{
			if (Player1Base->CurrentState.Resolve == 4 && !CurrentState.ResolveStates[3].bIsActive && CurrentState.ResolveStates[3].AnimFrameIndex == 11)
				ActivateResolveBar(3, true);
			else if (Player1Base->CurrentState.Resolve < 4 && !CurrentState.ResolveStates[3].bIsActive && CurrentState.ResolveStates[3].AnimFrameIndex == 0)
				ActivateResolveBar(3, false);

			if (Player1Base->CurrentState.Resolve >= 3 && !CurrentState.ResolveStates[2].bIsActive && CurrentState.ResolveStates[2].AnimFrameIndex == 11)
				ActivateResolveBar(2, true);
			else if (Player1Base->CurrentState.Resolve < 3 && !CurrentState.ResolveStates[2].bIsActive && CurrentState.ResolveStates[2].AnimFrameIndex == 0)
				ActivateResolveBar(2, false);

			if (Player1Base->CurrentState.Resolve >= 2 && !CurrentState.ResolveStates[1].bIsActive && CurrentState.ResolveStates[1].AnimFrameIndex == 11)
				ActivateResolveBar(1, true);
			else if (Player1Base->CurrentState.Resolve < 2 && !CurrentState.ResolveStates[1].bIsActive && CurrentState.ResolveStates[1].AnimFrameIndex == 0)
				ActivateResolveBar(1, false);

			if (Player1Base->CurrentState.Resolve >= 1 && !CurrentState.ResolveStates[0].bIsActive && CurrentState.ResolveStates[0].AnimFrameIndex == 11)
				ActivateResolveBar(0, true);
			else if (Player1Base->CurrentState.Resolve < 1 && !CurrentState.ResolveStates[0].bIsActive && CurrentState.ResolveStates[0].AnimFrameIndex == 0)
				ActivateResolveBar(0, false);
		}

		if (Player2Base)
		{
			if (Player2Base->CurrentState.Resolve == 4 && !CurrentState.ResolveStates[7].bIsActive && CurrentState.ResolveStates[7].AnimFrameIndex == 11)
				ActivateResolveBar(3, true);
			else if (Player2Base->CurrentState.Resolve < 4 && !CurrentState.ResolveStates[7].bIsActive && CurrentState.ResolveStates[7].AnimFrameIndex == 0)
				ActivateResolveBar(3, false);

			if (Player2Base->CurrentState.Resolve >= 3 && !CurrentState.ResolveStates[6].bIsActive && CurrentState.ResolveStates[6].AnimFrameIndex == 11)
				ActivateResolveBar(2, true);
			else if (Player2Base->CurrentState.Resolve < 3 && !CurrentState.ResolveStates[6].bIsActive && CurrentState.ResolveStates[6].AnimFrameIndex == 0)
				ActivateResolveBar(2, false);

			if (Player2Base->CurrentState.Resolve >= 2 && !CurrentState.ResolveStates[5].bIsActive && CurrentState.ResolveStates[5].AnimFrameIndex == 11)
				ActivateResolveBar(1, true);
			else if (Player2Base->CurrentState.Resolve < 2 && !CurrentState.ResolveStates[5].bIsActive && CurrentState.ResolveStates[5].AnimFrameIndex == 0)
				ActivateResolveBar(1, false);

			if (Player2Base->CurrentState.Resolve >= 1 && !CurrentState.ResolveStates[4].bIsActive && CurrentState.ResolveStates[4].AnimFrameIndex == 11)
				ActivateResolveBar(0, true);
			else if (Player2Base->CurrentState.Resolve < 1 && !CurrentState.ResolveStates[4].bIsActive && CurrentState.ResolveStates[4].AnimFrameIndex == 0)
				ActivateResolveBar(0, false);
		}
	}

	if (CurrentState.ResolveStates.Num() == 8)
	{
		for (uint8 i = 0; i < 8; i++)
			UpdateResolveBar(i);
	}
	
}

void ARoundManager::DrawScreen()
{
	//Draw HUD updates
	UpperHUD->UpdateUpperHUD(CurrentState.FrameCount, CurrentState.RoundTimer, Player1Base, Player2Base);
	LowerHUD->UpdateLowerHUD(Player1Base, Player2Base);

	if (CurrentState.ResolveStates[3].AnimFrameIndex < 11)
	{
		LowerHUD->P1ResolveBar4->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[3]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[3].AnimFrameIndex % 3, CurrentState.ResolveStates[3].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P1ResolveBar4->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[2].AnimFrameIndex < 11)
	{
		LowerHUD->P1ResolveBar3->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[2]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[2].AnimFrameIndex % 3, CurrentState.ResolveStates[2].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P1ResolveBar3->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[1].AnimFrameIndex < 11)
	{
		LowerHUD->P1ResolveBar2->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[1]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[1].AnimFrameIndex % 3, CurrentState.ResolveStates[1].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P1ResolveBar2->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[0].AnimFrameIndex < 11)
	{
		LowerHUD->P1ResolveBar1->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[0]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[0].AnimFrameIndex % 3, CurrentState.ResolveStates[0].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P1ResolveBar1->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[7].AnimFrameIndex < 11)
	{
		LowerHUD->P2ResolveBar4->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[7]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[7].AnimFrameIndex % 3, CurrentState.ResolveStates[7].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P2ResolveBar4->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[6].AnimFrameIndex < 11)
	{
		LowerHUD->P2ResolveBar3->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[6]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[6].AnimFrameIndex % 3, CurrentState.ResolveStates[6].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P2ResolveBar3->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[5].AnimFrameIndex < 11)
	{
		LowerHUD->P2ResolveBar2->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[5]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[5].AnimFrameIndex % 3, CurrentState.ResolveStates[5].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P2ResolveBar2->SetVisibility(ESlateVisibility::Hidden);

	if (CurrentState.ResolveStates[4].AnimFrameIndex < 11)
	{
		LowerHUD->P2ResolveBar1->SetVisibility(ESlateVisibility::Visible);
		DynamicResolve[4]->SetVectorParameterValue(FName("AnimIndex"), FVector(CurrentState.ResolveStates[4].AnimFrameIndex % 3, CurrentState.ResolveStates[4].AnimFrameIndex / 3, 0));
	}
	else
		LowerHUD->P2ResolveBar1->SetVisibility(ESlateVisibility::Hidden);

	//draw updated camera/transform position from here
	SetActorLocation(CurrentState.Position);
	MainCamera->SetWorldLocation(CurrentState.CameraPosition);
	MainCamera->SetRelativeRotation(CurrentState.CameraRotation);
}

void ARoundManager::ResetPositions()
{
	CurrentState.RoundCount++;
	CurrentState.RoundTimer = 99;
	Player1Base->CurrentState.Position = P1startPosition;
	Player1Base->CurrentState.Health = Player1Base->MaxHealth;
	Player2Base->CurrentState.Position = P2startPosition;
	Player2Base->CurrentState.Health = Player2Base->MaxHealth;
}

void ARoundManager::RoundStart()
{
	CurrentState.bIsGameActive = true;
	CurrentState.bLockInputs = false;
}

void ARoundManager::RoundStop()
{
	CurrentState.bIsGameActive = false;
	CurrentState.bLockInputs = true;
}

void ARoundManager::ResetGame()
{
	CurrentState.RoundCount = 0;
	CurrentState.P1Wins = 0;
	CurrentState.P2Wins = 0;
	Player1Base->CurrentState.Resolve = 4;
	Player1Base->CurrentState.Durability = 100;
	Player2Base->CurrentState.Resolve = 4;
	Player2Base->CurrentState.Durability = 100;
	ResetPositions();
}

//Used to check if a win condition has been met 
void ARoundManager::DetermineWinMethod()
{
	if (!CurrentState.bSuddenDeath && CurrentState.RoundTimer <= 0 && Player1Base->CurrentState.Health > 0 && Player2Base->CurrentState.Health > 0 && Player1Base->CurrentState.Health == Player2Base->CurrentState.Health)
	{
		RoundStop();
		CurrentState.bSuddenDeath = true;
		//Play Sudden Death Animation
		//UE_LOG(LogTemp, Warning, TEXT("Sudden Death"));
		RoundStart();
	}
	else if (!CurrentState.bSuddenDeath && CurrentState.RoundTimer <= 0 && Player1Base->CurrentState.Health > 0 && Player2Base->CurrentState.Health > 0 && Player1Base->CurrentState.Health != Player2Base->CurrentState.Health)
	{
		RoundStop();
		//Play Time Up Animation
		//UE_LOG(LogTemp, Warning, TEXT("Time Out"));
		//Increment win count
		if (Player1Base->CurrentState.Health > Player2Base->CurrentState.Health)
		{
			CurrentState.P1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			CurrentState.P2Wins++;
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
			CurrentState.P1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			CurrentState.P2Wins++;
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
			CurrentState.P1Wins++;
			//UE_LOG(LogTemp, Warning, TEXT("P1 Wins"));
		}
		else
		{
			CurrentState.P2Wins++;
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

void ARoundManager::ActivateResolveBar(uint8 index, bool bReverse)
{
	CurrentState.ResolveStates[index].bIsActive = true;
	CurrentState.ResolveStates[index].bReverse = bReverse;
	CurrentState.ResolveStates[index].FramePlayTime = 0;

	if (bReverse)
		CurrentState.ResolveStates[index].AnimFrameIndex = 11;
	else
		CurrentState.ResolveStates[index].AnimFrameIndex = 0;
}

void ARoundManager::UpdateResolveBar(uint8 index)
{
	if (CurrentState.ResolveStates[index].bIsActive)
	{
		CurrentState.ResolveStates[index].FramePlayTime++;

		if ((CurrentState.ResolveStates[index].FramePlayTime == 2 && !CurrentState.ResolveStates[index].bReverse) || (CurrentState.ResolveStates[index].FramePlayTime == 3 && CurrentState.ResolveStates[index].bReverse))
		{
			CurrentState.ResolveStates[index].FramePlayTime = 0;

			if (CurrentState.ResolveStates[index].bReverse)
			{
				CurrentState.ResolveStates[index].AnimFrameIndex--;
				if (CurrentState.ResolveStates[index].AnimFrameIndex == 0)
					CurrentState.ResolveStates[index].bIsActive = false;
			}
			else
			{
				CurrentState.ResolveStates[index].AnimFrameIndex++;
				if (CurrentState.ResolveStates[index].AnimFrameIndex == 11)
					CurrentState.ResolveStates[index].bIsActive = false;
			}
		}
	}
}

