// Copyright 2021 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterBase.h"

// Sets default values
ABTCharacterBase::ABTCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABTCharacterBase::BeginPlay()
{
	
}

// Called every frame
void ABTCharacterBase::Tick(float DeltaTime)
{

}

void ABTCharacterBase::HitDetection()
{
	//HitDetection is done at the beginning of every frame, hit detection can also cause animation transitions
	/* Check inputs and add them to InputQueue	
	if (HitStop == 0)
	{
		Process inputs from n frames ago (n = frame delay), will need to update/look at AvailableActions
		Checking PosePlayTime < PlayDuration, Changing Animations, and Anim Transitions
		ProcessAnimationFrame();
	} */
}

void ABTCharacterBase::UpdateCharacter()
{
	if (LandingLag > 0) //landing lag system
	{
		if (LandingLag > 4)
		{
			bCounterHitState = true;
			bIsCrouching = true;

			if (AvailableActions & AcceptBlitz)
				AvailableActions = AcceptBlitz + AcceptSuper;
			else
				AvailableActions = None;
		}
		else //universal four frames of recovery upon landing on feet that only restricts movement
		{
			bCounterHitState = false;
			AvailableActions = AcceptAll - (AcceptMove + AcceptJump);
		}

		if (!bIsAirborne)
		{
			LandingLag--;
		}
	}
}

void ABTCharacterBase::VelocitySolver()
{
	if (Opponent != NULL)
	{
		if (bTouchingOpponent)
		{
			if (bIsRunning && Opponent->bIsRunning)
			{
				if (FMath::Abs(Velocity.X) > FMath::Abs(Opponent->Velocity.X))
				{
					if (bFacingRight)
						Velocity.X = FMath::Abs(Velocity.X) - FMath::Abs(Opponent->Velocity.X);
					else
						Velocity.X = -(FMath::Abs(Velocity.X) - FMath::Abs(Opponent->Velocity.X));
				}
				else if (FMath::Abs(Velocity.X) < FMath::Abs(Opponent->Velocity.X))
				{
					if (bFacingRight)
						Velocity.X = -(FMath::Abs(Opponent->Velocity.X) - FMath::Abs(Velocity.X));
					else
						Velocity.X = (FMath::Abs(Opponent->Velocity.X) - FMath::Abs(Velocity.X));
				}
				else
				{
					Velocity.X = 0;
				}
			}
			else if (bIsRunning) //opponent provides resistance when dashing against them
				Velocity.X *= .85f;
			else if ((bFacingRight && Velocity.X > 0 && Opponent->Velocity.X < 0) || (!bFacingRight && Velocity.X < 0 && Opponent->Velocity.X > 0))
				Velocity.X = 0;

			Opponent->Velocity.X = Velocity.X;
		}
	}
}

void ABTCharacterBase::UpdatePosition()
{
	if (HitStop == 0)
	{
		if (KnockBack != FVector2D(0, 0)) //apply any knockback
		{
			Velocity = FVector2D(Weight * KnockBack.X, Weight * ComboGravity * KnockBack.Y);

			if (bFacingRight)
			{
				Velocity *= FVector2D(-1, 1);
			}

			KnockBack = FVector2D(0, 0);
		}

		if (SlowMoTime % 2 == 0) //animation speed is halved and stun values decrease at half speed while in slow motion
		{
			if (ShatteredTime % 2 == 0)
				PosePlayTime++;
			if (HitStun > 0)
				HitStun--;
			if (BlockStun > 0)
				BlockStun--;
		}

		if (bIsAirborne && GravDefyTime == 0) //apply gravity while character is airborne and not defying gravity
		{
			float GravCalc = -10.f / 60.f; // GravityScale * -10.f / 60.f;

			if (ShatteredTime > 0)
				GravCalc *= .8f;
			if (WallStickTime > 0)
				GravCalc *= .25f;
			if (SlowMoTime > 0)
				GravCalc *= .5f;

			Velocity.Y += GravCalc;
		}
		if (!bIsAirborne && Velocity.Y > 0)
			bIsAirborne = true;

		if (WallStickTime > 0)
			WallStickTime--;

		if (SlowMoTime > 0)
			Position += Velocity * 100 / 120.f;
		else
			Position += Velocity * 100 / 60.f;

		SurfaceContact();

		if (GravDefyTime > 0)
			GravDefyTime--;
	}
	else if (HitStop > 0)
	{
		HitStop--;
	}

	if (ShatteredTime > 0)
		ShatteredTime--;
	if (SlowMoTime > 0)
		SlowMoTime--;
}

