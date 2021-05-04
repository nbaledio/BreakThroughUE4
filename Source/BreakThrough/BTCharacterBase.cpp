// Copyright 2021 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterBase.h"

// Sets default values
ABTCharacterBase::ABTCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //false; most likely will update this actor from gamestatebase

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	CharacterVoice = CreateDefaultSubobject<UAudioComponent>(TEXT("Character Voice"));
	CharacterVoice->SetupAttachment(RootComponent);

	CharacterSoundEffects = CreateDefaultSubobject<UAudioComponent>(TEXT("Character SFX"));
	CharacterSoundEffects->SetupAttachment(RootComponent);

	MainLightRotator = CreateDefaultSubobject<USceneComponent>(TEXT("Main Light Rotator"));
	MainLightRotator->SetupAttachment(BaseMesh);
	MainLight = CreateDefaultSubobject<USceneComponent>(TEXT("Main Light"));
	MainLight->SetupAttachment(MainLightRotator);

	FillLightRotator = CreateDefaultSubobject<USceneComponent>(TEXT("Fill Light Rotator"));
	FillLightRotator->SetupAttachment(BaseMesh);
	FillLight = CreateDefaultSubobject<USceneComponent>(TEXT("Fill Light"));
	FillLight->SetupAttachment(FillLightRotator);
}

// Called when the game starts or when spawned
void ABTCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentState.LineThickness = 3.f;
	CreateMaterials();
	SpawnPBS();
	SetColor(1);
	EnterNewAnimation(IdleStand);
}

// Called every frame
void ABTCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawCharacter();
}

void ABTCharacterBase::SuperFlashSolver() //Only play once from Player1
{
	if (Opponent != nullptr) //Keeps game from freezing or slowing down if both characters super flash on the exact same frame, Player 1 Anim will play first
	{
		if (CurrentState.CurrentAnimFrame.bSuperFlash)
		{
			Opponent->CurrentState.HitStop++;
			DepthOffset = 0;
			Opponent->DepthOffset = 300;
		}
		else if (Opponent->CurrentState.CurrentAnimFrame.bSuperFlash)
		{
			CurrentState.HitStop++;
			DepthOffset = 300;
			Opponent->DepthOffset = 0;
		}
		else if (Opponent->CurrentState.CurrentAnimFrame.bCinematic || CurrentState.CurrentAnimFrame.bCinematic)
		{
			DepthOffset = 0;
			Opponent->DepthOffset = 0;
		}
	}
}