void ABTCharacterBase::DrawCharacter()
{
	if (CurrentAnimFrame != NULL && CurrentAnimFrame->Pose != NULL)
		BaseMesh->SetAnimation(CurrentAnimFrame->Pose);

	if (!bFacingRight && BaseMesh->GetRelativeScale3D().X != -1)
	{
		BaseMesh->SetRelativeScale3D(FVector(-1, 1, 1));
	}
	else if (bFacingRight && BaseMesh->GetRelativeScale3D().X != 1)
	{
		BaseMesh->SetRelativeScale3D(FVector(1, 1, 1));
	}

	SetActorLocation(FVector(Position.X, GetActorLocation().Y, Position.Y));
}

void ABTCharacterBase::ProcessAnimationFrame()
{
	if (CurrentAnimFrame != NULL)
	{
		if (PosePlayTime == 0)
		{
			//copy values stored in Animation data to character, these can be overridden by other functions based on character's current circumstances
			bIsCrouching = CurrentAnimFrame->bIsCrouching;
			bArmorActive = CurrentAnimFrame->bArmorActive;
			bCounterHitState = CurrentAnimFrame->bCounterHitState;

			AvailableActions = CurrentAnimFrame->AvailableActions;

			CurrentHitbox = &CurrentAnimFrame->Hitboxes;
			CurrentUpperBody = &CurrentAnimFrame->HurtboxUpperBody;
			CurrentLowerBody = &CurrentAnimFrame->HurtboxLowerBody;

			if (CurrentHitbox->Num() > 0)
				if ((*CurrentHitbox)[0].bNewHit)
					bAttackMadeContact = false;

			if (CurrentAnimFrame->Acceleration != FVector2D(0, 0) || CurrentAnimFrame->bStopVelocityY || CurrentAnimFrame->bStopVelocityX)
			{
				if (bFacingRight)
				{
					Velocity = FVector2D((!CurrentAnimFrame->bStopVelocityX * Velocity.X) + CurrentAnimFrame->Acceleration.X, 
						(!CurrentAnimFrame->bStopVelocityY * Velocity.Y) + CurrentAnimFrame->Acceleration.Y);
				}
				else
				{
					Velocity = FVector2D((!CurrentAnimFrame->bStopVelocityX * Velocity.X) - CurrentAnimFrame->Acceleration.X,
						(!CurrentAnimFrame->bStopVelocityY * Velocity.Y) + CurrentAnimFrame->Acceleration.Y);
				}
			}
			if (bIsAirborne && CurrentAnimFrame->LandingLag > 0)
				LandingLag = CurrentAnimFrame->LandingLag + 4;
		}
		//animation events
	}
}