void ABTCharacterBase::HitDetection()
{
	for (ABTProjectileBase* Projectile : Projectiles)
	{
		Projectile->HitDetection();
	}

	if (Opponent != nullptr && !CurrentState.bClash)
	{
		CurrentState.bHitSuccess = false;
		if (CurrentState.bBlitzing)
		{
			ProcessBlitz();
		}
		//only look for hits if there are hitboxes active, and the current hitbox has not hit anything previously
		else if (CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0 && !CurrentState.bAttackMadeContact && CurrentState.HitStop == 0)
		{
			if (CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight < Throw) //Current active attack is a strike
			{
				//loop through opponent's active hurtboxes and see if any current hitboxes overlap them
				if (Opponent->CurrentState.CurrentAnimFrame.Invincibility != StrikeInvincible && Opponent->CurrentState.CurrentAnimFrame.Invincibility != FullInvincible &&
					!(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == High && Opponent->CurrentState.bIsCrouching))
				{
					if (Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0 && !CurrentState.bAttackMadeContact && !Opponent->CurrentState.bAttackMadeContact) //only look for clashes if the opponent has an active attack out
					{
						if (Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
							ClashDetection();
					}
					if (Opponent->CurrentState.CurrentAnimFrame.Hurtboxes.Num() > 0)
					{
						if (Opponent->CurrentState.CurrentAnimFrame.Hurtboxes.Num() > 0)
						{
							for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
							{
								FVector2D HitboxCenter;
								if (CurrentState.bFacingRight)
									HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
								else
									HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

								for (uint8 j = 0; j < Opponent->CurrentState.CurrentAnimFrame.Hurtboxes.Num() && !CurrentState.bAttackMadeContact; j++)
								{
									FVector2D OpponentHurtboxCenter;
									if (Opponent->CurrentState.bFacingRight)
										OpponentHurtboxCenter = Opponent->CurrentState.Position + Opponent->CurrentState.CurrentAnimFrame.Hurtboxes[j].Position;
									else
										OpponentHurtboxCenter = FVector2D(Opponent->CurrentState.Position.X - Opponent->CurrentState.CurrentAnimFrame.Hurtboxes[j].Position.X, Opponent->CurrentState.Position.Y + Opponent->CurrentState.CurrentAnimFrame.Hurtboxes[j].Position.Y);

									if (RectangleOverlap(HitboxCenter, OpponentHurtboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, Opponent->CurrentState.CurrentAnimFrame.Hurtboxes[j].Size))
									{
										CurrentState.bAttackMadeContact = true;
										if (Opponent->CurrentState.CurrentAnimFrame.Invincibility == SuperCounter || (((Opponent->CurrentState.CurrentAnimFrame.Invincibility == AllCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight != Unblockable) ||
											(Opponent->CurrentState.CurrentAnimFrame.Invincibility == HiCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight < Low) || (Opponent->CurrentState.CurrentAnimFrame.Invincibility == LowCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight == Low)) &&
											!(CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & IsSuper))) //check if the opponent is in a counter stance that can counter the current attack
										{
											CurrentState.HitStop = 24;
											Opponent->CurrentState.bHitSuccess = true;
											Opponent->CurrentState.bClash = true;
										}
										else
											ContactHit(CurrentState.CurrentAnimFrame.Hitboxes[i], OpponentHurtboxCenter);
									}
								}
							}
						}
					}
				}
			}
			//logic to follow for throws
			else if ((((CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == CommandThrow || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == Throw) && !Opponent->CurrentState.bIsAirborne) ||
				((CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == AirCommandThrow || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == AirThrow) && Opponent->CurrentState.bIsAirborne)) &&
				((Opponent->CurrentState.HitStun == 0) || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & ComboThrow || Opponent->IsCurrentAnimation(Opponent->Stagger) ||
					Opponent->IsCurrentAnimation(Opponent->Crumple)) && Opponent->CurrentState.BlockStun == 0 && Opponent->CurrentState.CurrentAnimFrame.Invincibility != ThrowInvincible &&
				Opponent->CurrentState.CurrentAnimFrame.Invincibility != FullInvincible && Opponent->CurrentState.CurrentAnimFrame.Invincibility != Intangible && Opponent->CurrentState.CurrentAnimFrame.Invincibility != OTG)
			{
				for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
				{
					FVector2D HitboxCenter;
					if (CurrentState.bFacingRight)
						HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
					else
						HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

					FVector2D OpponentPushboxCenter = FVector2D(Opponent->CurrentState.Position.X, Opponent->CurrentState.Position.Y);
					FVector2D OpponentPushboxSize = FVector2D(Opponent->PushboxWidth, Opponent->StandingPushBoxHeight);

					if (Opponent->CurrentState.bIsAirborne)
					{
						OpponentPushboxCenter.Y += .5f * Opponent->CrouchingPushBoxHeight + Opponent->AirPushboxVerticalOffset;
						OpponentPushboxSize.Y = Opponent->CrouchingPushBoxHeight;
					}
					else if (Opponent->CurrentState.bIsCrouching)
					{
						OpponentPushboxCenter.Y += .5f * Opponent->CrouchingPushBoxHeight;
						OpponentPushboxSize.Y = Opponent->CrouchingPushBoxHeight;
					}
					else
						OpponentPushboxCenter.Y += .5f * Opponent->StandingPushBoxHeight;

					if (RectangleOverlap(HitboxCenter, OpponentPushboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, OpponentPushboxSize))
					{
						CurrentState.bAttackMadeContact = true;
						ContactThrow(CurrentState.CurrentAnimFrame.Hitboxes[i], CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight);
					}
				}
			}
		}
	}
	CurrentState.bClash = false;
}

void ABTCharacterBase::UpdateCharacter(int32 CurrentInputs, int32 FrameNumber)
{
	//Check inputs and add them to InputQueue
	UpdateInputHistory(CurrentInputs, FrameNumber);

	//if (Inputs.Num() > GameState->FrameDelay)
	ProcessInputs(CurrentInputs); //(Inputs[GameState->FrameCount - GameState->FrameDelay]);

	if (CurrentState.HitStop == 0)
	{
		// update/look at AvailableActions
		ApplyKnockBack();
		LandingLagCheck();
		RunBraking();

		if (CurrentState.HitStun > 0)
			CurrentState.AvailableActions = None;
		else if (CurrentState.BlockStun > 0)
			CurrentState.AvailableActions = AcceptGuard;
		else
			CurrentState.CharacterHitState = 0;

		//Checking PosePlayTime < PlayDuration, Changing Animations, and Anim Transitions based on read inputs
		AnimationStateMachine();
		
		ProcessAnimationFrame();

		Guarding();
		GravityCalculation();

		if (CurrentState.AvailableActions & AcceptMove && CurrentInputs == 0) //Character is not inputting anything while in neutral
			CurrentState.Resolute = true; //Character with Resolute state will gain meter instead of being hit by a Blitz wave and will automatically counter normal throws
		else
			CurrentState.Resolute = false;
	}

	if (CurrentState.CurrentAnimFrame.Pose != nullptr)
		BaseMesh->PlayAnimation(CurrentState.CurrentAnimFrame.Pose, false);

	if (Opponent != nullptr)
	{
		if (!CurrentState.CurrentAnimFrame.bSuperFlash && !Opponent->CurrentState.CurrentAnimFrame.bSuperFlash)
		{
			for (ASigil* Sigil : Sigils)
			{
				Sigil->Update();
			}

			for (ABTProjectileBase* Projectile : Projectiles)
			{
				Projectile->UpdateProjectile();
			}
		}
	}

	if (BlitzImage != nullptr && !Opponent->CurrentState.CurrentAnimFrame.bSuperFlash)
		BlitzImage->Update();

	if (CurrentState.MainLightRotation != CurrentState.CurrentAnimFrame.MainLightRotation && CurrentState.CurrentAnimFrame.MainLightRotation != FRotator(0))
	{
		if (CurrentState.CurrentAnimFrame.bCinematic)
			CurrentState.MainLightRotation = CurrentState.CurrentAnimFrame.MainLightRotation;
		else
			CurrentState.MainLightRotation = FMath::Lerp(CurrentState.MainLightRotation, CurrentState.CurrentAnimFrame.MainLightRotation, .5f);
	}
	else
	{
		CurrentState.MainLightRotation = FMath::Lerp(CurrentState.MainLightRotation, DefaultMainLightRotation, .5f);
	}

	if (CurrentState.FillLightRotation != CurrentState.CurrentAnimFrame.FillLightRotation && CurrentState.CurrentAnimFrame.FillLightRotation != FRotator(0))
	{
		if (CurrentState.CurrentAnimFrame.bCinematic)
			CurrentState.FillLightRotation = CurrentState.CurrentAnimFrame.FillLightRotation;
		else
			CurrentState.FillLightRotation = FMath::Lerp(CurrentState.FillLightRotation, CurrentState.CurrentAnimFrame.FillLightRotation, .5f);
	}
	else
	{
		CurrentState.FillLightRotation = FMath::Lerp(CurrentState.FillLightRotation, DefaultFillLightRotation, .5f);
	}

	if (CurrentState.StatusTimer > 0)
		CurrentState.StatusTimer--;

	//first setting of light colors

	SetSounds();
}

void ABTCharacterBase::VelocitySolver()
{
	if (Opponent != nullptr)
	{
		if (CurrentState.bTouchingOpponent) 
		{
			if ((CurrentState.bIsRunning && Opponent->CurrentState.bIsRunning) || ((CurrentState.bFacingRight && CurrentState.Velocity.X > 0 && Opponent->CurrentState.Velocity.X < 0) || (!CurrentState.bFacingRight && CurrentState.Velocity.X < 0 && Opponent->CurrentState.Velocity.X > 0) &&
				CurrentState.AvailableActions & AcceptMove && Opponent->CurrentState.AvailableActions & AcceptMove)) //both characters stop moving if walking against each other
			{
					CurrentState.Velocity.X = 0;
					Opponent->CurrentState.Velocity.X = CurrentState.Velocity.X;
			}
		}
	}
}

void ABTCharacterBase::UpdatePosition() //update character's location based on velocity and decrement certain timed values
{
	for (ABTProjectileBase* Projectile : Projectiles)
	{
		Projectile->UpdatePosition();
	}

	if (CurrentState.HitStop == 0)
	{
		if (CurrentState.SlowMoTime % 2 == 0) //animation speed is halved and stun values decrease at half speed while in slow motion
		{
			if (CurrentState.HitStun > 0 && !IsCurrentAnimation(Sweep) && !IsCurrentAnimation(Crumple) && !IsCurrentAnimation(Tumble))
				CurrentState.HitStun--;
			if (CurrentState.BlockStun > 0)
				CurrentState.BlockStun--;
			if (CurrentState.WallBounceTime > 0)
				CurrentState.WallBounceTime--;
		}

		if (CurrentState.PosePlayTime < CurrentState.CurrentAnimFrame.PlayDuration)
		{
			if ((CurrentState.SlowMoTime > 0 || CurrentState.ShatteredTime > 0) && !CurrentState.CurrentAnimFrame.bSuperFlash && !CurrentState.CurrentAnimFrame.bCinematic)
				CurrentState.PosePlayTime += .5f;
			else
				CurrentState.PosePlayTime++;
		}

		if (!CurrentState.CurrentAnimFrame.bLockPosition)
		{
			uint8 DistanceScaler = 100;
			if (CurrentState.bIsRunning && !Opponent->CurrentState.bIsRunning && CurrentState.bTouchingOpponent) //opponent provides resistance when dashing against them
				DistanceScaler = 80;

			if (CurrentState.SlowMoTime > 0)
				CurrentState.Position += CurrentState.Velocity * DistanceScaler / 120.f;
			else
				CurrentState.Position += CurrentState.Velocity * DistanceScaler / 60.f;
		}

		if (CurrentState.Position.Y <= 0)
			CurrentState.Position.Y = 0;
		
		if (CurrentState.Position.Y > 0)
			CurrentState.bIsAirborne = true;

		if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
		{
			CurrentState.Position.X = -1000 + .5f * PushboxWidth;
		}
		else if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
		{
			CurrentState.Position.X = 1000 - .5f * PushboxWidth;
		}

		if (CurrentState.GravDefyTime > 0)
		{
			if (CurrentState.GravDefyTime == 1)
				CurrentState.Velocity.X *= .5f;
			CurrentState.GravDefyTime--;
		}
	}
	else if (CurrentState.HitStop > 0)
	{
		CurrentState.HitStop--;
	}

	if ((CurrentState.HitStun > 0 || CurrentState.CurrentAnimFrame.Invincibility == FaceDown || CurrentState.CurrentAnimFrame.Invincibility == FaceUp) && CurrentState.ShatteredTime == 0)
	{
		if (CurrentState.SlowMoTime % 2 == 0 && !Opponent->CurrentState.CurrentAnimFrame.bSuperFlash)
			CurrentState.ComboTimer++;
	}
	else
		CurrentState.ComboTimer = 0;

	UpdateResolve();
		
	if (CurrentState.ShatteredTime > 0 && !CurrentState.CurrentAnimFrame.bSuperFlash && !CurrentState.CurrentAnimFrame.bCinematic)
		CurrentState.ShatteredTime--;
	if (CurrentState.SlowMoTime > 0 && !CurrentState.CurrentAnimFrame.bSuperFlash && !CurrentState.CurrentAnimFrame.bCinematic)
		CurrentState.SlowMoTime--;

	if (Opponent != nullptr)
	{
		if ((Opponent->CurrentState.HitStun == 0 && !Opponent->CurrentState.bIsAirborne) || (Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.CurrentAnimFrame.Invincibility != FaceDown && Opponent->CurrentState.CurrentAnimFrame.Invincibility != FaceUp))
			CurrentState.ComboCount = 0;
	}	

	SaveFXStates();
	InputCountdown();
}

void ABTCharacterBase::PushboxSolver() //only called once per gamestate tick after updating characters, do not call on multiple characters
{
	if (Opponent != nullptr)
	{
		if (!CurrentState.CurrentAnimFrame.bCinematic && !Opponent->CurrentState.CurrentAnimFrame.bCinematic)
		{
			if (CurrentState.CurrentAnimFrame.Invincibility != Intangible && Opponent->CurrentState.CurrentAnimFrame.Invincibility != Intangible && FMath::Abs(Opponent->CurrentState.Position.X - CurrentState.Position.X) <= .5f * Opponent->PushboxWidth + .5f * PushboxWidth)
			{
				if (!CurrentState.bIsAirborne && !Opponent->CurrentState.bIsAirborne) //both on the ground
				{
					if (CurrentState.bTouchingWall)
					{
						if (CurrentState.bFacingRight)
							Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						else
							Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
					}
					else if (Opponent->CurrentState.bTouchingWall)
					{
						if (CurrentState.bFacingRight)
							CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						else
							CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
					}
					//when character is faster than opponent, and both are moving in the same direction or towards each other
					else if (FMath::Abs(CurrentState.Velocity.X) > FMath::Abs(Opponent->CurrentState.Velocity.X) && 
						((CurrentState.Velocity.X < 0 && Opponent->CurrentState.Velocity.X < 0) || (CurrentState.Velocity.X > 0 && Opponent->CurrentState.Velocity.X > 0) || (!CurrentState.bFacingRight && CurrentState.Velocity.X < 0 && Opponent->CurrentState.Velocity.X > 0) || (CurrentState.bFacingRight && CurrentState.Velocity.X > 0 && Opponent->CurrentState.Velocity.X < 0)))
					{
						if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
						{
							Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (Opponent->CurrentState.Position.X >= 1000 - .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								CurrentState.Velocity.X = 0;
							}
						}
						else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
						{
							Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (Opponent->CurrentState.Position.X <= -1000 + .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								CurrentState.Velocity.X = 0;
							}
						}
					}
					//when opponent is faster than character, and both are moving in the same direction or towards each other
					else if (FMath::Abs(CurrentState.Velocity.X) < FMath::Abs(Opponent->CurrentState.Velocity.X) && 
						((CurrentState.Velocity.X < 0 && Opponent->CurrentState.Velocity.X < 0) || (CurrentState.Velocity.X > 0 && Opponent->CurrentState.Velocity.X > 0) || 
							(!CurrentState.bFacingRight && CurrentState.Velocity.X < 0 && Opponent->CurrentState.Velocity.X > 0) || (CurrentState.bFacingRight && CurrentState.Velocity.X > 0 && Opponent->CurrentState.Velocity.X < 0)))
					{
						if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
							{
								HitWall();
								CurrentState.Position.X = -1000 + .5f * PushboxWidth;
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								Opponent->CurrentState.Velocity.X = 0;
							}
						}
						else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
							{
								HitWall();
								CurrentState.Position.X = 1000 - .5f * PushboxWidth;
								Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								Opponent->CurrentState.Velocity.X = 0;
							}
						}
					}
					else //if both characters have the same velocity or arent moving
					{
						float MoveDistance = .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth - FMath::Abs(Opponent->CurrentState.Position.X - CurrentState.Position.X));

						if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X -= MoveDistance;
							Opponent->CurrentState.Position.X += MoveDistance;
						}
						else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X += MoveDistance;
							Opponent->CurrentState.Position.X -= MoveDistance;
						}
					}
					CurrentState.bTouchingOpponent = true;
					Opponent->CurrentState.bTouchingOpponent = true;
				}
				else if (!CurrentState.bIsAirborne && !IsCurrentAnimation(KnockDownFaceDown) && !IsCurrentAnimation(KnockDownFaceUp) 
					&& Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.Velocity.Y < 0)
				{
					if ((CurrentState.bIsCrouching && CrouchingPushBoxHeight > Opponent->CurrentState.Position.Y + Opponent->AirPushboxVerticalOffset) ||
						(!CurrentState.bIsCrouching && StandingPushBoxHeight > Opponent->CurrentState.Position.Y + Opponent->AirPushboxVerticalOffset)) //check if pushboxes intersect
					{
						if (CurrentState.bTouchingWall)
						{
							if (CurrentState.bFacingRight)
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (Opponent->CurrentState.bTouchingWall)
						{
							if (CurrentState.bFacingRight)
								CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (Opponent->CurrentState.Position.X < CurrentState.Position.X)
						{
							Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (Opponent->CurrentState.Position.X <= -1000 + .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
						}
						else if (Opponent->CurrentState.Position.X > CurrentState.Position.X)
						{
							Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (Opponent->CurrentState.Position.X >= 1000 - .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = 1000 - .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
						}
						else
						{
							if (Opponent->CurrentState.bFacingRight)
							{
								Opponent->CurrentState.Position.X = CurrentState.Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								CurrentState.Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->CurrentState.Position.X >= 1000 - .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->CurrentState.Position.X = 1000 - .5f * Opponent->PushboxWidth;
									CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									CurrentState.Position.X = -1000 + .5f * PushboxWidth;
									Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}

							}
							else
							{
								Opponent->CurrentState.Position.X = CurrentState.Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								CurrentState.Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->CurrentState.Position.X <= -1000 + .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
									CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
								{
									HitWall();
									CurrentState.Position.X = 1000 - .5f * PushboxWidth;
									Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
						}
						CurrentState.bTouchingOpponent = true;
						Opponent->CurrentState.bTouchingOpponent = true;
					}
				}
				else if (CurrentState.bIsAirborne && !Opponent->CurrentState.bIsAirborne && CurrentState.Velocity.Y < 0
						&& !Opponent->IsCurrentAnimation(KnockDownFaceDown) && !Opponent->IsCurrentAnimation(KnockDownFaceUp))
				{
					if ((Opponent->CurrentState.bIsCrouching && Opponent->CrouchingPushBoxHeight > CurrentState.Position.Y + AirPushboxVerticalOffset) ||
						(!Opponent->CurrentState.bIsCrouching && Opponent->StandingPushBoxHeight > CurrentState.Position.Y + AirPushboxVerticalOffset)) //check if pushboxes intersect
					{
						if (Opponent->CurrentState.bTouchingWall)
						{
							if (Opponent->CurrentState.bFacingRight)
								CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (CurrentState.bTouchingWall)
						{
							if (Opponent->CurrentState.bFacingRight)
								Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									CurrentState.Position.X = -1000 + .5f * PushboxWidth;
									Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
						}
						else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
						{
							CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

							if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
							{
								HitWall();
								CurrentState.Position.X = 1000 - .5f * PushboxWidth;
								Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
						}
						else
						{
							if (CurrentState.bFacingRight)
							{
								CurrentState.Position.X = Opponent->CurrentState.Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								Opponent->CurrentState.Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
								{
									HitWall();
									CurrentState.Position.X = 1000 - .5f * PushboxWidth;
									Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Opponent->CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
									CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}

								}
								else
								{
								CurrentState.Position.X = Opponent->CurrentState.Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								Opponent->CurrentState.Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									CurrentState.Position.X = -1000 + .5f * PushboxWidth;
									Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Opponent->CurrentState.Position.X >= 1000 - .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->CurrentState.Position.X = 1000 - .5f * Opponent->PushboxWidth;
									CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
						}
						CurrentState.bTouchingOpponent = true;
						Opponent->CurrentState.bTouchingOpponent = true;
					}
				}
				else if (CurrentState.bIsAirborne && Opponent->CurrentState.bIsAirborne)// both characters are in the air
				{
					// Check pushbox intersection
					if ((Opponent->CurrentState.Position.Y < CurrentState.Position.Y && (CurrentState.Position.Y + AirPushboxVerticalOffset) - (Opponent->CurrentState.Position.Y + Opponent->AirPushboxVerticalOffset) <= Opponent->CrouchingPushBoxHeight) ||
						(Opponent->CurrentState.Position.Y > CurrentState.Position.Y && (Opponent->CurrentState.Position.Y + Opponent->AirPushboxVerticalOffset) - (CurrentState.Position.Y + AirPushboxVerticalOffset) <= CrouchingPushBoxHeight) ||
						CurrentState.Position.Y + AirPushboxVerticalOffset == Opponent->CurrentState.Position.Y + Opponent->AirPushboxVerticalOffset)
					{
						if (CurrentState.bTouchingWall)
						{
							if (CurrentState.bFacingRight)
								Opponent->CurrentState.Position.X = CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (Opponent->CurrentState.bTouchingWall)
						{
							if (Opponent->CurrentState.bFacingRight)
								CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								CurrentState.Position.X = Opponent->CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (Opponent->CurrentState.Position.X > CurrentState.Position.X)
						{
							float CenterPoint = .5f * (CurrentState.Position.X + Opponent->CurrentState.Position.X);
							Opponent->CurrentState.Position.X = CenterPoint + .5f * (Opponent->PushboxWidth);
							CurrentState.Position.X = CenterPoint - .5f * (PushboxWidth);

							if (Opponent->CurrentState.Position.X >= 1000 - .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = 1000 - .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
							{
								HitWall();
								CurrentState.Position.X = -1000 + .5f * PushboxWidth;
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
						}
						else if (Opponent->CurrentState.Position.X < CurrentState.Position.X)
						{
							float CenterPoint = .5f * (CurrentState.Position.X + Opponent->CurrentState.Position.X);
							Opponent->CurrentState.Position.X = CenterPoint - .5f * (Opponent->PushboxWidth);
							CurrentState.Position.X = CenterPoint + .5f * (PushboxWidth);

							if (Opponent->CurrentState.Position.X <= -1000 + .5f * Opponent->PushboxWidth)
							{
								Opponent->HitWall();
								Opponent->CurrentState.Position.X = -1000 + .5f * Opponent->PushboxWidth;
								CurrentState.Position.X = Opponent->CurrentState.Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
							{
								HitWall();
								CurrentState.Position.X = 1000 - .5f * PushboxWidth;
								Opponent->CurrentState.Position.X = CurrentState.Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
						}
						else
						{
							if (Opponent->CurrentState.Position.Y < CurrentState.Position.Y)
							{
								if (CurrentState.bFacingRight)
									CurrentState.Position.X += .5f * Opponent->PushboxWidth;
								else
									CurrentState.Position.X -= .5f * Opponent->PushboxWidth;
							}
							else if (Opponent->CurrentState.Position.Y > CurrentState.Position.Y)
							{
								if (Opponent->CurrentState.bFacingRight)
									Opponent->CurrentState.Position.X += .5f * PushboxWidth;
								else
									Opponent->CurrentState.Position.X -= .5f * PushboxWidth;
							}
						}
						CurrentState.bTouchingOpponent = true;
						Opponent->CurrentState.bTouchingOpponent = true;
					}
				}
			}
			else
			{
				CurrentState.bTouchingOpponent = false;
				Opponent->CurrentState.bTouchingOpponent = false;
			}
		}
	}
}

void ABTCharacterBase::DrawCharacter()
{
	LightSettings();

	DynamicOutline->SetScalarParameterValue(FName("LineThickness"), CurrentState.LineThickness);
	DynamicOutline->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);

	DynamicEyeShine->SetScalarParameterValue(FName("DepthOffset"), DepthOffset);

	if (CurrentState.StatusTimer > 0)
	{
		DynamicOutline->SetVectorParameterValue("StatusColor", StatusColor);
		DynamicOutline->SetScalarParameterValue("StatusMix", StatusMix);
	}
	else
	{
		DynamicOutline->SetScalarParameterValue("StatusMix", 0);
	}

	if (CurrentState.bFacingRight)
	{
		BaseMesh->SetRelativeScale3D(FVector(1, 1, 1));
	}
	else
	{
		BaseMesh->SetRelativeScale3D(FVector(-1, 1, 1));
	}

	SetActorLocation(FVector(CurrentState.Position.X, GetActorLocation().Y, CurrentState.Position.Y));

	//play sounds if any
	if (CurrentState.bPlaySound)
	{
		if (CharacterVoice->Sound != nullptr)
			CharacterVoice->Play();
		if (CharacterSoundEffects->Sound != nullptr)
			CharacterSoundEffects->Play();

		CurrentState.bPlaySound = false;
	}

	if (Opponent != nullptr)
	{
		if (!CurrentState.CurrentAnimFrame.bSuperFlash && !Opponent->CurrentState.CurrentAnimFrame.bSuperFlash)
		{
			for (ABTProjectileBase* Projectile : Projectiles)
			{
				Projectile->DrawProjectile();
			}

			//draw sigils if they're active
			for (ASigil* Sigil : Sigils)
			{
				Sigil->DrawSigil();
			}
		}
	}

	//draw blitz image effects if active
	if (BlitzImage != nullptr)
	{
		FVector MainLightValue = MainLight->GetComponentLocation() - GetActorLocation();
		BlitzImage->LightVector = MainLightValue;
		BlitzImage->DrawBlitz();
	}

	//if Hitbox View is on also loop through hitbox and hurtbox arrays and draw to screen
	//HitboxViewer();
}

void ABTCharacterBase::ProcessAnimationFrame()
{
	if (CurrentState.PosePlayTime == 0)
	{
		//copy values stored in Animation data to character, these can be overridden by other functions based on character's current circumstances
		CurrentState.bIsCrouching = CurrentState.CurrentAnimFrame.bIsCrouching;
		CurrentState.bArmorActive = CurrentState.CurrentAnimFrame.bArmorActive;
		CurrentState.bCounterHitState = CurrentState.CurrentAnimFrame.bCounterHitState;

		if (CurrentState.LandingLag > 0)
		{
			if (CurrentState.LandingLag > 4)
			{
				CurrentState.bCounterHitState = true;
				CurrentState.bIsCrouching = true;

				if (CurrentState.AvailableActions & AcceptBlitz)
					CurrentState.AvailableActions = AcceptBlitz + AcceptSuper;
				else
					CurrentState.AvailableActions = None;
			}
			else if (CurrentState.LandingLag <= 4) //universal four frames of recovery upon landing on feet that only restricts movement
			{
				CurrentState.AvailableActions &= AcceptAll - (AcceptMove + AcceptJump);
			}
		}
		else
		{
			if (CurrentState.HitStun > 0)
				CurrentState.AvailableActions = None;
			else if (CurrentState.BlockStun > 0)
				CurrentState.AvailableActions = AcceptGuard;
			else if (CurrentState.bAttackMadeContact && CurrentState.AnimFrameIndex > 0)
				CurrentState.AvailableActions |= CurrentState.CurrentAnimFrame.AvailableActions;
			else
				CurrentState.AvailableActions = CurrentState.CurrentAnimFrame.AvailableActions;
		}

		if (CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
		{
			if (CurrentState.CurrentAnimFrame.Hitboxes[0].bNewHit)
			{
				CurrentState.bAttackMadeContact = false;
			}
		}

		//check if any of the frame's variables pertaining to velocity have stored values, add acceleration if any or halt velocity in certain axes if specified
		if (CurrentState.CurrentAnimFrame.Acceleration != FVector2D(0, 0) || CurrentState.CurrentAnimFrame.bStopVelocityY || CurrentState.CurrentAnimFrame.bStopVelocityX)
		{
			if (CurrentState.bFacingRight)
			{
				CurrentState.Velocity = FVector2D((!CurrentState.CurrentAnimFrame.bStopVelocityX * CurrentState.Velocity.X) + CurrentState.CurrentAnimFrame.Acceleration.X,
					(!CurrentState.CurrentAnimFrame.bStopVelocityY * CurrentState.Velocity.Y) + CurrentState.CurrentAnimFrame.Acceleration.Y);
			}
			else
			{
				CurrentState.Velocity = FVector2D((!CurrentState.CurrentAnimFrame.bStopVelocityX * CurrentState.Velocity.X) - CurrentState.CurrentAnimFrame.Acceleration.X,
					(!CurrentState.CurrentAnimFrame.bStopVelocityY * CurrentState.Velocity.Y) + CurrentState.CurrentAnimFrame.Acceleration.Y);
			}
		}
		if (CurrentState.bIsAirborne && CurrentState.CurrentAnimFrame.LandingLag > 0)
			CurrentState.LandingLag = CurrentState.CurrentAnimFrame.LandingLag + 4;

	}
	AnimationEvents(); // perform certain actions based on the current frame of animation
}

bool ABTCharacterBase::EnterNewAnimation(TArray<FAnimationFrame> Animation, int32 FrameNumber)
{
	//check if the specified animation and frame exist
	if (Animation.Num() > FrameNumber)
	{
		CurrentState.PosePlayTime = 0; // reset pose play time to make sure new frame is played for the correct amount of time
		CurrentState.AnimFrameIndex = FrameNumber;
		CurrentState.CurrentAnimation = Animation;
		CurrentState.CurrentAnimFrame = Animation[FrameNumber];
		return true;
	}
	return false;
}

bool ABTCharacterBase::IsCurrentAnimation(TArray<FAnimationFrame> Animation)
{
	if (Animation.Num() != CurrentState.CurrentAnimation.Num())
		return false;

	for (uint8 i = 0; i < Animation.Num(); i++)
	{
		if (Animation[i].Pose)
			if (CurrentState.CurrentAnimation[i].Pose)
				if (Animation[i].Pose != CurrentState.CurrentAnimation[i].Pose)
				{
					return false;
				}
	}
	return true;
}

bool ABTCharacterBase::TurnAroundCheck()
{
	if (Opponent != nullptr)
	{
		if (!CurrentState.bTouchingWall)
		{
			if (Opponent->CurrentState.Position.X < CurrentState.Position.X && CurrentState.bFacingRight)
			{
				CurrentState.bFacingRight = false;
				return TriggerTurnAround();
			}
			else if (Opponent->CurrentState.Position.X > CurrentState.Position.X && !CurrentState.bFacingRight)
			{
				CurrentState.bFacingRight = true;
				return TriggerTurnAround();
			}
		}
	}
	return false;
}

bool ABTCharacterBase::TriggerTurnAround()
{
	//Trigger turnaround animation if in idle stand or crouch;
	if (CurrentState.AvailableActions & AcceptMove && !CurrentState.bIsAirborne)
	{
		if (CurrentState.bIsCrouching)
		{
			return EnterNewAnimation(TurnAroundCrouch);
		}
		else
		{
			return EnterNewAnimation(TurnAroundStand);
		}
	}
	return false;
}

bool ABTCharacterBase::SurfaceContact() //Animation transitions that occur when a character touches a surface with certain conditions
{
	if (((CurrentState.bIsAirborne && CurrentState.Position.Y <= 0 && CurrentState.Velocity.Y <= 0) || (CurrentState.CharacterHitState & CanGroundBounce && !CurrentState.bIsAirborne))) //landing on the ground
	{
		//logic for setting character's state when they hit the ground (land on feet, knockdown, groundbounce)
		if (CurrentState.CharacterHitState & CanGroundBounce || (CurrentState.CharacterHitState & CanAirGroundBounce && CurrentState.bIsAirborne))
		{
			//cause ground bounce and ground bounce animation
			if (CurrentState.CharacterHitState & CanGroundBounce)
				CurrentState.CharacterHitState -= CanGroundBounce;
			if (CurrentState.CharacterHitState & CanAirGroundBounce)
				CurrentState.CharacterHitState -= CanAirGroundBounce;

			DepthOffset = 0;
			Opponent->DepthOffset = 300;

			CurrentState.Velocity.Y *= -1;
			if (CurrentState.Velocity.Y < 3)
				CurrentState.Velocity.Y = 3;

			if (Opponent != nullptr)
			{
				if (Opponent->Sigils.Num() > 1)
				{
					Opponent->Sigils[1]->Activate(CurrentState.Position, FRotator(0, 0, 20));
				}
			}

			return EnterNewAnimation(GroundBounce);
		}
		else if (!IsCurrentAnimation(BackDash))
		{
			CurrentState.JumpsUsed = 0;
			CurrentState.bIsAirborne = false;
			CurrentState.Velocity.Y = 0;
			CurrentState.Position.Y = 0;
			if (CurrentState.CurrentAnimFrame.Invincibility == FaceDown) //cause hard knockdown if in a hitstun state
			{
				CurrentState.SlowMoTime = 0;
				CurrentState.ShatteredTime = 0;
				CurrentState.HitStun = 0;
				return EnterNewAnimation(KnockDownFaceDown);
			}
			else if (CurrentState.CurrentAnimFrame.Invincibility == FaceUp)
			{
				CurrentState.SlowMoTime = 0;
				CurrentState.ShatteredTime = 0;
				CurrentState.HitStun = 0;
				return EnterNewAnimation(KnockDownFaceUp);
			}
			else //if a character is not in a hit state, they will land on their feet
			{
				TurnAroundCheck();

				if (CurrentState.LandingLag > 4 || IsCurrentAnimation(DeflectedAir)) //landing lag puts character in a recovery state when landing
				{
					if (IsCurrentAnimation(DeflectedAir))
						CurrentState.LandingLag = 10;

					if (CurrentState.AvailableActions & AcceptBlitz)
						CurrentState.AvailableActions = AcceptBlitz + AcceptSuper;
					else
						CurrentState.AvailableActions = None;

					return EnterNewAnimation(CrouchDown);
				}
				else if (CurrentState.BlockStun > 0)
				{
					CurrentState.BlockStun += 4; //Add to blockstun when transitioning from air to ground

					return EnterNewAnimation(GuardLo);
				}
				else if (NonKnockdownLanding())
				{
					return true;
				}
			}
		}
	}
	
	//Check if hitting wall, 20 meters is the tentative stage length, subject to change
	if (CurrentState.CharacterHitState & CanMidScreenWallBounce && CurrentState.WallBounceTime == 0 && CurrentState.bIsAirborne)
	{
		CurrentState.CharacterHitState -= CanMidScreenWallBounce;
		DepthOffset = 0;
		Opponent->DepthOffset = 300;
		CurrentState.Velocity.X *= -.5f;
		if (CurrentState.Velocity.Y < 1.5f)
			CurrentState.Velocity.Y = 1.5f;

		if (Opponent != nullptr)
		{
			if (Opponent->Sigils.Num() > 1)
			{
				if (CurrentState.bFacingRight)
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X - .5f * PushboxWidth, CurrentState.Position.Y + .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset), FRotator(-90, 0, 30));
				else
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X + .5f * PushboxWidth, CurrentState.Position.Y + .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset), FRotator(90, 0, 30));
			}
		}

		return EnterNewAnimation(WallBounce);
	}

	if (CurrentState.Position.X <= -1000 + .5f * PushboxWidth)
	{
		CurrentState.Position.X = -1000 + .5f * PushboxWidth;
		return HitWall();
	}
	else if (CurrentState.Position.X >= 1000 - .5f * PushboxWidth)
	{
		CurrentState.Position.X = 1000 - .5f * PushboxWidth;
		return HitWall();
	}

	CurrentState.bTouchingWall = false;
	return false;
}

bool ABTCharacterBase::HitWall()
{
	CurrentState.bTouchingWall = true;
	if (CurrentState.CharacterHitState & CanWallStick && CurrentState.bIsAirborne)
	{
		//cause wall stick
		CurrentState.WallBounceTime = 0;
		uint8 WallStickHitStun = 30;
		
		if (CurrentState.ComboTimer > 840) //14 seconds, special attacks have a minimum of 60% their base hitstun
			WallStickHitStun *= .6f;
		else if (CurrentState.ComboTimer > 600)//10 seconds
			WallStickHitStun *= .7f;
		else if (CurrentState.ComboTimer > 420)//7 seconds
			WallStickHitStun *= .8f;
		else if (CurrentState.ComboTimer > 300)//5 seconds
			WallStickHitStun *= .9f;

		if (CurrentState.HitStun < WallStickHitStun)
		{
			CurrentState.HitStun = WallStickHitStun;
		}

		if (Opponent != nullptr)
		{
			if (Opponent->Sigils.Num() > 1)
			{
				if (CurrentState.bFacingRight)
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X - PushboxWidth - 25), FRotator(-90, 0, 30));
				else
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X + PushboxWidth + 25), FRotator(90, 0, 30));
			}
		}

		CurrentState.CharacterHitState = None;
		CurrentState.Velocity = FVector2D(0, 0);
		return EnterNewAnimation(WallStick);
	}
	else if (CurrentState.CharacterHitState & CanWallBounce && CurrentState.bIsAirborne)
	{
		//cause wall bounce and wall bounce animation
		
		CurrentState.CharacterHitState -= CanWallBounce;
		DepthOffset = 0;
		Opponent->DepthOffset = 300;
		CurrentState.WallBounceTime = 0;
		CurrentState.Velocity.X *= -.5f;
		if (CurrentState.Velocity.Y < 1.5f)
			CurrentState.Velocity.Y = 1.5f;

		if (Opponent != nullptr)
		{
			if (Opponent->Sigils.Num() > 1)
			{
				if (CurrentState.bFacingRight)
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X - .5f * PushboxWidth, CurrentState.Position.Y + .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset), FRotator(-90, 0, 30));
				else
					Opponent->Sigils[1]->Activate(FVector2D(CurrentState.Position.X + .5f * PushboxWidth, CurrentState.Position.Y + .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset), FRotator(90, 0, 30));
			}
		}

		return EnterNewAnimation(WallBounce);
	}

	CurrentState.Velocity.X = 0;
	return false;
}

void ABTCharacterBase::Guarding()
{
	if (CurrentState.AvailableActions & AcceptGuard)
	{
		RefreshMovelist();
		if (!CurrentState.bIsAirborne && (CurrentState.Dir1 == InputTime || CurrentState.Dir2 == InputTime || CurrentState.Dir3 == InputTime))
		{
			//on the ground and holding a downward direction means the character is crouching
			CurrentState.bIsCrouching = true;
		}

		if (CurrentState.bIsGuarding) //(Dir7 == InputTime || Dir4 >= InputTime || Dir1 == InputTime)
		{
			//holding a backward direction while able to guard keeps the character's guard up
			//bIsGuarding = true;
			CurrentState.JustDefense--; //if a character inputs guard up to five frames before an attack lands, blockstun is reduced by a third and no knockback/pushback is applied
		}
		else
		{
			//bIsGuarding = false;
			CurrentState.JustDefense = 5;
		}
	}
	else
	{
		CurrentState.bIsGuarding = false;
		CurrentState.JustDefense = 5;
	}
}

void ABTCharacterBase::LandingLagCheck()
{
	if (CurrentState.LandingLag > 0) //landing lag system
	{
		if (CurrentState.LandingLag > 4)
		{
			CurrentState.bCounterHitState = true;
			CurrentState.bIsCrouching = true;

			if (CurrentState.AvailableActions & AcceptBlitz)
				CurrentState.AvailableActions = AcceptBlitz + AcceptSuper;
			else
				CurrentState.AvailableActions = None;
		}
		else if (CurrentState.LandingLag <= 4)
		{
			CurrentState.AvailableActions &= AcceptAll - AcceptMove - AcceptJump;
		}

		if (!CurrentState.bIsAirborne)
		{
			CurrentState.LandingLag--;
		}
	}
}

void ABTCharacterBase::RunBraking()
{
	if (CurrentState.bIsRunning || IsCurrentAnimation(Brake)) //accelerate while running
	{
		float AccelScale = 1;
		if (CurrentState.bIsRunning && !Opponent->CurrentState.bIsRunning && CurrentState.bTouchingOpponent) //opponent provides resistance when dashing against them
			AccelScale *= .8f;
		if (CurrentState.SlowMoTime > 0)
			AccelScale *= .5f;

		if (CurrentState.bIsRunning)
		{
			if (CurrentState.bFacingRight)
			{
				if (CurrentState.Velocity.X < InitRunSpeed)
					CurrentState.Velocity.X = InitRunSpeed;
				else
					CurrentState.Velocity.X += AccelScale * RunAcceleration;
			}
			else
			{
				if (CurrentState.Velocity.X > -InitRunSpeed)
					CurrentState.Velocity.X = -InitRunSpeed;
				else
					CurrentState.Velocity.X -= AccelScale * RunAcceleration;
			}

			//stop running if forward direction is no longer being held for run type characters and if animation has finished on dash type characters
			if ((RunAcceleration > 0 && CurrentState.Dir6 < InputTime && CurrentState.Dir4 < InputTime && CurrentState.Dir9 < InputTime) || (RunAcceleration == 0 && CurrentState.AvailableActions & AcceptMove))
			{
				CurrentState.bIsRunning = false;
			}
		}
	}
	
	if (!CurrentState.bIsRunning && !CurrentState.bIsAirborne) //braking/friction to slow down character when not voluntarily accelerating
	{
		if (FMath::Abs(CurrentState.Velocity.X) > 1.f)
			CurrentState.Velocity.X *= .95f; //gradually come to stop
		else
		{
			CurrentState.Velocity.X = 0;
		}
	}
}

void ABTCharacterBase::Jumping()
{
	RefreshMovelist();
	DepthOffset = 0;
	Opponent->DepthOffset = 300;

	if (CurrentState.bIsRunning) //extra horizontal velocity only if jumping with a running start
	{
		CurrentState.Velocity.X *= .5f;

		if (CurrentState.bTouchingOpponent)
			CurrentState.Velocity.X *= .85f;
	}
	else
		CurrentState.Velocity.X = 0;

	if (CurrentState.bIsAirborne && CurrentState.JumpsUsed == 0)
		CurrentState.JumpsUsed++;

	CurrentState.Velocity.Y = JumpForce.Y;

	if (CurrentState.bIsAirborne)
	{
		//CurrentState.Velocity.Y *= .95f;

		if (Sigils.Num() > 0)
		{
			FVector2D SigilLocation = FVector2D(CurrentState.Position.X, CurrentState.Position.Y + 25);
			FRotator SigilRotation = FRotator(0, 0, 20);
			if ((CurrentState.bFacingRight && CurrentState.bForwardJump) || (!CurrentState.bFacingRight && CurrentState.bBackwardJump))
			{
				SigilLocation.X -= 15;
				SigilLocation.Y += 10;
				SigilRotation.Pitch = -15;
			}
			else if ((!CurrentState.bFacingRight && CurrentState.bForwardJump) || (CurrentState.bFacingRight && CurrentState.bBackwardJump))
			{
				SigilLocation.X += 15;
				SigilLocation.Y += 10;
				SigilRotation.Pitch = 15;
			}
			Sigils[0]->Activate(SigilLocation, SigilRotation);
		}
	}

	if (CurrentState.bForwardJump) //if jumping forward
	{
		if (CurrentState.bFacingRight)
			CurrentState.Velocity.X += JumpForce.X;
		else
			CurrentState.Velocity.X -= JumpForce.X;
	}
	else if (CurrentState.bBackwardJump) //if jumping back
	{
		if (CurrentState.bFacingRight)
			CurrentState.Velocity.X -= JumpForce.X;
		else
			CurrentState.Velocity.X += JumpForce.X;
	}

	CurrentState.bIsAirborne = true;
	CurrentState.bIsRunning = false;
	CurrentState.bForwardJump = false;
	CurrentState.bBackwardJump = false;
	CurrentState.JumpsUsed++;
	TurnAroundCheck();
}

void ABTCharacterBase::GravityCalculation()
{
	if (CurrentState.bIsAirborne && CurrentState.GravDefyTime == 0 && !CurrentState.CurrentAnimFrame.bLockPosition) //apply gravity while character is airborne and not defying gravity
	{
		float GravCalc = Weight * -12.f / 60.f;

		if (IsCurrentAnimation(WallStick) || CurrentState.ShatteredTime > 0)
			GravCalc *= .75f;
		if (CurrentState.SlowMoTime > 0)
			GravCalc *= .5f;

		CurrentState.Velocity.Y += GravCalc;
	}
	if (!CurrentState.bIsAirborne && CurrentState.Position.Y > 0)
		CurrentState.bIsAirborne = true;
	
}

void ABTCharacterBase::ApplyKnockBack()
{
	float ComboGravity = 1;
	if (Opponent != nullptr)
	{
		if (Opponent->CurrentState.ComboCount <= 8 || CurrentState.CharacterHitState & IsSuper || CurrentState.ShatteredTime > 0)
			ComboGravity = 1;
		else if (Opponent->CurrentState.ComboCount <= 12)
			ComboGravity = .98;
		else if (Opponent->CurrentState.ComboCount <= 16)
			ComboGravity = .95;
		else if (Opponent->CurrentState.ComboCount <= 20)
			ComboGravity = .93;
		else if (Opponent->CurrentState.ComboCount <= 24)
			ComboGravity = .9;
		else if (Opponent->CurrentState.ComboCount <= 30)
			ComboGravity = .87;
		else
			ComboGravity = .85f;
	}

	if (CurrentState.KnockBack != FVector2D(0, 0)) //apply any knockback
	{
		if (CurrentState.CurrentAnimFrame.Invincibility != OTG)
		{
			if (CurrentState.HitStun > 0 || CurrentState.BlockStun > 0 || IsCurrentAnimation(Deflected) || IsCurrentAnimation(DeflectedAir))
			{
				if (CurrentState.KnockBack.Y > 0)
					CurrentState.Velocity = FVector2D(CurrentState.KnockBack.X, ComboGravity * CurrentState.KnockBack.Y);
				else
					CurrentState.Velocity = CurrentState.KnockBack;
			}
			else
			{
				CurrentState.Velocity.X = CurrentState.KnockBack.X;
				if (CurrentState.KnockBack.Y > 0)
					CurrentState.Velocity.Y = CurrentState.KnockBack.Y;
			}

			if (CurrentState.bFacingRight)
			{
				CurrentState.Velocity *= FVector2D(-1, 1);
			}
		}

		CurrentState.KnockBack = FVector2D(0, 0);
	}
}