bool ABTCharacterBase::EnterNewAnimation(TArray<FAnimationFrame> Animation, int32 FrameNumber)
{
	//check if the specified animation and frame exist
	if (Animation.Num() > FrameNumber)
	{
		if (Animation[FrameNumber].Pose != NULL)
		{
			PosePlayTime = 0; // reset pose play time to make sure new frame is played for the correct amount of time
			CurrentAnimation = &Animation;
			CurrentAnimFrame = &Animation[FrameNumber];
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void ABTCharacterBase::TurnAroundCheck()
{
	if (Opponent != NULL)
	{
		if (Opponent->Position.X < Position.X && bFacingRight)
		{
			bFacingRight = false;
			TriggerTurnAround();
		}
		else if (Opponent->Position.X > Position.X && !bFacingRight)
		{
			bFacingRight = true;
			TriggerTurnAround();
		}
	}
}

void ABTCharacterBase::TriggerTurnAround()
{
	//Trigger turnaround animation if in idle stand or crouch;
	if (AvailableActions & AcceptMove && !bIsAirborne && Velocity.X == 0)
	{
		PosePlayTime = 0;
		if (bIsCrouching)
		{
			EnterNewAnimation(TurnAroundCrouch);
		}
		else if (TurnAroundStand.Num() > 0)
		{
			EnterNewAnimation(TurnAroundStand);
		}
	}
}

void ABTCharacterBase::SurfaceContact()
{
	if (bIsAirborne && Position.Y <= 0 && Velocity.Y <= 0 && CurrentAnimFrame != NULL) //landing on the ground
	{
		Position.Y = 0;
		//logic for setting character's state when they hit the ground (land on feet, knockdown, groundbounce)
		if (CharacterHitState & GroundBounce)
		{
			//cause ground bounce and ground bounce animation
			CharacterHitState -= GroundBounce;
		}
		else
		{
			Velocity.Y = 0;
			bIsAirborne = false;
			PosePlayTime = 0;
			if (CurrentAnimFrame->Invincibility != FaceDown) //cause hard knockdown if in a hitstun state
			{
				EnterNewAnimation(KnockDownFaceDown);
			}
			else if (CurrentAnimFrame->Invincibility != FaceUp)
			{
				EnterNewAnimation(KnockDownFaceUp);
			}
			else //if a character is not in a hit state, they will land on their feet
			{
				if (LandingLag > 4) //landing lag puts character in a recovery state when landing
				{
					EnterNewAnimation(CrouchDown);
					bIsCrouching = true;
				}
				else
				{
					EnterNewAnimation(StandUp);
					LandingLag = 4;
				}
			}
		}
	}
	
	//Check if hitting wall, 20 meters is the tentative stage length, subject to change
	if (Position.X <= -10 + .5f * PushboxWidth)
	{
		Position.X = -10 + .5f * PushboxWidth;
		HitWall();
	}
	else if (Position.X >= 10 - .5f * PushboxWidth)
	{
		Position.X = 10 - .5f * PushboxWidth;
		HitWall();
	}
	else
	{
		bTouchingWall = false;
	}
}

void ABTCharacterBase::HitWall()
{
	bTouchingWall = true;
	if (CharacterHitState & WallStick)
	{
		//cause wall stick
		WallStickTime = 30;
		CharacterHitState = None;
		//minimum Position.Y
	}
	else if (CharacterHitState & WallBounce)
	{
		//cause wall bounce and wall bounce animation
		CharacterHitState -= WallBounce;
		//reverse Velocity.X and minimum Velocity.X
		//minimum Velocity.Y
		return;
	}

	Velocity.X = 0;
}

void ABTCharacterBase::Guarding()
{
	if (AvailableActions & AcceptGuard)
	{
		if (!bIsAirborne && (Dir1 >= InputTime - 1 || Dir2 >= InputTime - 1 || Dir3 >= InputTime - 1))
		{
			//on the ground and holding a downward direction means the character is crouching
			bIsCrouching = true;
		}

		if (Dir7 >= InputTime - 1 || Dir4 >= InputTime - 1 || Dir1 >= InputTime - 1)
		{
			//holding a backward direction while able to guard keeps the character's guard up
			bIsGuarding = true;
		}
	}
}

void ABTCharacterBase::PushboxSolver() //only called once per gamestate tick after updating characters, do not call on multiple characters
{
	if (Opponent != NULL)
	{
		if (CurrentAnimFrame != NULL && Opponent->CurrentAnimFrame != NULL)
		{
			if (!CurrentAnimFrame->bCinematic && !Opponent->CurrentAnimFrame->bCinematic)
			{
				if (CurrentAnimFrame->Invincibility != Intangible && Opponent->CurrentAnimFrame->Invincibility != Intangible &&
					FMath::Abs(Opponent->Position.X - Position.X) <= .5f * Opponent->PushboxWidth + .5f * PushboxWidth)
				{
					if (!bIsAirborne && !Opponent->bIsAirborne) //both on the ground
					{
						if (bTouchingWall)
						{
							if (bFacingRight)
								Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						else if (Opponent->bTouchingWall)
						{
							if (bFacingRight)
								Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							else
								Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
						}
						//when character is faster than opponent, and both are moving in the same direction or towards each other
						else if (FMath::Abs(Velocity.X) > FMath::Abs(Opponent->Velocity.X) && ((Velocity.X < 0 && Opponent->Velocity.X < 0) || (Velocity.X > 0 && Opponent->Velocity.X > 0) || (!bFacingRight && Velocity.X < 0 && Opponent->Velocity.X > 0) || (bFacingRight && Velocity.X > 0 && Opponent->Velocity.X < 0)))
						{
							if (bFacingRight)
							{
								Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X >= 10 - .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Velocity.X = 0;
								}
							}
							else
							{
								Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X <= -10 + .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Velocity.X = 0;
								}
							}
						}
						//when opponent is faster than character, and both are moving in the same direction or towards each other
						else if (FMath::Abs(Velocity.X) < FMath::Abs(Opponent->Velocity.X) && ((Velocity.X < 0 && Opponent->Velocity.X < 0) || (Velocity.X > 0 && Opponent->Velocity.X > 0) || (!bFacingRight && Velocity.X < 0 && Opponent->Velocity.X > 0) || (bFacingRight && Velocity.X > 0 && Opponent->Velocity.X < 0)))
						{
							if (bFacingRight)
							{
								Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X <= -10 + .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = -10 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Velocity.X = 0;
								}
							}
							else
							{
								Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X >= 10 - .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = 10 - .5f * PushboxWidth;
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Velocity.X = 0;
								}
							}
						}
						else //if both characters have the same velocity or arent moving
						{
							float MoveDistance = .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth - FMath::Abs(Opponent->Position.X - Position.X));

							if (bFacingRight)
							{
								Position.X -= MoveDistance;
								Opponent->Position.X += MoveDistance;
							}
							else
							{
								Position.X += MoveDistance;
								Opponent->Position.X -= MoveDistance;
							}
						}
						bTouchingOpponent = true;
						Opponent->bTouchingOpponent = true;
					}
					else if (!bIsAirborne && Opponent->bIsAirborne && Opponent->Velocity.Y < 0)
					{
						if ((bIsCrouching && CrouchingPushBoxHeight > Opponent->Position.Y + Opponent->AirPushboxVerticalOffset) ||
							(!bIsCrouching && StandingPushBoxHeight > Opponent->Position.Y + Opponent->AirPushboxVerticalOffset)) //check if pushboxes intersect
						{
							if (bTouchingWall)
							{
								if (bFacingRight)
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (Opponent->bTouchingWall)
							{
								if (bFacingRight)
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (Opponent->Position.X < Position.X)
							{
								Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X <= -10 + .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Opponent->Position.X > Position.X)
							{
								Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X >= 10 - .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = 10 - .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else
							{
								if (Opponent->bFacingRight)
								{
									Opponent->Position.X = Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Opponent->Position.X >= 10 - .5f * Opponent->PushboxWidth)
									{
										Opponent->bTouchingWall = true;
										Opponent->Velocity.X = 0;
										Opponent->Position.X = 10 - .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Position.X <= -10 + .5f * PushboxWidth)
									{
										bTouchingWall = true;
										Velocity.X = 0;
										Position.X = -10 + .5f * PushboxWidth;
										Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}

								}
								else
								{
									Opponent->Position.X = Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Opponent->Position.X <= -10 + .5f * Opponent->PushboxWidth)
									{
										Opponent->bTouchingWall = true;
										Opponent->Velocity.X = 0;
										Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Position.X >= 10 - .5f * PushboxWidth)
									{
										bTouchingWall = true;
										Velocity.X = 0;
										Position.X = 10 - .5f * PushboxWidth;
										Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
								}
							}
							bTouchingOpponent = true;
							Opponent->bTouchingOpponent = true;
						}
					}
					else if (bIsAirborne && !Opponent->bIsAirborne && Velocity.Y < 0)
					{
						if ((Opponent->bIsCrouching && Opponent->CrouchingPushBoxHeight > Position.Y + AirPushboxVerticalOffset) ||
							(!Opponent->bIsCrouching && Opponent->StandingPushBoxHeight > Position.Y + AirPushboxVerticalOffset)) //check if pushboxes intersect
						{
							if (Opponent->bTouchingWall)
							{
								if (Opponent->bFacingRight)
									Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (bTouchingWall)
							{
								if (Opponent->bFacingRight)
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (Position.X < Opponent->Position.X)
							{
								Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X <= -10 + .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = -10 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Position.X > Opponent->Position.X)
							{
								Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X >= 10 - .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = 10 - .5f * PushboxWidth;
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else
							{
								if (bFacingRight)
								{
									Position.X = Opponent->Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Position.X >= 10 - .5f * PushboxWidth)
									{
										bTouchingWall = true;
										Velocity.X = 0;
										Position.X = 10 - .5f * PushboxWidth;
										Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Opponent->Position.X <= -10 + .5f * PushboxWidth)
									{
										Opponent->bTouchingWall = true;
										Opponent->Velocity.X = 0;
										Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}

								}
								else
								{
									Position.X = Opponent->Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Position.X <= -10 + .5f * PushboxWidth)
									{
										bTouchingWall = true;
										Velocity.X = 0;
										Position.X = -10 + .5f * PushboxWidth;
										Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Opponent->Position.X >= 10 - .5f * Opponent->PushboxWidth)
									{
										Opponent->bTouchingWall = true;
										Opponent->Velocity.X = 0;
										Opponent->Position.X = 10 - .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
								}
							}
							bTouchingOpponent = true;
							Opponent->bTouchingOpponent = true;
						}
					}
					else //if both characters are in the air
					{
						if ((Opponent->Position.Y < Position.Y &&
							(Position.Y + AirPushboxVerticalOffset) - (Opponent->Position.Y + Opponent->AirPushboxVerticalOffset) <= Opponent->CrouchingPushBoxHeight) ||
							(Opponent->Position.Y > Position.Y &&
								(Opponent->Position.Y + Opponent->AirPushboxVerticalOffset) - (Position.Y + AirPushboxVerticalOffset) <= CrouchingPushBoxHeight) ||
							Position.Y + AirPushboxVerticalOffset == Opponent->Position.Y + Opponent->AirPushboxVerticalOffset)
						{
							if (bTouchingWall)
							{
								if (bFacingRight)
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (Opponent->bTouchingWall)
							{
								if (Opponent->bFacingRight)
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								else
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
							}
							else if (Opponent->Position.X > Position.X)
							{
								float CenterPoint = .5f * (Position.X + Opponent->Position.X);
								Opponent->Position.X = CenterPoint + .5f * (Opponent->PushboxWidth);
								Position.X = CenterPoint - .5f * (PushboxWidth);

								if (Opponent->Position.X >= 10 - .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = 10 - .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Position.X <= -10 + .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = -10 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Opponent->Position.X < Position.X)
							{
								float CenterPoint = .5f * (Position.X + Opponent->Position.X);
								Opponent->Position.X = CenterPoint - .5f * (Opponent->PushboxWidth);
								Position.X = CenterPoint + .5f * (PushboxWidth);

								if (Opponent->Position.X <= -10 + .5f * Opponent->PushboxWidth)
								{
									Opponent->bTouchingWall = true;
									Opponent->Velocity.X = 0;
									Opponent->Position.X = -10 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Position.X >= 10 - .5f * PushboxWidth)
								{
									bTouchingWall = true;
									Velocity.X = 0;
									Position.X = 10 - .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							bTouchingOpponent = true;
							Opponent->bTouchingOpponent = true;
						}
					}
				}
				else
				{
					bTouchingOpponent = false;
					Opponent->bTouchingOpponent = false;
				}
			}
		}
	}
}