void ABTCharacterBase::UpdateResolve()
{
	if (Opponent != nullptr)
	{
		if (CurrentState.ShatteredTime == 0 && !CurrentState.CurrentAnimFrame.bCinematic && !CurrentState.CurrentAnimFrame.bSuperFlash &&
			!Opponent->CurrentState.CurrentAnimFrame.bCinematic && !Opponent->CurrentState.CurrentAnimFrame.bSuperFlash && !CurrentState.bWin && !Opponent->CurrentState.bWin)
		{
			if (CurrentState.ResolveRecoverTimer >= 180 && CurrentState.Durability <= 100) //Resolve starts passively regenerating after three seconds without being used or broken, resolve regen speeds up if the character is not inputting anything
			{
				if ((float)CurrentState.Health / (float)MaxHealth <= .1f && (CurrentState.SlowMoTime == 0 || (CurrentState.SlowMoTime > 0 && CurrentState.RecoverInterval >= 2)))
				{
					CurrentState.Durability++;
					if (CurrentState.Resolute || CurrentState.bIsRunning)
						CurrentState.Durability++;
					CurrentState.RecoverInterval = 0;
				}
				else if ((float)CurrentState.Health / (float)MaxHealth <= .25f && ((CurrentState.SlowMoTime == 0 && CurrentState.RecoverInterval >= 2) || (CurrentState.SlowMoTime > 0 && CurrentState.RecoverInterval >= 4) || (CurrentState.SlowMoTime == 0 && (CurrentState.Resolute))))
				{
					CurrentState.Durability++;
					CurrentState.RecoverInterval = 0;
				}
				else if ((float)CurrentState.Health / (float)MaxHealth <= .5f && ((CurrentState.SlowMoTime == 0 && CurrentState.RecoverInterval >= 3) || (CurrentState.SlowMoTime > 0 && CurrentState.RecoverInterval >= 6) || (CurrentState.SlowMoTime == 0 && (CurrentState.Resolute) && CurrentState.RecoverInterval >= 2)))
				{
					CurrentState.Durability++;
					CurrentState.RecoverInterval = 0;
				}
				else if (((CurrentState.SlowMoTime == 0 && CurrentState.RecoverInterval >= 4) || (CurrentState.SlowMoTime > 0 && CurrentState.RecoverInterval >= 8) || (CurrentState.SlowMoTime == 0 && (CurrentState.Resolute) && CurrentState.RecoverInterval >= 3)))
				{
					CurrentState.Durability++;
					CurrentState.RecoverInterval = 0;
				}
				else
					CurrentState.RecoverInterval++;
			}
			else if (CurrentState.ResolveRecoverTimer < 180)
				CurrentState.ResolveRecoverTimer++;
		}
	}

	if (CurrentState.Durability > 100)
	{
		if (CurrentState.Resolve < 4)
		{
			CurrentState.Resolve++;
			CurrentState.Durability = 0;
		}
		else
		{
			CurrentState.Resolve = 4;
			CurrentState.Durability = 100;
		}
	}
	else if (CurrentState.Durability < 0)
	{
		if (CurrentState.Resolve > 0)
		{
			CurrentState.Resolve--;
			CurrentState.Durability = 100;
		}
	}
}

void ABTCharacterBase::ProcessInputs(int32 Inputs)
{
	ChargeInputs(Inputs);
	DirectionalInputs(Inputs);
	ButtonInputs(Inputs);
}

void ABTCharacterBase::ChargeInputs(int32 Inputs)  //set the correct charges based on the inputs read
{
	if (Inputs & INPUT_UP) //up charge will override down charge
	{
		CurrentState.Charge8++;
		CurrentState.Charge8Life = InputTime;
	}
	else if (Inputs & INPUT_DOWN)
	{
		CurrentState.Charge2++;
		CurrentState.Charge2Life = InputTime;
	}

	if ((Inputs & INPUT_LEFT && CurrentState.Position.X > Opponent->CurrentState.Position.X) || (Inputs & INPUT_RIGHT && CurrentState.Position.X < Opponent->CurrentState.Position.X))
	{
		CurrentState.Charge6++;
		CurrentState.Charge6Life = InputTime;
	}
	else if ((Inputs & INPUT_LEFT && CurrentState.Position.X < Opponent->CurrentState.Position.X) || (Inputs & INPUT_RIGHT && CurrentState.Position.X > Opponent->CurrentState.Position.X)) //prevent charging both directions at the same time
	{
		CurrentState.Charge4++;
		CurrentState.Charge4Life = InputTime;
	}
	
	if (CurrentState.Charge4Life == 0)
		CurrentState.Charge4 = 0;

	if (CurrentState.Charge6Life == 0)
		CurrentState.Charge6 = 0;

	if (CurrentState.Charge2Life == 0)
		CurrentState.Charge2 = 0;

	if (CurrentState.Charge8Life == 0)
		CurrentState.Charge8 = 0;
}

void ABTCharacterBase::DirectionalInputs(int32 Inputs) //set the correct directional inputs based on the inputs read
{
	CurrentState.bIsGuarding = false;
	if (Inputs & INPUT_DOWN)
	{
		if ((CurrentState.Position.X < Opponent->CurrentState.Position.X && Inputs & INPUT_RIGHT) || (CurrentState.Position.X > Opponent->CurrentState.Position.X && Inputs & INPUT_LEFT))
			CurrentState.Dir3 = InputTime;
		else if ((CurrentState.Position.X > Opponent->CurrentState.Position.X && Inputs & INPUT_RIGHT) || (CurrentState.Position.X < Opponent->CurrentState.Position.X && Inputs & INPUT_LEFT))
			CurrentState.Dir1 = InputTime;
		else
		{
			if (CurrentState.Dir2 < InputTime - 1 && CurrentState.Dir2 > 3 && CurrentState.DoubleDir2 == 0)
			{
				CurrentState.DoubleDir2 = InputTime;
			}
			CurrentState.Dir2 = InputTime;
		}

		if (Opponent != nullptr)
		{
			if (Opponent->CurrentState.Position.X < CurrentState.Position.X && Inputs & INPUT_RIGHT && !(Inputs & INPUT_LEFT))
				CurrentState.bIsGuarding = true;
			else if (!CurrentState.bFacingRight && Inputs & INPUT_RIGHT && !(Inputs & INPUT_LEFT))
				CurrentState.bIsGuarding = true;
			if (Opponent->CurrentState.Position.X > CurrentState.Position.X && Inputs & INPUT_LEFT && !(Inputs & INPUT_RIGHT))
				CurrentState.bIsGuarding = true;
			else if (CurrentState.bFacingRight && Inputs & INPUT_LEFT && !(Inputs & INPUT_RIGHT))
				CurrentState.bIsGuarding = true;
		}
	}
	else if (Inputs & INPUT_UP)
	{
		if (CurrentState.Dir7 < InputTime - 1 && CurrentState.Dir8 < InputTime - 1 && CurrentState.Dir9 < InputTime - 1 && CurrentState.bIsAirborne && !IsCurrentAnimation(FocusBlitz))
			CurrentState.AirJump = InputTime;

		if ((CurrentState.Position.X < Opponent->CurrentState.Position.X && Inputs & INPUT_RIGHT) || (CurrentState.Position.X > Opponent->CurrentState.Position.X && Inputs & INPUT_LEFT))
		{
			CurrentState.Dir9 = InputTime;
		}
		else if ((CurrentState.Position.X > Opponent->CurrentState.Position.X && Inputs & INPUT_RIGHT) || (CurrentState.Position.X < Opponent->CurrentState.Position.X && Inputs & INPUT_LEFT))
		{
			CurrentState.Dir7 = InputTime;
		}
		else
		{
			CurrentState.Dir8 = InputTime;
		}
	}
	else if ((Inputs & INPUT_RIGHT && !(Inputs & INPUT_LEFT)) || (Inputs & INPUT_LEFT && !(Inputs & INPUT_RIGHT)))
	{
		if (Inputs & INPUT_RIGHT)
		{
			if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
			{
				if (((CurrentState.Dir6 < InputTime - 1 && CurrentState.Dir6 > 0) || (CurrentState.Dir9 < InputTime - 1 && CurrentState.Dir9 > 0)) && CurrentState.DoubleDir4 == 0)
				{
					CurrentState.DoubleDir6 = InputTime;
				}
				CurrentState.Dir6 = InputTime;
			}
			else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
			{
				if (((CurrentState.Dir4 < InputTime - 1 && CurrentState.Dir4 > 0) || (CurrentState.Dir7 < InputTime - 1 && CurrentState.Dir7 > 0)) && CurrentState.DoubleDir4 == 0)
				{
					CurrentState.DoubleDir4 = InputTime;
				}
				CurrentState.Dir4 = InputTime;
			}

			if (Opponent != nullptr)
			{
				if (Opponent->CurrentState.Position.X < CurrentState.Position.X && Inputs & INPUT_RIGHT)
					CurrentState.bIsGuarding = true;
				else if (!CurrentState.bFacingRight && Inputs & INPUT_RIGHT)
					CurrentState.bIsGuarding = true;
			}
		}
		else
		{
			if (CurrentState.Position.X < Opponent->CurrentState.Position.X)
			{
				if (((CurrentState.Dir4 < InputTime - 1 && CurrentState.Dir4 > 0) || (CurrentState.Dir7 < InputTime - 1 && CurrentState.Dir7 > 0)) && CurrentState.DoubleDir4 == 0)
				{
					CurrentState.DoubleDir4 = InputTime;
				}
				CurrentState.Dir4 = InputTime;
			}
			else if (CurrentState.Position.X > Opponent->CurrentState.Position.X)
			{
				if (((CurrentState.Dir6 < InputTime - 1 && CurrentState.Dir6 > 0) || (CurrentState.Dir9 < InputTime - 1 && CurrentState.Dir9 > 0)) && CurrentState.DoubleDir4 == 0)
				{
					CurrentState.DoubleDir6 = InputTime;
				}
				CurrentState.Dir6 = InputTime;
			}

			if (Opponent != nullptr)
			{
				if (Opponent->CurrentState.Position.X > CurrentState.Position.X && Inputs & INPUT_LEFT)
					CurrentState.bIsGuarding = true;
				else if (CurrentState.bFacingRight && Inputs & INPUT_LEFT)
					CurrentState.bIsGuarding = true;
			}
		}
	}
}

void ABTCharacterBase::ButtonInputs(int32 Inputs) //set the correct button inputs based on the inputs read
{
	if (Inputs & INPUT_LIGHT && !CurrentState.bIsLDown) //Light Attack Button
	{
		CurrentState.LPressed = InputTime;
		CurrentState.bIsLDown = true;
	}
	else if (!(Inputs & INPUT_LIGHT) && CurrentState.bIsLDown)
	{
		CurrentState.LReleased = InputTime;
		CurrentState.bIsLDown = false;
	}

	if (Inputs & INPUT_MEDIUM && !CurrentState.bIsMDown) //Medium Attack Button
	{
		CurrentState.MPressed = InputTime;
		CurrentState.bIsMDown = true;
	}
	else if (!(Inputs & INPUT_MEDIUM) && CurrentState.bIsMDown)
	{
		CurrentState.MReleased = InputTime;
		CurrentState.bIsMDown = false;
	}

	if (Inputs & INPUT_HEAVY && !CurrentState.bIsHDown) //Heavy Attack Button
	{
		CurrentState.HPressed = InputTime;
		CurrentState.bIsHDown = true;
	}
	else if (!(Inputs & INPUT_HEAVY) && CurrentState.bIsHDown)
	{
		CurrentState.HReleased = InputTime;
		CurrentState.bIsHDown = false;
	}

	if (Inputs & INPUT_BREAK && !CurrentState.bIsBDown) //Break Attack Button
	{
		CurrentState.BPressed = InputTime;
		CurrentState.bIsBDown = true;
	}
	else if (!(Inputs & INPUT_BREAK) && CurrentState.bIsBDown)
	{
		CurrentState.BReleased = InputTime;
		CurrentState.bIsBDown = false;
	}
}

void ABTCharacterBase::UpdateInputHistory(int32 Inputs, int32 FrameNumber) //Called to add current inputs to input history
{
	//UE_LOG(LogTemp, Warning, TEXT("Frame %d: %d."), FrameNumber, Inputs);
	if (InputHistory.Num() < FrameNumber)
		InputHistory.Add(Inputs);
	else
		InputHistory[FMath::Max(0, FrameNumber - 1)] = Inputs;
}

void ABTCharacterBase::InputCountdown() //decrement input values
{
	if (CurrentState.Charge2Life > 0)
		CurrentState.Charge2Life--;
	if (CurrentState.Charge4Life > 0)
		CurrentState.Charge4Life--;
	if (CurrentState.Charge6Life > 0)
		CurrentState.Charge6Life--;
	if (CurrentState.Charge8Life > 0)
		CurrentState.Charge8Life--;

	if (CurrentState.Dir1 > 0)
		CurrentState.Dir1--;
	if (CurrentState.Dir2 > 0)
		CurrentState.Dir2--;
	if (CurrentState.Dir3 > 0)
		CurrentState.Dir3--;
	if (CurrentState.Dir4 > 0)
		CurrentState.Dir4--;
	if (CurrentState.Dir6 > 0)
		CurrentState.Dir6--;
	if (CurrentState.Dir7 > 0)
		CurrentState.Dir7--;
	if (CurrentState.Dir8 > 0)
		CurrentState.Dir8--;
	if (CurrentState.Dir9 > 0)
		CurrentState.Dir9--;

	if (CurrentState.DoubleDir2 > 0)
		CurrentState.DoubleDir2--;
	if (CurrentState.DoubleDir4 > 0)
		CurrentState.DoubleDir4--;
	if (CurrentState.DoubleDir6 > 0)
		CurrentState.DoubleDir6--;

	if (CurrentState.LPressed > 0)
		CurrentState.LPressed--;
	if (CurrentState.MPressed > 0)
		CurrentState.MPressed--;
	if (CurrentState.HPressed > 0)
		CurrentState.HPressed--;
	if (CurrentState.BPressed > 0)
		CurrentState.BPressed--;

	if (CurrentState.LReleased > 0)
		CurrentState.LReleased--;
	if (CurrentState.MReleased > 0)
		CurrentState.MReleased--;
	if (CurrentState.HReleased > 0)
		CurrentState.HReleased--;
	if (CurrentState.BReleased > 0)
		CurrentState.BReleased--;
}

void ABTCharacterBase::AnimationStateMachine()
{
	if (!SurfaceContact())
	{
		if (!ActiveTransitions())
		{
			if (!ConditionalTransitions())
				if (PassiveTransitions())
					return;
		}	
	}
}

bool ABTCharacterBase::NonKnockdownLanding()
{
	if (IsCurrentAnimation(WallStick))
		return EnterNewAnimation(Crumple, 2);

	CurrentState.LandingLag = 4;
	CurrentState.Velocity.X *= .75f;
	CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptJump);

	return EnterNewAnimation(StandUp);
}

bool ABTCharacterBase::ActiveTransitions() //Transitions controlled by player input and character state
{
	//Attack transitions supersede all others

	if (CurrentState.bIsAirborne && (CurrentState.CurrentAnimFrame.Invincibility == FaceDown || CurrentState.CurrentAnimFrame.Invincibility == FaceUp || IsCurrentAnimation(WallStick)) 
		&& !IsCurrentAnimation(WallBounce) && !IsCurrentAnimation(GroundBounce) && CurrentState.HitStun == 0 && CurrentState.ShatteredTime == 0)
	{
		if (CurrentState.bIsLDown || CurrentState.bIsMDown || CurrentState.bIsHDown || CurrentState.bIsBDown) //hold any attack button down to air recover once able
		{
			if (CurrentState.Dir6 == InputTime) //can hold a direction as well to move in that direction while recovering
			{
				if (CurrentState.bFacingRight)
				{
					if (CurrentState.Velocity.X >= 0)
						CurrentState.Velocity.X = 1.5;
					else
						CurrentState.Velocity.X = .5f;
				}
				else
				{
					if (CurrentState.Velocity.X <= 0)
						CurrentState.Velocity.X = -1.5;
					else
						CurrentState.Velocity.X = -.5f;
				}
			}
			else if (CurrentState.Dir4 == InputTime)
			{
				if (CurrentState.bFacingRight)
				{
					if (CurrentState.Velocity.X <= 0)
						CurrentState.Velocity.X = -1.5;
					else
						CurrentState.Velocity.X = -.5f;
				}
				else
				{
					if (CurrentState.Velocity.X >= 0)
						CurrentState.Velocity.X = 1.5;
					else
						CurrentState.Velocity.X = .5f;
				}
			}
			CurrentState.Velocity.Y = 1.5f;
			//make flash white and play chime
			StatusMix = .5f;
			StatusColor = FVector(1);
			CurrentState.SlowMoTime = 0;
			CurrentState.StatusTimer = 10;
			CurrentState.JumpsUsed = 0;
			CurrentState.WallBounceTime = 0;
			return EnterNewAnimation(AirRecovery);
		}
	}

	if (IsCurrentAnimation(Stagger) && CurrentState.HitStun == 0)
	{
		if (CurrentState.LPressed > 0 || CurrentState.MPressed > 0 || CurrentState.HPressed > 0 || CurrentState.BPressed > 0) //press any attack button once hitstun has ended to recover from stagger
		{
			//make flash white and play chime
			StatusMix = .5f;
			StatusColor = FVector(1);
			CurrentState.StatusTimer = 10;
			CurrentState.LPressed = 0;
			CurrentState.MPressed = 0;
			CurrentState.HPressed = 0;
			CurrentState.BPressed = 0;
			return EnterNewAnimation(IdleCrouch);
		}
	}

	if (Opponent)
	{
		if ((CurrentState.AvailableActions & AcceptGuard) && CurrentState.bIsGuarding && !CurrentState.bIsAirborne && FMath::Abs(CurrentState.Position.X - Opponent->CurrentState.Position.X) <= Opponent->CurrentState.CurrentAnimFrame.AutoGuardProximity 
			&& CurrentState.BlockStun == 0 && CurrentState.HitStun == 0)
		{
			if (CurrentState.bIsCrouching && !IsCurrentAnimation(GuardLoIn) && !IsCurrentAnimation(GuardLo))
				return EnterNewAnimation(GuardLoIn);
			else if (!CurrentState.bIsCrouching && !IsCurrentAnimation(GuardHiIn) && !IsCurrentAnimation(GuardHi))
				return EnterNewAnimation(GuardHiIn);
		}
	}

	if (CurrentState.AvailableActions & AcceptJump && ((!CurrentState.bIsAirborne && (CurrentState.Dir7 == InputTime || CurrentState.Dir8 == InputTime || CurrentState.Dir9 == InputTime)) || 
		CurrentState.AirJump > 0) && CurrentState.JumpsUsed < MaxJumps)
	{
		CurrentState.AirJump = 0;

		if (CurrentState.Dir9 > 0 && CurrentState.Dir9 > CurrentState.Dir7 && CurrentState.Dir9 > CurrentState.Dir8) //Most recent input is forward jump
			CurrentState.bForwardJump = true;
		else if (CurrentState.Dir7 > 0 && CurrentState.Dir7 > CurrentState.Dir9 && CurrentState.Dir7 > CurrentState.Dir8) //Most recent input is backward jump
				CurrentState.bBackwardJump = true;

		if (!CurrentState.bIsAirborne)
		{
			return EnterNewAnimation(PreJump); //ground jump has prejump frames
		}
		else // Make sure corresponding animations exist, otherwise default to neutral jump animation
		{
			if (CurrentState.bForwardJump && ForwardJump.Num() > 0)
				return EnterNewAnimation(ForwardJump);
			else if (CurrentState.bBackwardJump && BackwardJump.Num() > 0)
				return EnterNewAnimation(BackwardJump);
			else
				return EnterNewAnimation(NeutralJump);
		}
	}

	if (CurrentState.AvailableActions & AcceptMove && !CurrentState.bIsAirborne)
	{
		/*if (Taunt > 0)
			return EnterNewAnimation(Taunt); */
		if (CurrentState.bLose)
			return EnterNewAnimation(TimeOverLose);

		if (CurrentState.DoubleDir6 > 0)
		{
			CurrentState.bIsRunning = true;
			CurrentState.DoubleDir6 = 0;
			if (RunStart.Num() > 0)
				return EnterNewAnimation(RunStart);
			else
				return EnterNewAnimation(RunCycle);
		}

		if ((IsCurrentAnimation(WalkBackward) && CurrentState.Dir4 < InputTime) || (IsCurrentAnimation(WalkForward) && CurrentState.Dir6 < InputTime))
			return EnterNewAnimation(IdleStand);

		if (CurrentState.bIsCrouching && !IsCurrentAnimation(TurnAroundCrouch) && !IsCurrentAnimation(CrouchDown) && !IsCurrentAnimation(IdleCrouch) && !IsCurrentAnimation(IdleCrouchBlink) && !IsCurrentAnimation(CrouchIdleAction)
			&& !IsCurrentAnimation(GuardLoIn) && !IsCurrentAnimation(GuardLo) && !IsCurrentAnimation(GuardLoOut) && !IsCurrentAnimation(HitCOut) && !IsCurrentAnimation(HitCHeavyOut))
			return EnterNewAnimation(CrouchDown);

		if (!CurrentState.bIsCrouching && (IsCurrentAnimation(IdleCrouch) || IsCurrentAnimation(CrouchDown) || IsCurrentAnimation(IdleCrouchBlink) || IsCurrentAnimation(CrouchIdleAction)))
			return EnterNewAnimation(StandUp);

		if (CurrentState.Dir6 == InputTime && !IsCurrentAnimation(WalkForward))
			return EnterNewAnimation(WalkForward);

		if (CurrentState.Dir4 == InputTime && !IsCurrentAnimation(WalkBackward))
			return EnterNewAnimation(WalkBackward);
	}

	if (CurrentState.DoubleDir4 > 0 && (!CurrentState.bIsAirborne && CurrentState.AvailableActions & AcceptGuard && CurrentState.BlockStun == 0))
	{
		CurrentState.DoubleDir4 = 0;
		return EnterNewAnimation(BackDash);
	}

	return false;
}

bool ABTCharacterBase::ConditionalTransitions()
{
	if (CurrentState.AvailableActions & AcceptMove && !CurrentState.bIsAirborne)
	{
		CurrentState.JumpsUsed = 0;
		RefreshMovelist();
		if (TurnAroundCheck())
			return true;
	}

	if (IsCurrentAnimation(AirDashForward) && CurrentState.GravDefyTime == 0)
		return EnterNewAnimation(AirDashForwardOut);
	if (IsCurrentAnimation(AirDashBackward) && CurrentState.GravDefyTime == 0)
		return EnterNewAnimation(AirDashBackwardOut);
	if (IsCurrentAnimation(MidJump) && CurrentState.Velocity.Y < 1.5f)
		return EnterNewAnimation(JumpTransition);
	if (IsCurrentAnimation(LaunchCycle) && CurrentState.Velocity.Y < 2)
		return EnterNewAnimation(LaunchTransition);
	if ((IsCurrentAnimation(RunStart) || IsCurrentAnimation(RunCycle)) && !CurrentState.bIsRunning)
		return EnterNewAnimation(Brake);
	if ((IsCurrentAnimation(GuardHi) || IsCurrentAnimation(GuardHiHeavy) || IsCurrentAnimation(GuardHiVertical) || IsCurrentAnimation(GuardHiIn)) && CurrentState.BlockStun == 0 &&
		(FMath::Abs(CurrentState.Position.X - Opponent->CurrentState.Position.X) > Opponent->CurrentState.CurrentAnimFrame.AutoGuardProximity || !CurrentState.bIsGuarding))
		return EnterNewAnimation(GuardHiOut);
	if ((IsCurrentAnimation(GuardLo) || IsCurrentAnimation(GuardLoHeavy) || IsCurrentAnimation(GuardLoIn)) && CurrentState.BlockStun == 0 &&
		(FMath::Abs(CurrentState.Position.X - Opponent->CurrentState.Position.X) > Opponent->CurrentState.CurrentAnimFrame.AutoGuardProximity || !CurrentState.bIsGuarding))
		return EnterNewAnimation(GuardLoOut);
	if (IsCurrentAnimation(GuardAir) && CurrentState.BlockStun == 0)
		return EnterNewAnimation(GuardAirOut);

	if (IsCurrentAnimation(HitSLIn) && CurrentState.HitStun <= 4)
		return EnterNewAnimation(HitSLOut);
	if (IsCurrentAnimation(HitSLHeavyIn) && CurrentState.HitStun <= 6)
		return EnterNewAnimation(HitSLHeavyOut);
	if (IsCurrentAnimation(HitSHIn) && CurrentState.HitStun <= 4)
		return EnterNewAnimation(HitSHOut);
	if (IsCurrentAnimation(HitSHHeavyIn) && CurrentState.HitStun <= 6)
		return EnterNewAnimation(HitSHHeavyOut);
	if (IsCurrentAnimation(HitCIn) && CurrentState.HitStun <= 4)
		return EnterNewAnimation(HitCOut);
	if (IsCurrentAnimation(HitCHeavyIn) && CurrentState.HitStun <= 6)
		return EnterNewAnimation(HitCHeavyOut);

	return false;
}

bool ABTCharacterBase::PassiveTransitions()
{
	//Animation transitions triggered by having finished the current animation
	if (CurrentState.AnimFrameIndex == CurrentState.CurrentAnimation.Num() - 1 && CurrentState.PosePlayTime >= CurrentState.CurrentAnimFrame.PlayDuration) // When hitting the end of an animation
	{
		if (ExitTimeTransitions()) //certain animations need to transition to other animations upon finishing
			return true;
		else if (CurrentState.CurrentAnimFrame.bDoesCycle) //if the anim cycles and are currently at the end of the animation, play it again
		{
			if (IsCurrentAnimation(IdleStand) || IsCurrentAnimation(IdleCrouch))
				CurrentState.IdleCycle++;

			if (CurrentState.IdleCycle == 2)
			{
				CurrentState.IdleCycle = 0;
				if (IsCurrentAnimation(IdleStand))
					return EnterNewAnimation(IdleStandBlink);
				else if (IsCurrentAnimation(IdleCrouch))
					return EnterNewAnimation(IdleCrouchBlink);
			}
			/*else if (CurrentState.IdleCycle == 4)
			{
				CurrentState.IdleCycle = 0;
				if (CurrentAnimation == IdleStand)
					return EnterNewAnimation(StandIdleAction);
				else
					return EnterNewAnimation(CrouchIdleAction);
			}*/
			return EnterNewAnimation(CurrentState.CurrentAnimation);
		}
	}
	else if (CurrentState.PosePlayTime >= CurrentState.CurrentAnimFrame.PlayDuration && CurrentState.AnimFrameIndex < CurrentState.CurrentAnimation.Num() - 1) //Move to next frame of current animation
	{
		CurrentState.CurrentAnimFrame = CurrentState.CurrentAnimation[++CurrentState.AnimFrameIndex];
		CurrentState.PosePlayTime = 0;
	}
	return false;
}

bool ABTCharacterBase::ExitTimeTransitions()
{
	if (IsCurrentAnimation(TimeOverLose))
	{
		return EnterNewAnimation(LoseCycle);
	}

	if (IsCurrentAnimation(PreJump)) // Transition from PreJump frames to JumpAnimation
	{
		if (CurrentState.bForwardJump && ForwardJump.Num() > 0)
			return EnterNewAnimation(ForwardJump);
		else if (CurrentState.bBackwardJump && BackwardJump.Num() > 0)
			return EnterNewAnimation(BackwardJump);
		else
			return EnterNewAnimation(NeutralJump);
	}

	if (IsCurrentAnimation(NeutralJump) || IsCurrentAnimation(ForwardJump) || IsCurrentAnimation(BackwardJump) || IsCurrentAnimation(GuardAirOut) ||
		IsCurrentAnimation(AirRecovery) || IsCurrentAnimation(DeflectedAir) || IsCurrentAnimation(ThrowEscapeAir) || IsCurrentAnimation(BlitzOutAir))
	{
		return EnterNewAnimation(MidJump);
	}

	if (IsCurrentAnimation(AirDashBackwardOut) || IsCurrentAnimation(AirDashForwardOut))
		return EnterNewAnimation(JumpTransition);

	if (IsCurrentAnimation(JumpTransition))
	{
		return EnterNewAnimation(JumpDescent);
	}

	if (IsCurrentAnimation(LaunchTransition))
	{
		return EnterNewAnimation(LaunchFallCycle);
	}

	if (IsCurrentAnimation(BackDash))
	{
		CurrentState.Position.Y = 0;
		CurrentState.Velocity.Y = 0;
		CurrentState.bIsAirborne = false;
		return EnterNewAnimation(IdleStand);
	}

	if (IsCurrentAnimation(StandUp) || IsCurrentAnimation(Brake) || IsCurrentAnimation(WakeUpFaceDown) || IsCurrentAnimation(WakeUpFaceUp) || IsCurrentAnimation(GuardHiOut) ||
		IsCurrentAnimation(HitSLOut) || IsCurrentAnimation(HitSHOut) || IsCurrentAnimation(HitSLHeavyOut) || IsCurrentAnimation(HitSHHeavyOut) || IsCurrentAnimation(IdleStandBlink) ||
		IsCurrentAnimation(StandIdleAction) || IsCurrentAnimation(Deflected) || IsCurrentAnimation(ThrowEscape) || IsCurrentAnimation(BlitzOutStanding) || IsCurrentAnimation(TurnAroundStand))
		return EnterNewAnimation(IdleStand);

	if (IsCurrentAnimation(CrouchDown) || IsCurrentAnimation(GuardLoOut) || IsCurrentAnimation(IdleCrouchBlink) || IsCurrentAnimation(CrouchIdleAction) ||
		IsCurrentAnimation(HitCOut) || IsCurrentAnimation(HitCHeavyOut) || IsCurrentAnimation(TurnAroundCrouch))
		return EnterNewAnimation(IdleCrouch);

	if ((IsCurrentAnimation(KnockDownFaceDown) || IsCurrentAnimation(Crumple)) && (CurrentState.Health != 0))
	{
		if (CurrentState.Resolve == 0)
		{
			CurrentState.Resolve = 1;
			CurrentState.Durability = 80;
		}
		return EnterNewAnimation(WakeUpFaceDown);
	}

	if (IsCurrentAnimation(KnockDownFaceUp) && CurrentState.Health != 0)
	{
		if (CurrentState.Resolve == 0)
		{
			CurrentState.Resolve = 1;
			CurrentState.Durability = 80;
		}
		return EnterNewAnimation(WakeUpFaceUp);
	}

	if (IsCurrentAnimation(GuardHiIn))
		return EnterNewAnimation(GuardHi);

	if (IsCurrentAnimation(GuardLoIn))
		return EnterNewAnimation(GuardLo);

	if (IsCurrentAnimation(Sweep) || IsCurrentAnimation(WallBounce))
		return EnterNewAnimation(FallingForward);

	if (IsCurrentAnimation(GroundBounce) || IsCurrentAnimation(HitstunAir))
		return EnterNewAnimation(HitstunAirCycle);

	if (IsCurrentAnimation(FocusBlitz) || IsCurrentAnimation(BreakerBlitz))
	{
		if (CurrentState.bIsAirborne)
			return EnterNewAnimation(BlitzOutAir);
		else
			return EnterNewAnimation(BlitzOutStanding);
	}

	return false;
}

void ABTCharacterBase::AnimationEvents()
{
	if (IsCurrentAnimation(FocusBlitz) || IsCurrentAnimation(BreakerBlitz))
	{
		if (IsCurrentAnimation(FocusBlitz)) //Focus Blitz
		{
			if (CurrentState.AnimFrameIndex == 1 && CurrentState.PosePlayTime == 0)
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 1);

			if (CurrentState.AnimFrameIndex == 7 && CurrentState.PosePlayTime == 0)
				BlitzImage->ActivateWave();

			if (CurrentState.AnimFrameIndex == FocusBlitz.Num() - 1 && CurrentState.PosePlayTime == CurrentState.CurrentAnimFrame.PlayDuration - 1)
			{
				CurrentState.bBlitzing = true;

				//can hold direction to change velocity when exiting Focus Blitz
				if (CurrentState.Dir2 == InputTime)
					CurrentState.Velocity = FVector2D(0, -.25 * BlitzDashForce);
				else if (CurrentState.Dir8 == InputTime)
					CurrentState.Velocity = FVector2D(0, .35 * BlitzDashForce);
				else if (CurrentState.Dir4 == InputTime)
				{
					CurrentState.Velocity = FVector2D(.35f * BlitzDashForce, 0);
					if (CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
				else if (CurrentState.Dir6 == InputTime)
				{
					CurrentState.Velocity = FVector2D(.35f * BlitzDashForce, 0);

					if (!CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
				else if (CurrentState.Dir7 == InputTime)
				{
					CurrentState.Velocity = FVector2D(.35f * BlitzDashForce);

					if (CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
				else if (CurrentState.Dir9 == InputTime)
				{
					CurrentState.Velocity = FVector2D(.35f * BlitzDashForce);

					if (!CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
				else if (CurrentState.Dir1 == InputTime)
				{
					CurrentState.Velocity = FVector2D(-.25f * BlitzDashForce);

					if (!CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
				else if (CurrentState.Dir3 == InputTime)
				{
					CurrentState.Velocity = FVector2D(-.25f * BlitzDashForce);

					if (CurrentState.bFacingRight)
						CurrentState.Velocity.X *= -1;
				}
			}
		}
		else
		{
			if (CurrentState.AnimFrameIndex == 4 && CurrentState.PosePlayTime == 0)
				BlitzImage->ActivateWave();

			if (CurrentState.AnimFrameIndex == 5 && CurrentState.PosePlayTime == 0)
				CurrentState.bBlitzing = true;
		}
	}
	else if (IsCurrentAnimation(NeutralJump) || IsCurrentAnimation(ForwardJump) || IsCurrentAnimation(BackwardJump))
	{
		if (CurrentState.PosePlayTime == 0 && CurrentState.AnimFrameIndex == 0) //Jump animation event
		{
			Jumping();
		}
	}
	else if (IsCurrentAnimation(WalkForward))
	{
		CurrentState.Velocity = FVector2D(WalkSpeed, 0);

		if (!CurrentState.bFacingRight)
			CurrentState.Velocity.X *= -1;

	}
	else if (IsCurrentAnimation(WalkBackward))
	{
		CurrentState.Velocity = FVector2D(WalkBackSpeed, 0);

		if (CurrentState.bFacingRight)
			CurrentState.Velocity.X *= -1;
	}
	else if (IsCurrentAnimation(BackDash) && CurrentState.PosePlayTime == 0 && CurrentState.AnimFrameIndex == 0)
	{
		CurrentState.Velocity = BackDashForce;

		if (CurrentState.bFacingRight)
			CurrentState.Velocity.X *= -1;
	}
}

bool ABTCharacterBase::QCF()
{
	return (CurrentState.Dir2 > 0 && CurrentState.Dir3 > 0 && CurrentState.Dir6 > 0 && CurrentState.Dir6 > CurrentState.Dir3 && CurrentState.Dir3 > CurrentState.Dir2);
}

bool ABTCharacterBase::QCB()
{
	return (CurrentState.Dir2 > 0 && CurrentState.Dir1 > 0 && CurrentState.Dir4 > 0 && CurrentState.Dir4 > CurrentState.Dir1 && CurrentState.Dir1 > CurrentState.Dir2);
}

bool ABTCharacterBase::HCF()
{
	return (CurrentState.Dir4 > 0 && CurrentState.Dir2 > 0 && CurrentState.Dir6 > 0 && CurrentState.Dir6 > CurrentState.Dir2 && CurrentState.Dir2 > CurrentState.Dir4);
}

bool ABTCharacterBase::HCB()
{
	return (CurrentState.Dir4 > 0 && CurrentState.Dir2 > 0 && CurrentState.Dir6 > 0 && CurrentState.Dir6 < CurrentState.Dir2&& CurrentState.Dir2 < CurrentState.Dir4);
}

bool ABTCharacterBase::DP()
{
	return (CurrentState.Dir6 > 0 && CurrentState.Dir2 > 0 && CurrentState.Dir3 > 0 && CurrentState.Dir3 > CurrentState.Dir2 && CurrentState.Dir2 > CurrentState.Dir6);
}

bool ABTCharacterBase::RDP()
{
	return (CurrentState.Dir4 > 0 && CurrentState.Dir2 > 0 && CurrentState.Dir1 > 0 && CurrentState.Dir1 > CurrentState.Dir2 && CurrentState.Dir2 > CurrentState.Dir4);
}

bool ABTCharacterBase::FC()
{
	return (CurrentState.Dir4 > 0 && CurrentState.Dir2 > 0 && CurrentState.Dir6 > 0 && CurrentState.Dir8) && ((CurrentState.Dir8 > CurrentState.Dir4 && CurrentState.Dir4 > CurrentState.Dir2 && CurrentState.Dir2 > CurrentState.Dir6) || (CurrentState.Dir8 > CurrentState.Dir6 && CurrentState.Dir6 > CurrentState.Dir2 && CurrentState.Dir2 > CurrentState.Dir4));
}

void ABTCharacterBase::RefreshMovelist()
{
	CurrentState.MoveList = 0;
}

bool ABTCharacterBase::RectangleOverlap(FVector2D Pos1, FVector2D Pos2, FVector2D Size1, FVector2D Size2)
{
	FVector2D TL1, TL2, BR1, BR2; //TL: Top left corner, BR: Bottom right corner

	TL1 = FVector2D(Pos1.X - .5f * Size1.X, Pos1.Y + .5f * Size1.Y);
	BR1 = FVector2D(Pos1.X + .5f * Size1.X, Pos1.Y - .5f * Size1.Y);

	TL2 = FVector2D(Pos2.X - .5f * Size2.X, Pos2.Y + .5f * Size2.Y);
	BR2 = FVector2D(Pos2.X + .5f * Size2.X, Pos2.Y - .5f * Size2.Y);

	// cannot overlap if either rectangle is a line
	if (TL1.X == BR1.X || TL1.Y == BR1.Y || TL2.X == BR2.X || TL2.Y == BR2.Y) 
		return false;

	// no overlap if left side of one rectangle is to the right of the other's right side
	if (TL1.X >= BR2.X || TL2.X >= BR1.X)
		return false;

	// no overlap if one rectangle is above the other
	if (TL1.Y <= BR2.Y || TL2.Y <= BR1.Y)
		return false;

	return true;
}

void ABTCharacterBase::ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter)
{
	Opponent->TurnAroundCheck();
	if (!CurrentState.CurrentAnimFrame.bCinematic)
	{
		DepthOffset = 0;
		Opponent->DepthOffset = 300;
	}

	//If the opponent would be on the ground on the next frame, treat them as if they were hit while on the ground
	if (Opponent->CurrentState.CurrentAnimFrame.Invincibility != FaceUp && Opponent->CurrentState.CurrentAnimFrame.Invincibility != FaceDown && Opponent->CurrentState.HitStun == 0
		&& Opponent->CurrentState.BlockStun == 0 && Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.Position.Y + (Opponent->CurrentState.Velocity.Y * 100.f / 60.f) <= 0)
	{
		Opponent->CurrentState.Position.Y = 0;
		Opponent->CurrentState.Velocity.Y = 0;
		Opponent->CurrentState.bIsAirborne = false;
		if (Opponent->CurrentState.Dir1 == InputTime || Opponent->CurrentState.Dir2 == InputTime || Opponent->CurrentState.Dir3 == InputTime)
		{
			Opponent->CurrentState.bIsCrouching = true;
		}
	}


	//Opponent successfully guarded the attack
	if (((Opponent->CurrentState.bIsGuarding && Opponent->CurrentState.bIsAirborne && !(Hitbox.AttackProperties & AntiAir)) || (Opponent->CurrentState.bIsGuarding && Opponent->CurrentState.bIsAirborne && (Hitbox.AttackProperties & AntiAir) && Opponent->CurrentState.JustDefense >= 0) || 
		(Opponent->CurrentState.bIsGuarding && Hitbox.AttackHeight <= High) || (Opponent->CurrentState.bIsGuarding && !Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.bIsCrouching && Hitbox.AttackHeight == Low) ||
		(Opponent->CurrentState.bIsGuarding && !Opponent->CurrentState.bIsAirborne && !Opponent->CurrentState.bIsCrouching && Hitbox.AttackHeight == Overhead)) && Hitbox.AttackHeight != Unblockable)
	{
		int32 BlockstunToApply;
		FVector2D KnockbackToApply;
		if (Hitbox.BaseBlockStun == 0)
			BlockstunToApply = Hitbox.BaseHitStun * .8f;
		else
			BlockstunToApply = Hitbox.BaseBlockStun;

		if (Opponent->CurrentState.bIsAirborne)
		{
			if (FMath::Abs(Hitbox.PotentialAirKnockBack.X) > FMath::Abs(Hitbox.PotentialAirKnockBack.Y))
				KnockbackToApply = FVector2D(Hitbox.PotentialAirKnockBack.X, 1.f);
			else
				KnockbackToApply = FVector2D(.5f * (Hitbox.PotentialAirKnockBack.X + FMath::Abs(Hitbox.PotentialAirKnockBack.Y)), 1.f);
		}
		else
		{
			if (FMath::Abs(Hitbox.PotentialKnockBack.X) > FMath::Abs(Hitbox.PotentialKnockBack.Y))
				KnockbackToApply = FVector2D(Hitbox.PotentialKnockBack.X, 0);
			else
				KnockbackToApply = FVector2D(.5f * (Hitbox.PotentialKnockBack.X + FMath::Abs(Hitbox.PotentialKnockBack.Y)), 0);
		}

		BlockstunToApply = FMath::Min(BlockstunToApply, 30);

		if (Opponent->CurrentState.JustDefense >= 0) //if the opponent Instant Blocked the attack
		{
			//Cuts blockstun by a third on Just defense, cuts by 2 frame at minimum
			if (BlockstunToApply < 9)
				BlockstunToApply -= 2;
			else
				BlockstunToApply = BlockstunToApply * 2 / 3;

			BlockstunToApply = FMath::Max(1, BlockstunToApply);

			if (Opponent->CurrentState.bIsAirborne)
				KnockbackToApply = FVector2D(-Opponent->CurrentState.Velocity.X, .5f);
			else
				KnockbackToApply *= 0;

			Opponent->CurrentState.Durability += 35; //reward opponent for blocking with exceptional timing
			Opponent->CurrentState.JustDefense = 0;
			//make opponent flash white
			Opponent->StatusMix = .5f;
			Opponent->CurrentState.StatusTimer = 8;
			Opponent->StatusColor = FVector(1);
			UE_LOG(LogTemp, Warning, TEXT("JUST DEFEND")); //ui Instant block effect "Instant"
		}
		else
		{
			Opponent->CurrentState.ResolveRecoverTimer = 0;
			Opponent->CurrentState.RecoverInterval = 0;

			//blocked hits chip away at durability
			if (Opponent->CurrentState.Resolve > 0)
			{
				if (Opponent->CurrentState.bIsAirborne)
				{
					if (Hitbox.AttackProperties & AntiAir) //attacks with the anti air property must be Instant Blocked
					{
						Opponent->CurrentState.Durability = -1;
						AttackCalculation(Hitbox, HurtboxCenter);
						return;
					}

					if (Hitbox.AttackProperties & IsSuper)
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .35f);
					else if (Hitbox.AttackProperties & IsSpecial)
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .25f);
					else
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .2f);
				}
				else
				{
					if (Hitbox.AttackProperties & IsSuper)
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .25f);
					else if (Hitbox.AttackProperties & IsSpecial)
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .15f);
					else
						Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * .1f);
				}

			}
			else
			{
				//deal chip damage if the opponent has no resolve left, shatter their guard if they are airborne
				if (Opponent->CurrentState.bIsAirborne)
				{
					Opponent->CurrentState.bCounterHitState = true;
					AttackCalculation(Hitbox, HurtboxCenter);
					return;
				}
				else
				{
					int32 ChipDamage = (int32)(Hitbox.BaseDamage * .2f);

					if (Opponent->CurrentState.Health - ChipDamage <= 0 && Opponent->CurrentState.Health > 1) //Chip damage reduces character's life to 1 before being lethal
						ChipDamage = Opponent->CurrentState.Health - 1;
					else
						ChipDamage = FMath::Min(Opponent->CurrentState.Health, ChipDamage);

					Opponent->CurrentState.Health -= ChipDamage;

					if (Opponent->CurrentState.Health == 0)
					{
						AttackCalculation(Hitbox, HurtboxCenter);
						return;
					}
				}
			}
		}

		//apply final blockstun calculation
		Opponent->CurrentState.BlockStun = BlockstunToApply;

		if (Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.JustDefense < 0) //extra blockstun if the opponent is airborne
		{
			Opponent->CurrentState.BlockStun += 3;
			Opponent->CurrentState.JumpsUsed = 0;
		}

		//apply hitstop
		Opponent->CurrentState.HitStop = FMath::Min((int32)Hitbox.BaseHitStop - 2, 24);
		CurrentState.HitStop = FMath::Min((int32)Hitbox.BaseHitStop - 2, 24);

		if (Opponent->CurrentState.bTouchingWall && KnockbackToApply.X >= 0)
		{
			if (!(Hitbox.AttackProperties & IsSuper) && !(Hitbox.AttackProperties & IsSpecial))
				CurrentState.KnockBack = FVector2D(.75f * KnockbackToApply.X, 0);
		}
		else
		{
			if (KnockbackToApply.X < 0)
				Opponent->CurrentState.KnockBack = FVector2D(.5f * KnockbackToApply.X, KnockbackToApply.Y);
			else
				Opponent->CurrentState.KnockBack = KnockbackToApply;
		}

		//Made contact so can cancel into other actions
		CurrentState.AvailableActions = Hitbox.PotentialActions;
		CurrentState.AvailableActions &= AcceptAll - AcceptJump - AcceptBlitz;

		if (Hitbox.PotentialActions & JumpCancelOnBlock && Hitbox.PotentialActions & AcceptJump) //can only jump cancel if the move allows it on block
			CurrentState.AvailableActions |= AcceptJump;

		if (!(Hitbox.PotentialActions & BlitzOnHitOnly) && Hitbox.PotentialActions & AcceptBlitz) //can blitz cancel if it allows it on block
			CurrentState.AvailableActions |= AcceptBlitz;

		//update opponent's animation to guarding
		if (Opponent->CurrentState.bIsAirborne)
			Opponent->EnterNewAnimation(Opponent->GuardAir);
		else
		{
			if (Opponent->CurrentState.bIsCrouching)
			{
				if (Hitbox.AttackProperties & IsHeavy)
					Opponent->EnterNewAnimation(Opponent->GuardLoHeavy);
				else
					Opponent->EnterNewAnimation(Opponent->GuardLo);
			}
			else
			{
				if (Hitbox.AttackProperties & IsVertical)
					Opponent->EnterNewAnimation(Opponent->GuardHiVertical);
				else if (Hitbox.AttackProperties & IsHeavy)
					Opponent->EnterNewAnimation(Opponent->GuardHiHeavy);
				else
					Opponent->EnterNewAnimation(Opponent->GuardHi);
			}
		}

		//place and play guard effect
		//place at midpoint between hitbox center and hurtbox center
	}
	else if (Opponent->CurrentState.SlowMoTime == 0 && Opponent->CurrentState.bArmorActive && Opponent->CurrentState.Resolve > 0 && !(Hitbox.AttackProperties & Piercing) && !(Hitbox.AttackProperties & Shatter))
	{
		Opponent->CurrentState.ResolveRecoverTimer = 0;
		Opponent->CurrentState.RecoverInterval = 0;
		Opponent->CurrentState.Durability -= Hitbox.DurabilityDamage;
		Opponent->CurrentState.Resolve -= Hitbox.ResolveDamage;
		CurrentState.HitStop = Hitbox.BaseHitStop;
		Opponent->CurrentState.HitStop = CurrentState.HitStop;
		//make opponent flash red
		Opponent->StatusMix = .8f;
		Opponent->CurrentState.StatusTimer = 8;
		Opponent->StatusColor = FVector(1, 0, 0);

		//available actions are more limited when hitting an opponent's armor
		if (Hitbox.PotentialActions & AcceptSpecial)
			CurrentState.AvailableActions |= AcceptSpecial;
		if (Hitbox.PotentialActions & AcceptSuper)
			CurrentState.AvailableActions |= AcceptSuper;
		if (Hitbox.PotentialActions & AcceptJump)
			CurrentState.AvailableActions |= AcceptJump;
		if (Hitbox.PotentialActions & AcceptBlitz)
			CurrentState.AvailableActions |= AcceptBlitz;

		//place and play armor hit effect
		//place at midpoint between hitbox center and hurtbox center
	}
	else //the attack hit the opponent
	{
		AttackCalculation(Hitbox, HurtboxCenter);
	}
}

void ABTCharacterBase::AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter)
{
	if (CurrentState.ComboCount == 0)
	{
		CurrentState.SpecialProration = 1;
		Opponent->CurrentState.GravDefyTime = 0;
		CurrentState.SlowMoTime = 0;
	}

	if (Hitbox.AttackHeight < Throw)
		CurrentState.ComboCount++;
	Opponent->CurrentState.BlockStun = 0;
	CurrentState.bHitSuccess = true;

	if (Hitbox.AttackHeight < Throw && (Opponent->IsCurrentAnimation(Opponent->Crumple) || Opponent->IsCurrentAnimation(Opponent->BackDash) || Opponent->CurrentState.CurrentAnimFrame.Invincibility == OTG)) //treat opponent as if airborne if hitting them in crumple
	{
		Opponent->CurrentState.bIsAirborne = true;
		Opponent->CurrentState.Position.Y += .05f;
	}

	if (CurrentState.ComboCount >= 2)
	{
		//if the opponent is in an aerial hitstun animation or staggered but their hitstun is zero, they could have escaped
		if ((!Opponent->IsCurrentAnimation(Opponent->WallBounce) && !Opponent->IsCurrentAnimation(Opponent->GroundBounce) && Opponent->CurrentState.HitStun == 0 && Opponent->CurrentState.ShatteredTime == 0 && Opponent->CurrentState.bIsAirborne &&
			(Opponent->CurrentState.CurrentAnimFrame.Invincibility == FaceDown || Opponent->CurrentState.CurrentAnimFrame.Invincibility == FaceUp)) ||
			(Opponent->IsCurrentAnimation(Opponent->Stagger) && Opponent->CurrentState.HitStun == 0))
			CurrentState.bTrueCombo = false;
		//display UI combo counter
	}
	else
	{
		//hide UI combo counter
	}

	if (Hitbox.AttackProperties & ForceCrouch && !Opponent->CurrentState.bIsAirborne)
		Opponent->CurrentState.bIsCrouching = true;

	//calculate damage scaling based on opponent's remaining health
	float OpponentValor;
	float OpponentHealthPercent = (float)Opponent->CurrentState.Health / (float)Opponent->MaxHealth;

	if (OpponentHealthPercent <= .1f)
		OpponentValor = Opponent->Valor10;
	else if (OpponentHealthPercent <= .25f)
		OpponentValor = Opponent->Valor25;
	else if (OpponentHealthPercent <= .5f)
		OpponentValor = Opponent->Valor50;
	else
		OpponentValor = Opponent->Valor100;

	int32 DamageToApply = OpponentValor * CurrentState.SpecialProration * Hitbox.BaseDamage;
	uint8 HitStunToApply = Hitbox.BaseHitStun;
	FVector2D KnockBackToApply;
	uint8 HitStopToApply = Hitbox.BaseHitStop;

	if (Opponent->CurrentState.CurrentAnimFrame.Invincibility == OTG)
	{
		KnockBackToApply = FVector2D(1.1f * FMath::Abs(Hitbox.PotentialKnockBack.X), 3);
	}
	else if (Opponent->CurrentState.bIsAirborne)
	{
		if (Hitbox.PotentialAirKnockBack == FVector2D(0, 0) && Hitbox.PotentialKnockBack.Y == 0)
			KnockBackToApply = FVector2D(Hitbox.PotentialKnockBack.X, 2.25f);
		else
			KnockBackToApply = Hitbox.PotentialAirKnockBack;
	}
	else
	{
		KnockBackToApply = Hitbox.PotentialKnockBack;
	}

	CurrentState.AvailableActions = Hitbox.PotentialActions;
	Opponent->CurrentState.CharacterHitState = Hitbox.AttackProperties;
	//apply certain modifiers based on circumstances around the hit
	if (Hitbox.AttackHeight < Throw)
	{
		if ((Opponent->CurrentState.bArmorActive || Opponent->CurrentState.bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			Opponent->CurrentState.ShatteredTime = 120;
			Opponent->CurrentState.ResolveRecoverTimer = 0;
			Opponent->CurrentState.RecoverInterval = 0;
			HitStunToApply *= 1.2f;
			Opponent->CurrentState.CharacterHitState |= Hitbox.CounterAttackProperties;
			//set shatter UI effect to play
		}
		else if ((Opponent->CurrentState.bArmorActive || Opponent->CurrentState.bCounterHitState) && 
			(!(Hitbox.AttackProperties & Piercing && Opponent->CurrentState.bArmorActive && Opponent->CurrentState.Resolve > 0) || Opponent->CurrentState.SlowMoTime > 0))
		{
			//increase hitstun and vertical knockback on counterhit
			HitStunToApply += HitStunToApply/2;
			if (KnockBackToApply.Y > 0)
				KnockBackToApply.Y *= 1.2f;
			HitStopToApply += HitStopToApply / 2;
			Opponent->CurrentState.CharacterHitState |= Hitbox.CounterAttackProperties;
			//set counter hit ui effect to play
		}
		else if (Hitbox.AttackProperties & Piercing && Opponent->CurrentState.bArmorActive && Opponent->CurrentState.Resolve > 0)
		{
			//set piercing ui effect to play
		}
	}

	if (Opponent->CurrentState.CharacterHitState & CanMidScreenWallBounce)
		Opponent->CurrentState.WallBounceTime = 24;
	else
		Opponent->CurrentState.WallBounceTime = 0;
		
	if (Opponent->CurrentState.CurrentAnimFrame.Invincibility == OTG)
	{
		DamageToApply = (int32)(.3f * DamageToApply);
		HitStunToApply = (uint8)(.3f * HitStunToApply);
	}

	//calculate proration to apply on subsequent hits in a combo
	if (CurrentState.ComboCount == 1)
	{
		CurrentState.SpecialProration = Hitbox.InitProration; //initial proration is only applied if its the first hit of a combo
		CurrentState.bTrueCombo = true; //reset to true since a new combo has started
		if ((Opponent->CurrentState.bArmorActive || Opponent->CurrentState.bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			CurrentState.SpecialProration *= 1.2f;
			DamageToApply *= 1.2f;
		}
		else if (Opponent->CurrentState.bCounterHitState)
			CurrentState.SpecialProration *= 1.1f;
	}
	else if (CurrentState.ComboCount > 1 && Hitbox.ForcedProration != 1)
		CurrentState.SpecialProration *= Hitbox.ForcedProration; //forced proration is applied as long as the move is used in combo

	//apply damage, damage is scaled by the number of hits in a combo
	float ComboProration;
	if (CurrentState.ComboCount < 3)
		ComboProration = 1;
	else if (CurrentState.ComboCount < 5)
		ComboProration = .8f;
	else if (CurrentState.ComboCount < 6)
		ComboProration = .7f;
	else if (CurrentState.ComboCount < 7)
		ComboProration = .6f;
	else if (CurrentState.ComboCount < 8)
		ComboProration = .5f;
	else if (CurrentState.ComboCount < 9)
		ComboProration = .4f;
	else if (CurrentState.ComboCount < 10)
		ComboProration = .3f;
	else if (CurrentState.ComboCount < 11)
		ComboProration = .2f;
	else
		ComboProration = .1f;

	DamageToApply *= ComboProration;

	if (Hitbox.AttackProperties & IsSuper)
		DamageToApply = FMath::Max((int32)(OpponentValor * Hitbox.BaseDamage * .25f), DamageToApply); //Supers will always deal a minimum of 25% their base damage affected by valor

	DamageToApply = FMath::Max(1, DamageToApply); //non-super attacks will always deal a minimum of one damage

	Opponent->CurrentState.Health -= FMath::Min(DamageToApply, Opponent->CurrentState.Health);

	//apply hitstun, hitstun is scaled by how much time the opponent has spent in hitstun, supers' hitstun is never scaled
	if (Opponent->CurrentState.bIsAirborne && !(Hitbox.AttackProperties & IsSuper))
	{
		if (Opponent->CurrentState.ComboTimer > 960 && !(Hitbox.AttackProperties & IsSpecial)) //16 seconds, normal attacks will only deal 1 frame of hitstun
			HitStunToApply = 1;
		else if (Opponent->CurrentState.ComboTimer > 840) //14 seconds, special attacks have a minimum of 60% their base hitstun
			HitStunToApply *= .6f;
		else if (Opponent->CurrentState.ComboTimer > 600)//10 seconds
			HitStunToApply *= .7f;
		else if (Opponent->CurrentState.ComboTimer > 420)//7 seconds
			HitStunToApply *= .8f;
		else if (Opponent->CurrentState.ComboTimer > 300)//5 seconds
			HitStunToApply *= .9f;
	}
	Opponent->CurrentState.HitStun = HitStunToApply;
	if (Opponent->CurrentState.bIsCrouching || Opponent->IsCurrentAnimation(Opponent->GuardLo) || Opponent->IsCurrentAnimation(Opponent->GuardLoHeavy)) //two extra frames of hitstun if hitting a crouching opponent
		Opponent->CurrentState.HitStun += 2;

	//apply hitstop
	Opponent->CurrentState.HitStop = HitStopToApply;
	CurrentState.HitStop = HitStopToApply;

	//meter gain for each character
	if (Opponent->CurrentState.ShatteredTime == 0)
		Opponent->CurrentState.Durability += FMath::Max((int32)(Hitbox.BaseDamage * .2f), 1);

	if (CurrentState.ResolveRecoverTimer == 180)
		CurrentState.Durability += FMath::Max((int32)(Hitbox.BaseDamage * .1f), 1);

	//Make certain actions available for hitting with an attack
	CurrentState.AvailableActions = Hitbox.PotentialActions;

	//Apply knockback to opponent
	Opponent->CurrentState.KnockBack = KnockBackToApply;

	//apply pushback to self when not using a special or super
	if (!(Hitbox.AttackProperties & IsSpecial) && !(Hitbox.AttackProperties & IsSuper))
	{
		float WallPushBack = 0, PushBack = 0;
		if (Opponent->CurrentState.bTouchingWall) //if opponent is against a wall, character has to move back instead albeit with less magnitude
		{
			if (FMath::Abs(Hitbox.PotentialKnockBack.X) > FMath::Abs(Hitbox.PotentialKnockBack.Y))
				WallPushBack = .5f * FMath::Abs(Hitbox.PotentialKnockBack.X);
			else
				WallPushBack = .25f * (FMath::Abs(Hitbox.PotentialKnockBack.X) + FMath::Abs(Hitbox.PotentialKnockBack.Y));
		}

		if (Opponent->CurrentState.ComboTimer > 180) //will not experience pushback during first three seconds of a combo
		{
			if (Opponent->CurrentState.ComboTimer < 300)
				PushBack = .75f;
			else if (Opponent->CurrentState.ComboTimer < 420)
				PushBack = 1.f;
			else if (Opponent->CurrentState.ComboTimer < 540)
				PushBack = 1.25f;
			else if (Opponent->CurrentState.ComboTimer < 720)
				PushBack = 1.5f;
			else if (Opponent->CurrentState.ComboTimer < 900)
				PushBack = 1.75f;
			else
				PushBack = 2.f;
		}

		if (WallPushBack > PushBack)
			CurrentState.KnockBack.X = WallPushBack;
		else
			CurrentState.KnockBack.X = PushBack;
	}
}

void ABTCharacterBase::ContactThrow(FHitbox Hitbox, int32 ThrowType)
{
	Opponent->TurnAroundCheck();

	if (ThrowType == Throw || ThrowType == AirThrow)
	{
		if (Opponent->CurrentState.AvailableActions & ThrowTech)
		{
			CurrentState.bClash = true;
			Opponent->CurrentState.bClash = true;
			CurrentState.KnockBack = FVector2D(-1, 0);
			Opponent->CurrentState.KnockBack = FVector2D(-1, 0);

			if (ThrowType == AirThrow)
			{
				EnterNewAnimation(ThrowEscapeAir);
				Opponent->EnterNewAnimation(Opponent->ThrowEscapeAir);
			}
			else
			{
				EnterNewAnimation(ThrowEscape);
				Opponent->EnterNewAnimation(Opponent->ThrowEscape);
			}

			//play throw escape effect
		}
		else if (Opponent->CurrentState.Resolute)
		{
			//play Resolute Counter UI graphic
			Opponent->CurrentState.ResolveRecoverTimer = 180;
			Opponent->CurrentState.Durability = 101;

			if (ThrowType == AirThrow)
			{
				EnterNewAnimation(DeflectedAir);
				Opponent->EnterNewAnimation(Opponent->AirResoluteCounter);
			}
			else
			{
				EnterNewAnimation(Deflected);
				Opponent->EnterNewAnimation(Opponent->ResoluteCounter);
			}
		}
	}
	else if ((ThrowType == CommandThrow || ThrowType == AirCommandThrow) && Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)  //check if both players have landed a command throw, to induce a throwbreak
	{
		if (Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
		{
			if ((Opponent->CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == CommandThrow && !CurrentState.bIsAirborne) || (Opponent->CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == AirCommandThrow && CurrentState.bIsAirborne))
			{
				for (uint8 i = 0; i < Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bClash; i++)
				{
					FVector2D OpponentHitboxCenter;
					if (CurrentState.bFacingRight)
						OpponentHitboxCenter = Opponent->CurrentState.Position + Opponent->CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
					else
						OpponentHitboxCenter = FVector2D(Opponent->CurrentState.Position.X - Opponent->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + Opponent->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

					FVector2D PushboxCenter = FVector2D(CurrentState.Position.X, CurrentState.Position.Y);
					FVector2D PushboxSize = FVector2D(PushboxWidth, StandingPushBoxHeight);

					if (CurrentState.bIsAirborne)
					{
						PushboxCenter.Y += .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset;
						PushboxSize.Y = CrouchingPushBoxHeight;
					}
					else if (CurrentState.bIsCrouching)
					{
						PushboxCenter.Y += .5f * CrouchingPushBoxHeight;
						PushboxSize.Y = CrouchingPushBoxHeight;
					}
					else
						PushboxCenter.Y += .5f * StandingPushBoxHeight;

					if (RectangleOverlap(OpponentHitboxCenter, PushboxCenter, Opponent->CurrentState.CurrentAnimFrame.Hitboxes[i].Size, PushboxSize))
					{
						CurrentState.bClash = true;
						Opponent->CurrentState.bClash = true;
						CurrentState.KnockBack = FVector2D(-2, 0);
						Opponent->CurrentState.KnockBack = FVector2D(-2, 0);

						if (ThrowType == AirCommandThrow)
						{
							EnterNewAnimation(ThrowEscapeAir);
							Opponent->EnterNewAnimation(Opponent->ThrowEscapeAir);
						}
						else
						{
							EnterNewAnimation(ThrowEscape);
							Opponent->EnterNewAnimation(Opponent->ThrowEscape);
						}
						//play throw escape effect
					}
				}
			}
		}

	}

	if (!CurrentState.bClash)
	{
		if (!CurrentState.CurrentAnimFrame.bCinematic)
		{
			DepthOffset = 0;
			Opponent->DepthOffset = 300;
		}
		CurrentState.bAttackMadeContact = true;
		AttackCalculation(Hitbox, Opponent->CurrentState.Position);
	}
}

void ABTCharacterBase::SetSounds()
{
	if (CurrentState.PosePlayTime == 0)
	{
		if (CurrentState.CurrentAnimFrame.SFX != nullptr)
		{
			CharacterSoundEffects->SetSound(CurrentState.CurrentAnimFrame.SFX);
			CurrentState.bPlaySound = true;
		}
		else
		{
			CharacterSoundEffects->SetSound(nullptr);
		}

		if (CurrentState.CurrentAnimFrame.VoiceLines != nullptr)
		{
			CharacterVoice->SetSound(CurrentState.CurrentAnimFrame.VoiceLines);
			CurrentState.bPlaySound = true;
		}
		else
		{
			CharacterVoice->SetSound(nullptr);
		}
	}
}

void ABTCharacterBase::SaveFXStates()
{
	for (uint8 i = 0; i < Sigils.Num(); i++)
	{
		CurrentState.CurrentSigilStates[i] = Sigils[i]->CurrentState;
	}

	if (CurrentState.CurrentBlitzState.Num() > 0)
		CurrentState.CurrentBlitzState[0] = BlitzImage->CurrentState;

	if (CurrentState.CurrentProjectileStates.Num() == Projectiles.Num() && CurrentState.CurrentProjectileStates.Num() > 0)
	{
		for (uint8 i = 0; i < Projectiles.Num(); i++)
		{
			CurrentState.CurrentProjectileStates[i] = Projectiles[i]->CurrentState;
		}
	}
}

bool ABTCharacterBase::BlitzCancel()
{
	if (((CurrentState.AvailableActions & AcceptBlitz && CurrentState.Resolve > 0) || (CurrentState.BlockStun > 0 && !CurrentState.bIsAirborne && CurrentState.Resolve > 1)) && CurrentState.SlowMoTime == 0 &&
		CurrentState.MPressed > 0 && CurrentState.HPressed > 0 && FMath::Abs(CurrentState.MPressed - CurrentState.HPressed) <= 3) //Blitz cancel is performed by hitting M and H at the same time
	{
		CurrentState.Resolve--;
		CurrentState.Durability = 80;
		CurrentState.MPressed = 0;
		CurrentState.HPressed = 0;
		CurrentState.ResolveRecoverTimer = 0;
		CurrentState.RecoverInterval = 0;

		if (CurrentState.bIsAirborne && CurrentState.BlockStun == 0)
		{
			if (CurrentState.Dir6 == InputTime) //blitz air dash forward
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 0);
				TurnAroundCheck();
				CurrentState.Velocity = FVector2D(BlitzDashForce, 0);
				CurrentState.GravDefyTime = 24;
				if (!CurrentState.bFacingRight)
					CurrentState.Velocity.X *= -1;

				CurrentState.bBlitzing = true;

				FRotator SigilRotation = FRotator(0, 0, 30);
				if (CurrentState.bFacingRight)
				{
					SigilRotation.Pitch = -80;
				}
				else
				{
					SigilRotation.Pitch = 80;
				}

				FVector2D SigilPosition = CurrentState.Position;
				if (CurrentState.bFacingRight)
				{
					SigilPosition += AirDashForwardOffset;
				}
				else
				{
					SigilPosition.X -= AirDashForwardOffset.X;
					SigilPosition.Y += AirDashForwardOffset.Y;
				}
				Sigils[0]->Activate(SigilPosition, SigilRotation);

				return EnterNewAnimation(AirDashForward);
			}
			if (CurrentState.Dir4 == InputTime) //blitz air dash backward
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 0);
				TurnAroundCheck();
				CurrentState.Velocity = FVector2D(-.85f * BlitzDashForce, 0);
				CurrentState.GravDefyTime = 24;
				if (!CurrentState.bFacingRight)
					CurrentState.Velocity.X *= -1;

				FRotator SigilRotation = FRotator(0, 0, 30);
				if (CurrentState.bFacingRight)
				{
					SigilRotation.Pitch = 80;
				}
				else
				{
					SigilRotation.Pitch = -80;
				}

				FVector2D SigilPosition = CurrentState.Position;
				if (CurrentState.bFacingRight)
				{
					SigilPosition += AirDashBackOffset;
				}
				else
				{
					SigilPosition.X -= AirDashBackOffset.X;
					SigilPosition.Y += AirDashBackOffset.Y;
				}
				Sigils[0]->Activate(SigilPosition, SigilRotation);

				CurrentState.bBlitzing = true;

				return EnterNewAnimation(AirDashBackward);
			}
			if (CurrentState.Dir2 == InputTime) //blitz downward
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 0);
				TurnAroundCheck();
				CurrentState.Velocity = FVector2D(0, -BlitzDashForce);
				CurrentState.GravDefyTime = 0;

				CurrentState.bBlitzing = true;

				return EnterNewAnimation(JumpDescent);
			}
			if (CurrentState.AvailableActions & AcceptMove) //focus blitz
			{
				TurnAroundCheck();

				return EnterNewAnimation(FocusBlitz);
			}

			BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 0);
			CurrentState.bBlitzing = true;
			return EnterNewAnimation(MidJump);
		}
		else
		{
			if (CurrentState.BlockStun > 0) //breaker blitz
			{
				TurnAroundCheck();
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 2);

				CurrentState.Resolve--;

				return EnterNewAnimation(BreakerBlitz);
			}
			if (CurrentState.AvailableActions & AcceptMove) //focus blitz
			{
				TurnAroundCheck();

				return EnterNewAnimation(FocusBlitz);
			}

			BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame.Pose, CurrentState.bFacingRight, 0);
			TurnAroundCheck();
			CurrentState.bBlitzing = true;
			return EnterNewAnimation(IdleStand);
		}
	}
	return false;
}

void ABTCharacterBase::CreateMaterials()
{
	if (Outline != nullptr)
		DynamicOutline = UMaterialInstanceDynamic::Create(Outline, this);
	if (EyeShine != nullptr)
		DynamicEyeShine = UMaterialInstanceDynamic::Create(EyeShine, this);
}

void ABTCharacterBase::SpawnPBS() //spawn projectiles, blitz image, and sigils
{
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;

		if (SigilBlueprint)
		{
			for (uint8 i = 0; i < 2; i++)
			{
				Sigils.Add(GetWorld()->SpawnActor<ASigil>(SigilBlueprint, GetActorLocation(), FRotator(0), SpawnParams)); //create a sigil
				CurrentState.CurrentSigilStates.Add(Sigils[i]->CurrentState); //add its state to the serializable characterstate struct
			}
		}

		if (BlitzImageBlueprint)
		{
			BlitzImage = GetWorld()->SpawnActor<ABlitzImageBase>(BlitzImageBlueprint, GetActorLocation(), FRotator(0), SpawnParams);
			BlitzImage->AssignOwner(this);
			CurrentState.CurrentBlitzState.Add(BlitzImage->CurrentState);
		}
	}
}

void ABTCharacterBase::SetColor(uint8 ColorID)
{
	//switch statement to set color/emissivity values for all dynamic materials for a character based on colorID
	//search for textures using filepaths, BC, SSS
	//ILMs and Lines should be pointers in character classes
	//set parameters in dynamic material instances for Character Mesh and BlitzImage Mesh
	//Cast blitzimage as character's unique blitz image actor and set texture/vector parameters for blitzimage materials
}

void ABTCharacterBase::LightSettings() 
{
	MainLightRotator->SetRelativeRotation(CurrentState.MainLightRotation);
	FillLightRotator->SetRelativeRotation(CurrentState.FillLightRotation);

	//light color overrides if the animation frame specifies
	if (CurrentState.CurrentAnimFrame.MainLightColor != FVector(0))
		CurrentState.MainLightColor = CurrentState.CurrentAnimFrame.MainLightColor;
	else
		CurrentState.MainLightColor = DefaultMainLightColor;

	if (CurrentState.CurrentAnimFrame.RimLightColor != FVector(0))
		CurrentState.RimLightColor = CurrentState.CurrentAnimFrame.RimLightColor;
	else
		CurrentState.RimLightColor = DefaultRimLightColor;

	if (CurrentState.CurrentAnimFrame.FillLightColor != FVector(0))
		CurrentState.FillLightColor = CurrentState.CurrentAnimFrame.FillLightColor;
	else
		CurrentState.FillLightColor = DefaultFillLightColor;
}

void ABTCharacterBase::ProcessBlitz()
{
	CurrentState.bBlitzing = false;
	if (FMath::Sqrt(FMath::Square(CurrentState.CurrentBlitzState[0].Position.X - Opponent->CurrentState.Position.X) + FMath::Square(CurrentState.CurrentBlitzState[0].Position.Y - Opponent->CurrentState.Position.Y)) < 150
		&& Opponent->CurrentState.CurrentAnimFrame.Invincibility != FullInvincible && Opponent->CurrentState.CurrentAnimFrame.Invincibility != OTG)
	{
		if (Opponent->CurrentState.Resolute)
		{
			Opponent->CurrentState.Durability += 50;
			//Play UI Resolute signal
			UE_LOG(LogTemp, Warning, TEXT("RESOLUTE"));
		}
		else if (IsCurrentAnimation(FocusBlitz))
		{
			UE_LOG(LogTemp, Warning, TEXT("Focus Blitz Slow"));
			if (Opponent->IsCurrentAnimation(Opponent->FocusBlitz)) //only way this happens is if both players FB within three frames of each other
			{
				UE_LOG(LogTemp, Warning, TEXT("Focus Blitz QuickDraw Duel?"));
				CurrentState.SlowMoTime = 0;
			}
			else
				Opponent->CurrentState.SlowMoTime = 75;
		}
		else if (IsCurrentAnimation(BreakerBlitz))
		{
			DepthOffset = 0;
			Opponent->DepthOffset = 300;
			//Similar to normal hit behavior based on whether the opponent is guarding or not
			if (Opponent->CurrentState.bIsGuarding)
			{
				//set opponent's animation to guarding
				//guarding logic
				Opponent->TurnAroundCheck();
				//If the opponent would be on the ground on the next frame, treat them as if they were hit while on the ground
				if (Opponent->CurrentState.bIsAirborne && Opponent->CurrentState.Position.Y + Opponent->CurrentState.Velocity.Y * 100 / 60.f <= 0)
				{
					Opponent->CurrentState.Position.Y = 0;
					Opponent->SurfaceContact();
					if (Opponent->CurrentState.Dir1 == InputTime || Opponent->CurrentState.Dir2 == InputTime || Opponent->CurrentState.Dir3 == InputTime)
					{
						CurrentState.bIsCrouching = true;
					}
				}

				//Opponent successfully guarded the attack
				int32 BlockstunToApply = 18;
				FVector2D KnockbackToApply = FVector2D(2, 0);

				if (Opponent->CurrentState.bIsAirborne)
				{
					KnockbackToApply.Y = 1.f;
				}

				if (Opponent->CurrentState.JustDefense >= 0) //if the opponent Instant Blocked the attack
				{
					//Cuts blockstun by a third on Just defense, cuts by 2 frame at minimum
					BlockstunToApply = BlockstunToApply * 2 / 3;

					BlockstunToApply = FMath::Max(1, BlockstunToApply);
					KnockbackToApply *= 0;
					Opponent->CurrentState.Durability += 35; //reward opponent for blocking with exceptional timing
					//make opponent flash white
					Opponent->StatusMix = .5f;
					Opponent->CurrentState.StatusTimer = 8;
					Opponent->StatusColor = FVector(1);
					UE_LOG(LogTemp, Warning, TEXT("JUST DEFEND")); //ui Instant block effect "Instant"
				}

				//apply final blockstun calculation
				Opponent->CurrentState.BlockStun = BlockstunToApply;

				if (Opponent->CurrentState.bIsCrouching) //extra blockstun if the opponent is crouching
					Opponent->CurrentState.BlockStun += 2;

				//apply hitstop
				Opponent->CurrentState.HitStop = 8;
				CurrentState.HitStop = 8;

				if (Opponent->CurrentState.bTouchingWall && KnockbackToApply.X >= 0)
				{
					CurrentState.KnockBack = FVector2D(.75f * KnockbackToApply.X, 0);
				}
				else
				{
					Opponent->CurrentState.KnockBack = KnockbackToApply;
				}

				//update opponent's animation to guarding
				if (CurrentState.bIsAirborne)
					Opponent->EnterNewAnimation(Opponent->GuardAir);
				else
				{
					if (Opponent->CurrentState.bIsCrouching)
					{
						Opponent->EnterNewAnimation(Opponent->GuardLoHeavy);
					}
					else
					{
						Opponent->EnterNewAnimation(Opponent->GuardHiHeavy);
					}
				}

				//place and play guard effect
				//place at midpoint between hitbox center and hurtbox center
			}
			else
			{
				Opponent->CurrentState.KnockBack = FVector2D(6, 0);
				Opponent->CurrentState.SlowMoTime = 18; //Slows opponent for .5 seconds
				if (Opponent->CurrentState.bIsAirborne)
				{
					Opponent->CurrentState.KnockBack.Y = 1.5f;
					Opponent->EnterNewAnimation(Opponent->DeflectedAir);
				}
				else
					Opponent->EnterNewAnimation(Opponent->Deflected);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Slowing Opponent"));
			if (CurrentState.SlowMoTime > 0)
				CurrentState.SlowMoTime = 0;
			else
				Opponent->CurrentState.SlowMoTime = 60; //Slows opponent for one second
		}
	}
}

void ABTCharacterBase::ClashDetection()
{
	//loop through opponent's active hitboxes
	for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
	{
		FVector2D HitboxCenter;
		if (CurrentState.bFacingRight)
			HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
		else
			HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

		for (uint8 j = 0; j < Opponent->CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; j++)
		{
			FVector2D OpponentHitboxCenter;
			if (Opponent->CurrentState.bFacingRight)
				OpponentHitboxCenter = Opponent->CurrentState.Position + Opponent->CurrentState.CurrentAnimFrame.Hitboxes[j].Position;
			else
				OpponentHitboxCenter = FVector2D(Opponent->CurrentState.Position.X - Opponent->CurrentState.CurrentAnimFrame.Hitboxes[j].Position.X,
					Opponent->CurrentState.Position.Y + Opponent->CurrentState.CurrentAnimFrame.Hitboxes[j].Position.Y);

			if (RectangleOverlap(HitboxCenter, OpponentHitboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, Opponent->CurrentState.CurrentAnimFrame.Hitboxes[j].Size))
			{
				CurrentState.bClash = true;
				Opponent->CurrentState.bClash = true;
				CurrentState.bAttackMadeContact = true;
				Opponent->CurrentState.bAttackMadeContact = true;
				//attacks with the deflect can deflect opponents' non-super, non-deflect attacks
				if (CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect && !(Opponent->CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect) &&
					!(Opponent->CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & IsSuper))
				{
					CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);
					Opponent->CurrentState.AvailableActions = None;
					if (Opponent->CurrentState.bIsAirborne)
						Opponent->EnterNewAnimation(Opponent->DeflectedAir);
					else
						Opponent->EnterNewAnimation(Opponent->Deflected);

					CurrentState.HitStop = 9;
					Opponent->CurrentState.HitStop = 9;
				}
				//opponent's attack can deflect
				else if (!(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect) && Opponent->CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect && !(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & IsSuper))
				{
					Opponent->CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);
					CurrentState.AvailableActions = None;
					if (CurrentState.bIsAirborne)
						EnterNewAnimation(DeflectedAir);
					else
						EnterNewAnimation(Deflected);

					CurrentState.HitStop = 9;
					Opponent->CurrentState.HitStop = 9;
				}
				else //otherwise normal clash
				{
					CurrentState.HitStop = 15;
					Opponent->CurrentState.HitStop = 15;
					CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);
					Opponent->CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);
					//play clash effect
				}
			}
		}
	}
}

void ABTCharacterBase::HitAnimation()
{
	if (Opponent->CurrentState.bHitSuccess && CurrentState.HitStun > 0)
	{
		//update opponent's animation
		if (CurrentState.CharacterHitState & CanTumble)
		{
			EnterNewAnimation(Tumble);
		}
		else if (CurrentState.CharacterHitState & CanLaunch && CurrentState.KnockBack.Y != 0)
		{
			if (CurrentState.KnockBack.Y < 0)
				EnterNewAnimation(LaunchFallCycle);
			else
				EnterNewAnimation(LaunchCycle);
		}
		else if (CurrentState.CharacterHitState & CanSweep)
		{
			EnterNewAnimation(Sweep);
		}
		else if (CurrentState.bIsAirborne || CurrentState.KnockBack.Y != 0)
		{
			CurrentState.bIsAirborne = true;
			if (CurrentState.CharacterHitState & CanKnockAway)
				EnterNewAnimation(KnockAway);
			else
				EnterNewAnimation(HitstunAir);
		}
		else if (!CurrentState.bIsAirborne) //ground only hit states
		{
			if (CurrentState.CharacterHitState & CanCrumple || CurrentState.Health == 0)
				EnterNewAnimation(Crumple);
			else if (CurrentState.CharacterHitState & CanStagger)
			{
				if (CurrentState.HitStun < 36)
					CurrentState.HitStun = 36;
				EnterNewAnimation(Stagger);
			}
			else if (CurrentState.bIsCrouching)
			{
				if (CurrentState.CharacterHitState & IsHeavy)
					EnterNewAnimation(HitCHeavyIn);
				else
					EnterNewAnimation(HitCIn);
			}
			else
			{
				if (CurrentState.CharacterHitState & LowerBodyHit)
				{
					if (CurrentState.CharacterHitState & IsHeavy)
						EnterNewAnimation(HitSLHeavyIn);
					else
						EnterNewAnimation(HitSLIn);
				}
				else
				{
					if (CurrentState.CharacterHitState & IsHeavy)
						EnterNewAnimation(HitSHHeavyIn);
					else
						EnterNewAnimation(HitSHIn);
				}
			}
		}

		//place and play hit effect
		//place at midpoint between hitbox center and hurtbox center
		if (!(CurrentState.CharacterHitState & NoHitEffect))
		{

		}
	}
}

void ABTCharacterBase::HitboxViewer()
{
	//Pushbox
	if (CurrentState.CurrentAnimFrame.Invincibility != Intangible)
	{
		float PushboxBottom = GetActorLocation().Z;
		float PushBoxHeight = StandingPushBoxHeight;
		if (CurrentState.bIsCrouching || CurrentState.bIsAirborne)
			PushBoxHeight = CrouchingPushBoxHeight;
		if (CurrentState.bIsAirborne)
			PushboxBottom += AirPushboxVerticalOffset;
		//Left side
		DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom),
			FVector(GetActorLocation().X - .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
		//bottom side
		DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom),
			FVector(GetActorLocation().X + .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom), FColor(255, 255, 0), false, 0, 0, .5f);
		//right side
		DrawDebugLine(GetWorld(), FVector(GetActorLocation().X + .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom),
			FVector(GetActorLocation().X + .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
		//top side
		DrawDebugLine(GetWorld(), FVector(GetActorLocation().X - .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom + PushBoxHeight),
			FVector(GetActorLocation().X + .5f * PushboxWidth, GetActorLocation().Y + 35, PushboxBottom + PushBoxHeight), FColor(255, 255, 0), false, 0, 0, .5f);
	}

	if (CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
	{
		for (int32 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num(); i++)
		{
			DrawHitbox(CurrentState.CurrentAnimFrame.Hitboxes[i]);
		}
	}

	if (CurrentState.CurrentAnimFrame.Hurtboxes.Num() > 0)
	{
		for (int32 i = 0; i < CurrentState.CurrentAnimFrame.Hurtboxes.Num(); i++)
		{
			DrawHurtbox(CurrentState.CurrentAnimFrame.Hurtboxes[i]);
		}
	}
}

void ABTCharacterBase::DrawHitbox(FHitbox Box)
{
	FVector BoxCenter = FVector(GetActorLocation().X + Box.Position.X, GetActorLocation().Y, GetActorLocation().Z + Box.Position.Y);

	if (!CurrentState.bFacingRight)
	{
		BoxCenter.X = GetActorLocation().X - Box.Position.X;
	}
	//Left side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Bottom side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Right side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
	//Top side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y), FColor(255, 0, 0), false, 0, 0, .5f);
}

void ABTCharacterBase::DrawHurtbox(FHurtbox Box)
{
	FVector BoxCenter = FVector(GetActorLocation().X + Box.Position.X, GetActorLocation().Y, GetActorLocation().Z + Box.Position.Y);
	if (!CurrentState.bFacingRight)
	{
		BoxCenter.X = GetActorLocation().X - Box.Position.X;
	}
	//Left side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Bottom side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Right side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z - .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
	//Top side
	DrawDebugLine(GetWorld(), FVector(BoxCenter.X - .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y),
		FVector(BoxCenter.X + .5f * Box.Size.X, BoxCenter.Y, BoxCenter.Z + .5f * Box.Size.Y), FColor(0, 255, 0), false, 0, 0, .5f);
}
