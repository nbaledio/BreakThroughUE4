// Copyright 2021 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterBase.h"

// Sets default values
ABTCharacterBase::ABTCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; //false; most likely will update this actor from gamestate

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	CharacterVoice = CreateDefaultSubobject<UAudioComponent>(TEXT("Character Voice"));
	CharacterVoice->SetupAttachment(RootComponent);

	CharacterSoundEffects = CreateDefaultSubobject<UAudioComponent>(TEXT("Character SFX"));
	CharacterSoundEffects->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABTCharacterBase::BeginPlay()
{
	EnterNewAnimation(IdleStand);
}

// Called every frame
void ABTCharacterBase::Tick(float DeltaTime)
{

}

void ABTCharacterBase::SuperFlashSolver() //Only play once from Player1
{
	if (Opponent != NULL) //Keeps game from freezing or slowing down if both characters super flash on the exact same frame, Player 1 Anim will play first
	{
		if ((CurrentAnimFrame->bSuperFlash && Opponent->CurrentAnimFrame->bSuperFlash) || (CurrentAnimFrame->bSuperFlash))
		{
			Opponent->HitStop++;
		}
		else if (Opponent->CurrentAnimFrame->bSuperFlash)
		{
			HitStop++;
		}
	}
}

void ABTCharacterBase::HitDetection()
{
	if (Opponent != NULL && !bClash)
	{
		bHitSuccess = false;
		if (bBlitzing)
		{
			if (FMath::Sqrt(FMath::Square(Position.X - Opponent->Position.X) + FMath::Square(Position.Y - Opponent->Position.Y)) < 250)
			{
				if (Opponent->Resolute)
				{
					Opponent->Durability += 50;
					//Play UI Resolute signal
				}
				else if (CurrentAnimation == &BreakerBlitz)
				{
					//Similar to normal hit behavior based on whether the opponent is guarding or not
					if (Opponent->bIsGuarding)
					{
						//set opponent's animation to guarding
						//guarding logic
					}
					else
					{
						Opponent->KnockBack = FVector2D(2, 0);
						Opponent->SlowMoTime = 30; //Slows opponent for .5 seconds
						Opponent->EnterNewAnimation(Opponent->Deflected);
					}
				}
				else
				{
					if (Opponent->ShatteredTime % 2 == 0)
						Opponent->SlowMoTime = 60; //Slows opponent for one second
					else
						Opponent->SlowMoTime = 61; //Slows opponent for one second
					//Blitz Afterimage effect
				}
			}
			bBlitzing = false;
		}
		//only look for hits if there are hitboxes active, and the current hitbox has not hit anything previously
		else if (CurrentHitbox->Num() > 0 && !bAttackMadeContact) 
		{
			if ((*CurrentHitbox)[0].AttackHeight < Throw) //Current active attack is a strike
			{
				if (Opponent->CurrentHitbox->Num() > 0 && !Opponent->bAttackMadeContact) //only look for clashes if the opponent has an active attack out
				{
					//loop through opponent's active hitboxes
					for (int32 i = 0; i < CurrentHitbox->Num() && !bAttackMadeContact; i++)
					{
						FVector2D HitboxCenter;
						if (bFacingRight)
							HitboxCenter = Position + (*CurrentHitbox)[i].Position;
						else
							HitboxCenter = FVector2D(Position.X - (*CurrentHitbox)[i].Position.X, Position.Y + (*CurrentHitbox)[i].Position.Y);

						for (int32 j = 0; j < Opponent->CurrentHitbox->Num() && !bAttackMadeContact; j++)
						{
							FVector2D OpponentHitboxCenter;
							if (Opponent->bFacingRight)
								OpponentHitboxCenter = Opponent->Position + (*Opponent->CurrentHitbox)[j].Position;
							else
								OpponentHitboxCenter = FVector2D(Opponent->Position.X - (*Opponent->CurrentHitbox)[j].Position.X, Opponent->Position.Y + (*Opponent->CurrentHitbox)[j].Position.Y);

							if (RectangleOverlap(HitboxCenter, OpponentHitboxCenter, (*CurrentHitbox)[i].Size, (*Opponent->CurrentHitbox)[j].Size))
							{
								bClash = true;
								Opponent->bClash = true;
								bAttackMadeContact = true;
								Opponent->bAttackMadeContact = true;
								//attacks with the deflect can deflect opponents' non-super, non-deflect attacks
								if ((*CurrentHitbox)[0].AttackProperties & CanDeflect && !((*Opponent->CurrentHitbox)[0].AttackProperties & CanDeflect) && !((*Opponent->CurrentHitbox)[0].AttackProperties & IsSuper))
								{
									AvailableActions = AcceptAll;
									Opponent->AvailableActions = None;
									Opponent->EnterNewAnimation(Opponent->Deflected);
									HitStop = 9;
									Opponent->HitStop = 9;			
								}
								//opponent's attack can deflect
								else if (!((*CurrentHitbox)[0].AttackProperties & CanDeflect) && (*Opponent->CurrentHitbox)[0].AttackProperties & CanDeflect && !((*CurrentHitbox)[0].AttackProperties & IsSuper))
								{
									Opponent->AvailableActions = AcceptAll;
									AvailableActions = None;
									EnterNewAnimation(Deflected);
									HitStop = 9;
									Opponent->HitStop = 9;
								}
								else //otherwise normal clash
								{
									HitStop = 15;
									Opponent->HitStop = 15;
									AvailableActions = AcceptAll;
									Opponent->AvailableActions = AcceptAll;
									//play clash effect
								}
							}
						}
					}
				}
				//loop through opponent's active hurtboxes and see if any current hitboxes overlap them
				if (Opponent->CurrentAnimFrame->Invincibility != StrikeInvincible && Opponent->CurrentAnimFrame->Invincibility != FullInvincible &&
					!((*CurrentHitbox)[0].AttackHeight == High && Opponent->bIsCrouching) && !bAttackMadeContact)
				{
					if (Opponent->CurrentHurtbox->Num() > 0)
					{
						for (int32 i = 0; i < CurrentHitbox->Num() && !bAttackMadeContact; i++)
						{
							FVector2D HitboxCenter;
							if (bFacingRight)
								HitboxCenter = Position + (*CurrentHitbox)[i].Position;
							else
								HitboxCenter = FVector2D(Position.X - (*CurrentHitbox)[i].Position.X, Position.Y + (*CurrentHitbox)[i].Position.Y);

							for (int32 j = 0; j < Opponent->CurrentHurtbox->Num() && !bAttackMadeContact; j++)
							{
								FVector2D OpponentHurtboxCenter;
								if (Opponent->bFacingRight)
									OpponentHurtboxCenter = Opponent->Position + (*Opponent->CurrentHurtbox)[j].Position;
								else
									OpponentHurtboxCenter = FVector2D(Opponent->Position.X - (*Opponent->CurrentHurtbox)[j].Position.X, Opponent->Position.Y + (*Opponent->CurrentHurtbox)[j].Position.Y);

								if (RectangleOverlap(HitboxCenter, OpponentHurtboxCenter, (*CurrentHitbox)[i].Size, (*Opponent->CurrentHurtbox)[j].Size))
								{
									bAttackMadeContact = true;
									if (Opponent->CurrentAnimFrame->Invincibility == SuperCounter || (((Opponent->CurrentAnimFrame->Invincibility == AllCounter && (*CurrentHitbox)[i].AttackHeight != Unblockable) || 
										(Opponent->CurrentAnimFrame->Invincibility == HiCounter && (*CurrentHitbox)[i].AttackHeight < Low) || (Opponent->CurrentAnimFrame->Invincibility == LowCounter && (*CurrentHitbox)[i].AttackHeight == Low)) && 
										!((*CurrentHitbox)[i].AttackProperties & IsSuper))) //check if the opponent is in a counter stance that can counter the current attack
									{
										HitStop = 24;
										Opponent->bHitSuccess = true;
										Opponent->bClash = true;
									}
									else
										ContactHit((*CurrentHitbox)[i], OpponentHurtboxCenter);
								}
							}
						}
					}
				}
			}
			//logic to follow for throws
			else if (((((*CurrentHitbox)[0].AttackHeight == CommandThrow || (*CurrentHitbox)[0].AttackHeight == Throw) && !Opponent->bIsAirborne) || 
				(((*CurrentHitbox)[0].AttackHeight == AirCommandThrow || (*CurrentHitbox)[0].AttackHeight == AirThrow) && Opponent->bIsAirborne)) &&
				((Opponent->HitStun == 0 && Opponent->BlockStun == 0) || (*CurrentHitbox)[0].AttackProperties & ComboThrow || Opponent->CurrentAnimation == &Opponent->Stagger ||
				Opponent->CurrentAnimation == &Opponent->Crumple) && Opponent->CurrentAnimFrame->Invincibility != ThrowInvincible &&
				Opponent->CurrentAnimFrame->Invincibility != FullInvincible && Opponent->CurrentAnimFrame->Invincibility != Intangible && Opponent->CurrentAnimFrame->Invincibility != OTG)
			{
				for (int32 i = 0; i < CurrentHitbox->Num() && !bAttackMadeContact; i++)
				{
					FVector2D HitboxCenter;
					if (bFacingRight)
						HitboxCenter = Position + (*CurrentHitbox)[i].Position;
					else
						HitboxCenter = FVector2D(Position.X - (*CurrentHitbox)[i].Position.X, Position.Y + (*CurrentHitbox)[i].Position.Y);

					FVector2D OpponentPushboxCenter = FVector2D(Opponent->Position.X, Opponent->Position.Y);
					FVector2D OpponentPushboxSize = FVector2D(Opponent->PushboxWidth, Opponent->StandingPushBoxHeight);

					if (Opponent->bIsAirborne)
					{
						OpponentPushboxCenter.Y += .5f * Opponent->CrouchingPushBoxHeight + Opponent->AirPushboxVerticalOffset;
						OpponentPushboxSize.Y = Opponent->CrouchingPushBoxHeight;
					}
					else if (Opponent->bIsCrouching)
					{
						OpponentPushboxCenter.Y += .5f * Opponent->CrouchingPushBoxHeight;
						OpponentPushboxSize.Y = Opponent->CrouchingPushBoxHeight;
					}
					else
						OpponentPushboxCenter.Y += .5f * Opponent->StandingPushBoxHeight;

					if (RectangleOverlap(HitboxCenter, OpponentPushboxCenter, (*CurrentHitbox)[i].Size, OpponentPushboxSize))
					{
						bAttackMadeContact = true;
						ContactThrow((*CurrentHitbox)[i], (*CurrentHitbox)[0].AttackHeight); 
					}
				}
			}
		}
	}
	bClash = false;
	
	//HitDetection is done at the beginning of every frame, hit detection can also cause animation transitions to hitstun states
}

void ABTCharacterBase::UpdateCharacter(int32 CurrentInputs)
{
	//Check inputs and add them to InputQueue
	//Process inputs from n frames ago (n = frame delay), 

	ProcessInputs(CurrentInputs); //(Inputs[GameState->FrameCount - FrameDelay]);

	if (HitStop == 0)
	{
		// update/look at AvailableActions
		LandingLagCheck();
		RunBraking();

		if (HitStun > 0)
		{
			AvailableActions = None;
		}
		else if (BlockStun > 0)
			AvailableActions = AcceptGuard;

		if (AvailableActions & AcceptMove)
		{
			if (!bIsAirborne)
				TurnAroundCheck();
		}
			

		//Checking PosePlayTime < PlayDuration, Changing Animations, and Anim Transitions based on read inputs
		AnimationStateMachine();
		
		ProcessAnimationFrame();

		Guarding();
		GravityCalculation();
		ApplyKnockBack();

		if (AvailableActions & AcceptMove && CurrentInputs == 0) //Character is not inputting anything while in neutral
			Resolute = true; //Character with Resolute state will gain meter instead of being hit by a Blitz wave and will automatically counter normal throws
	}
	SetSounds();
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
			else if (Opponent->bIsRunning) //provide resistance against dashing opponent
				Opponent->Velocity.X *= .85f;
			else if ((bFacingRight && Velocity.X > 0 && Opponent->Velocity.X < 0) || (!bFacingRight && Velocity.X < 0 && Opponent->Velocity.X > 0) && 
				AvailableActions & AcceptMove && Opponent->AvailableActions & AcceptMove) //both characters stop moving if walking against each other
			{
				Velocity.X = 0;
				Opponent->Velocity.X = Velocity.X;
			}	
		}
	}
}

void ABTCharacterBase::UpdatePosition() //update character's location based on velocity and decrement certain timed values
{
	if (HitStop == 0)
	{
		if (SlowMoTime % 2 == 0) //animation speed is halved and stun values decrease at half speed while in slow motion
		{
			if (ShatteredTime % 2 == 0 && PosePlayTime < CurrentAnimFrame->PlayDuration)
			{
				PosePlayTime++;
				if (PosePlayTime == CurrentAnimFrame->PlayDuration)
					AnimFrameIndex++;
			}
			if (HitStun > 0 && CurrentAnimation != &WallBounce && CurrentAnimation != &Crumple && CurrentAnimation != &GroundBounce && CurrentAnimation != &Tumble)
				HitStun--;
			if (BlockStun > 0)
				BlockStun--;
		}

		if (WallStickTime > 0)
			WallStickTime--;

		if (!CurrentAnimFrame->bLockPosition)
		{
			if (SlowMoTime > 0)
				Position += Velocity * 100 / 120.f;
			else
				Position += Velocity * 100 / 60.f;
		}

		if (Position.Y <= 0)
			Position.Y = 0;

		if (Position.X <= -1000 + .5f * PushboxWidth)
		{
			Position.X = -1000 + .5f * PushboxWidth;
		}
		else if (Position.X >= 1000 - .5f * PushboxWidth)
		{
			Position.X = 1000 - .5f * PushboxWidth;
		}

		if (GravDefyTime > 0)
			GravDefyTime--;
	}
	else if (HitStop > 0)
	{
		HitStop--;
	}

	if ((HitStun > 0 || CurrentAnimFrame->Invincibility == FaceDown || CurrentAnimFrame->Invincibility == FaceUp) && ShatteredTime == 0)
	{
		if (SlowMoTime % 2 == 0)
			ComboTimer++;
	}
	else
		ComboTimer = 0;

	UpdateResolve();
		
	if (ShatteredTime > 0)
		ShatteredTime--;
	if (SlowMoTime > 0)
		SlowMoTime--;

	if (Opponent != NULL)
	{
		if ((Opponent->HitStun == 0 && !Opponent->bIsAirborne) || (Opponent->bIsAirborne && Opponent->CurrentAnimFrame->Invincibility != FaceDown && Opponent->CurrentAnimFrame->Invincibility != FaceUp))
			ComboCount = 0;
	}	

	InputCountdown();
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

								if (Opponent->Position.X >= 1000 - .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Velocity.X = 0;
								}
							}
							else
							{
								Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X <= -1000 + .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
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

								if (Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									Position.X = -1000 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Velocity.X = 0;
								}
							}
							else
							{
								Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X >= 1000 - .5f * PushboxWidth)
								{
									HitWall();
									Position.X = 1000 - .5f * PushboxWidth;
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

								if (Opponent->Position.X <= -1000 + .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Opponent->Position.X > Position.X)
							{
								Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Opponent->Position.X >= 1000 - .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = 1000 - .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else
							{
								if (Opponent->bFacingRight)
								{
									Opponent->Position.X = Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Opponent->Position.X >= 1000 - .5f * Opponent->PushboxWidth)
									{
										Opponent->HitWall();
										Opponent->Position.X = 1000 - .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Position.X <= -1000 + .5f * PushboxWidth)
									{
										HitWall();
										Position.X = -1000 + .5f * PushboxWidth;
										Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}

								}
								else
								{
									Opponent->Position.X = Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Opponent->Position.X <= -1000 + .5f * Opponent->PushboxWidth)
									{
										Opponent->HitWall();
										Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Position.X >= 1000 - .5f * PushboxWidth)
									{
										HitWall();
										Position.X = 1000 - .5f * PushboxWidth;
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

								if (Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									Position.X = -1000 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Position.X > Opponent->Position.X)
							{
								Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

								if (Position.X >= 1000 - .5f * PushboxWidth)
								{
									HitWall();
									Position.X = 1000 - .5f * PushboxWidth;
									Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else
							{
								if (bFacingRight)
								{
									Position.X = Opponent->Position.X + .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Position.X -= .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Position.X >= 1000 - .5f * PushboxWidth)
									{
										HitWall();
										Position.X = 1000 - .5f * PushboxWidth;
										Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Opponent->Position.X <= -1000 + .5f * PushboxWidth)
									{
										Opponent->HitWall();
										Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
										Position.X = Opponent->Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}

								}
								else
								{
									Position.X = Opponent->Position.X - .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									Opponent->Position.X += .5f * (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);

									if (Position.X <= -1000 + .5f * PushboxWidth)
									{
										HitWall();
										Position.X = -1000 + .5f * PushboxWidth;
										Opponent->Position.X = Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
									}
									else if (Opponent->Position.X >= 1000 - .5f * Opponent->PushboxWidth)
									{
										Opponent->HitWall();
										Opponent->Position.X = 1000 - .5f * Opponent->PushboxWidth;
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
						// Check pushbox intersection
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

								if (Opponent->Position.X >= 1000 - .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = 1000 - .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Position.X <= -1000 + .5f * PushboxWidth)
								{
									HitWall();
									Position.X = -1000 + .5f * PushboxWidth;
									Opponent->Position.X = Position.X + (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
							}
							else if (Opponent->Position.X < Position.X)
							{
								float CenterPoint = .5f * (Position.X + Opponent->Position.X);
								Opponent->Position.X = CenterPoint - .5f * (Opponent->PushboxWidth);
								Position.X = CenterPoint + .5f * (PushboxWidth);

								if (Opponent->Position.X <= -1000 + .5f * Opponent->PushboxWidth)
								{
									Opponent->HitWall();
									Opponent->Position.X = -1000 + .5f * Opponent->PushboxWidth;
									Position.X = Opponent->Position.X - (.5f * Opponent->PushboxWidth + .5f * PushboxWidth);
								}
								else if (Position.X >= 1000 - .5f * PushboxWidth)
								{
									HitWall();
									Position.X = 1000 - .5f * PushboxWidth;
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

	if (PlaySound)
	{
		if (CharacterVoice->Sound != NULL)
			CharacterVoice->Play();
		if (CharacterSoundEffects->Sound != NULL)
			CharacterSoundEffects->Play();

		PlaySound = false;
	}

	//if Hitbox View is on also loop through hitbox and hurtbox arrays and draw to screen
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

			if (LandingLag > 0)
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
				else if (LandingLag <= 4) //universal four frames of recovery upon landing on feet that only restricts movement
				{
					AvailableActions &= AcceptAll - (AcceptMove + AcceptJump);
				}
			}
			else
			{
				if (bAttackMadeContact && AnimFrameIndex > 0)
					AvailableActions |= CurrentAnimFrame->AvailableActions;
				else
					AvailableActions = CurrentAnimFrame->AvailableActions;
			}

			CurrentHitbox = &CurrentAnimFrame->Hitboxes;
			CurrentHurtbox = &CurrentAnimFrame->Hurtboxes;


			if (CurrentHitbox->Num() > 0)
				if ((*CurrentHitbox)[0].bNewHit)
				{
					bAttackMadeContact = false;
				}

			//check if any of the frame's variables pertaining to velocity have stored values, add acceleration if any or halt velocity in certain axes if specified
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
		AnimationEvents(); // perform certain actions based on the current frame of animation
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
			AnimFrameIndex = FrameNumber;
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
		if (!bTouchingWall)
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

bool ABTCharacterBase::SurfaceContact() //Animation transitions that occur when a character touches a surface with certain conditions
{
	if (((bIsAirborne && Position.Y <= 0 && Velocity.Y <= 0) || (CharacterHitState & CanGroundBounce && !bIsAirborne)) && CurrentAnimFrame != NULL) //landing on the ground
	{
		//logic for setting character's state when they hit the ground (land on feet, knockdown, groundbounce)
		if (CharacterHitState & CanGroundBounce)
		{
			//cause ground bounce and ground bounce animation
			CharacterHitState -= CanGroundBounce;
			Velocity.Y *= -1;
			if (Velocity.Y < 3)
				Velocity.Y = 3;
			return EnterNewAnimation(GroundBounce);
		}
		else
		{
			Velocity.Y = 0;
			bIsAirborne = false;
			TurnAroundCheck();
			if (CurrentAnimFrame->Invincibility != FaceDown) //cause hard knockdown if in a hitstun state
			{
				SlowMoTime = 0;
				ShatteredTime = 0;
				HitStun = 0;
				return EnterNewAnimation(KnockDownFaceDown);
			}
			else if (CurrentAnimFrame->Invincibility != FaceUp)
			{
				SlowMoTime = 0;
				ShatteredTime = 0;
				HitStun = 0;
				return EnterNewAnimation(KnockDownFaceUp);
			}
			else //if a character is not in a hit state, they will land on their feet
			{
				if (LandingLag > 4) //landing lag puts character in a recovery state when landing
				{

					if (AvailableActions & AcceptBlitz)
						AvailableActions = AcceptBlitz + AcceptSuper;
					else
						AvailableActions = None;

					return EnterNewAnimation(CrouchDown);
				}
				else
				{
					LandingLag = 4;
					AvailableActions = AcceptAll - (AcceptMove + AcceptJump);

					return EnterNewAnimation(StandUp);
				}
			}
		}
	}
	
	//Check if hitting wall, 20 meters is the tentative stage length, subject to change
	if (Position.X <= -1000 + .5f * PushboxWidth)
	{
		Position.X = -1000 + .5f * PushboxWidth;
		return HitWall();
	}
	else if (Position.X >= 1000 - .5f * PushboxWidth)
	{
		Position.X = 1000 - .5f * PushboxWidth;
		return HitWall();
	}

	bTouchingWall = false;
	return false;
}

bool ABTCharacterBase::HitWall()
{
	bTouchingWall = true;
	if (CharacterHitState & CanWallStick && bIsAirborne)
	{
		//cause wall stick
		WallStickTime = 30;
		CharacterHitState = None;
		//minimum Position.Y
		return EnterNewAnimation(WallStick);
	}
	else if (CharacterHitState & CanWallBounce && bIsAirborne)
	{
		//cause wall bounce and wall bounce animation
		
		CharacterHitState -= CanWallBounce;
		WallStickTime = 3;
		Velocity.X *= -1;
		if (Velocity.Y < 1.5f)
			Velocity.Y = 1.5f;
		return EnterNewAnimation(WallStick);
	}

	Velocity.X = 0;
	return false;
}

void ABTCharacterBase::Guarding()
{
	if (AvailableActions & AcceptGuard)
	{
		RefreshMovelist();
		if (!bIsAirborne && (Dir1 == InputTime || Dir2 == InputTime || Dir3 == InputTime))
		{
			//on the ground and holding a downward direction means the character is crouching
			bIsCrouching = true;
		}

		if (bIsGuarding) //(Dir7 == InputTime || Dir4 >= InputTime || Dir1 == InputTime)
		{
			//holding a backward direction while able to guard keeps the character's guard up
			//bIsGuarding = true;
			JustDefense--; //if a character inputs guard up to five frames before an attack lands, blockstun is reduced by a third and no knockback/pushback is applied
		}
		else
		{
			//bIsGuarding = false;
			JustDefense = 5;
		}
	}
	else
	{
		bIsGuarding = false;
		JustDefense = 5;
	}
}

void ABTCharacterBase::LandingLagCheck()
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
		else if (LandingLag <= 4)
		{
			AvailableActions &= AcceptAll - AcceptMove - AcceptJump;
		}

		if (!bIsAirborne)
		{
			LandingLag--;
		}
	}
}

void ABTCharacterBase::RunBraking()
{
	if (bIsRunning) //accelerate while running
	{
		if (bFacingRight)
		{
			if (Velocity.X < InitRunSpeed)
				Velocity.X = InitRunSpeed;
			else
			{
				if (SlowMoTime > 0)
					Velocity.X += .5f * RunAcceleration;
				else
					Velocity.X += RunAcceleration;
			}

		}
		else
		{
			if (Velocity.X > -InitRunSpeed)
				Velocity.X = -InitRunSpeed;
			else
			{
				if (SlowMoTime > 0)
					Velocity.X -= .5f * RunAcceleration;
				else
					Velocity.X -= RunAcceleration;
			}
		}

		//stop running if forward direction is no longer being held for run type characters and if animation has finished on dash type characters
		if ((RunAcceleration > 0 && Dir6 < InputTime && Dir9 < InputTime) || (RunAcceleration == 0 && AvailableActions & AcceptMove))
		{
			bIsRunning = false;
		}
	}
	else if (!(AvailableActions & AcceptMove) && !bIsRunning && !bIsAirborne) //braking/friction to slow down character when not voluntarily accelerating
	{
		if (FMath::Abs(Velocity.X) > 1.f) // 1 is not necessarily the final value, just for testing
			Velocity.X *= .95f;//test values once more things are put in place
		else
		{
			Velocity.X = 0;
		}
	}
}

void ABTCharacterBase::Jumping()
{
	TurnAroundCheck();
	RefreshMovelist();
	if (!bIsRunning || bBackwardJump || bIsAirborne) //preserve horizontal velocity only if jumping with a running start
		Velocity.X = 0;
	if (bIsAirborne && JumpsUsed == 0)
		JumpsUsed++;

	if (bForwardJump) //if jumping forward
	{
		if (bFacingRight)
			Velocity.X += JumpForce.X;
		else
			Velocity.X -= JumpForce.X;
	}
	else if (bBackwardJump) //if jumping back
	{
		if (bFacingRight)
			Velocity.X -= JumpForce.X;
		else
			Velocity.X += JumpForce.X;
	}

	Velocity.Y = JumpForce.Y;

	if (bIsAirborne)
	{
		Velocity.Y *= .5f;
	}

	AirJump = 0;
	bIsAirborne = true;
	bIsRunning = false;
	bForwardJump = false;
	bBackwardJump = false;
	JumpsUsed++;
}

void ABTCharacterBase::GravityCalculation()
{
	if (bIsAirborne && GravDefyTime == 0 && !CurrentAnimFrame->bLockPosition) //apply gravity while character is airborne and not defying gravity
	{
		float GravCalc = Weight * -10.f / 60.f;

		if (ShatteredTime > 0)
			GravCalc *= .8f;
		if (WallStickTime > 0)
			GravCalc *= .5f;
		if (SlowMoTime > 0)
			GravCalc *= .5f;

		Velocity.Y += GravCalc;
	}
	if (!bIsAirborne && Velocity.Y > 0)
		bIsAirborne = true;
	
}

void ABTCharacterBase::ApplyKnockBack()
{
	float ComboGravity = 1;
	if (Opponent != NULL)
	{
		if (Opponent->ComboCount <= 8 || CharacterHitState & IsSuper || ShatteredTime > 0)
			ComboGravity = 1;
		else if (Opponent->ComboCount <= 12)
			ComboGravity = .98;
		else if (Opponent->ComboCount <= 16)
			ComboGravity = .95;
		else if (Opponent->ComboCount <= 20)
			ComboGravity = .93;
		else if (Opponent->ComboCount <= 24)
			ComboGravity = .9;
		else if (Opponent->ComboCount <= 30)
			ComboGravity = .87;
		else
			ComboGravity = .85f;
	}

	if (KnockBack != FVector2D(0, 0)) //apply any knockback
	{
		if (HitStun > 0 || BlockStun > 0 || CurrentAnimation == &Deflected)
		{
			if (KnockBack.Y > 0)
				Velocity = FVector2D(KnockBack.X, ComboGravity * KnockBack.Y);
			else
				Velocity = KnockBack;
		}
		else
		{
				Velocity.X = KnockBack.X;
				if (KnockBack.Y > 0)
					Velocity.Y = KnockBack.Y;
		}

		if (bFacingRight)
		{
			Velocity *= FVector2D(-1, 1);
		}

		KnockBack = FVector2D(0, 0);
	}
}

void ABTCharacterBase::UpdateResolve()
{
	if (ShatteredTime == 0 && !CurrentAnimFrame->bCinematic && !CurrentAnimFrame->bSuperFlash)
	{
		if (ResolveRecoverTimer >= 180) //Resolve starts passively regenerating after three seconds without being used or broken, resolve regen speeds up if the character is not inputting anything
		{
			if ((float)Health / (float)MaxHealth <= .1f && (SlowMoTime == 0 || (SlowMoTime > 0 && RecoverInterval >= 2)))
			{
				Durability++;
				if (Resolute)
					Durability++;
				RecoverInterval = 0;
			}
			else if ((float)Health / (float)MaxHealth <= .25f && ((SlowMoTime == 0 &&RecoverInterval >= 2)|| (SlowMoTime > 0 && RecoverInterval >= 4) || (SlowMoTime == 0 && Resolute)))
			{
				Durability++;
				RecoverInterval = 0;
			}
			else if ((float)Health / (float)MaxHealth <= .5f && ((SlowMoTime == 0 && RecoverInterval >= 3) || (SlowMoTime > 0 && RecoverInterval >= 6) || (SlowMoTime == 0 && Resolute && RecoverInterval >= 2)))
			{
				Durability++;
				RecoverInterval = 0;
			}
			else if (((SlowMoTime == 0 && RecoverInterval >= 4) || (SlowMoTime > 0 && RecoverInterval >= 8) || (SlowMoTime == 0 && Resolute && RecoverInterval >= 3)))
			{
				Durability++;
				RecoverInterval = 0;
			}
			else
				RecoverInterval++;
		}
		else
			ResolveRecoverTimer++;
	}

	if (Durability > 100)
	{
		if (Resolve < 4)
		{
			Resolve++;
			Durability = 0;
		}
		else
		{
			Resolve = 4;
			Durability = 100;
		}
	}
	else if (Durability <= 0)
	{
		if (Resolve > 0)
		{
			Resolve--;
			Durability = 100;
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
	if (Inputs & INPUT_DOWN)
	{
		Charge2++;
		Charge2Life = InputTime;
	}
	else if (Charge2Life == 0)
		Charge2 = 0;

	if (Inputs & INPUT_UP)
	{
		Charge8++;
		Charge8Life = InputTime;
	}
	else if (Charge8Life == 0)
		Charge8 = 0;

	if ((Inputs & INPUT_LEFT && !bFacingRight) || (Inputs & INPUT_RIGHT && bFacingRight))
	{
		Charge6++;
		Charge6Life = InputTime;
	}
	else if (Charge6Life == 0)
		Charge6 = 0;

	if ((Inputs & INPUT_LEFT && bFacingRight) || (Inputs & INPUT_RIGHT && !bFacingRight))
	{
		Charge4++;
		Charge4Life = InputTime;
	}
	else if (Charge4Life == 0)
		Charge4 = 0;
}

void ABTCharacterBase::DirectionalInputs(int32 Inputs) //set the correct directional inputs based on the inputs read
{
	if (Inputs & INPUT_DOWN)
	{
		if ((bFacingRight && Inputs & INPUT_RIGHT) || (!bFacingRight && Inputs & INPUT_LEFT))
			Dir3 = InputTime;
		else if ((!bFacingRight && Inputs & INPUT_RIGHT) || (bFacingRight && Inputs & INPUT_LEFT))
			Dir1 = InputTime;
		else
		{
			if (Dir2 < InputTime && Dir2 > 0 && DoubleDir2 == 0)
			{
				DoubleDir2 = InputTime;
			}
			Dir2 = InputTime;
		}
	}
	else if (Inputs & INPUT_UP)
	{
		if ((bFacingRight && Inputs & INPUT_RIGHT) || (!bFacingRight && Inputs & INPUT_LEFT))
			Dir9 = InputTime;
		else if ((!bFacingRight && Inputs & INPUT_RIGHT) || (bFacingRight && Inputs & INPUT_LEFT))
			Dir7 = InputTime;
		else
			Dir8 = InputTime;

		if (bIsAirborne)
			AirJump = InputTime;
	}
	else if (Inputs & INPUT_RIGHT)
	{
		if (bFacingRight)
		{
			if (Dir6 < InputTime && Dir6 > 0 && DoubleDir4 == 0)
			{
				DoubleDir6 = InputTime;
			}
			Dir6 = InputTime;
		}
		else
		{
			if (Dir4 < InputTime && Dir4 > 0 && DoubleDir4 == 0)
			{
				DoubleDir4 = InputTime;
			}
			Dir4 = InputTime;
		}
			
	}
	else if (Inputs & INPUT_LEFT)
	{
		if (bFacingRight)
		{
			if (Dir4 < InputTime && Dir4 > 0 && DoubleDir4 == 0)
			{
				DoubleDir4 = InputTime;
			}
			Dir4 = InputTime;
			bIsGuarding = true;
		}
		else
		{
			if (Dir6 < InputTime && Dir6 > 0 && DoubleDir4 == 0)
			{
				DoubleDir6 = InputTime;
			}
			Dir6 = InputTime;
		}
	}

	if (Opponent != NULL)
	{
		if ((Opponent->Position.X < Position.X && Inputs & INPUT_RIGHT) || (Opponent->Position.X > Position.X && Inputs & INPUT_LEFT))
			bIsGuarding = true;
		else if ((!bFacingRight && Inputs & INPUT_RIGHT) || (bFacingRight && Inputs & INPUT_LEFT))
			bIsGuarding = true;
	}
}

void ABTCharacterBase::ButtonInputs(int32 Inputs) //set the correct button inputs based on the inputs read
{
	if (Inputs & INPUT_LIGHT && !bIsLDown) //Light Attack Button
	{
		LPressed = InputTime;
		bIsLDown = true;
	}
	else if (!(Inputs & INPUT_LIGHT) && bIsLDown)
	{
		LReleased = InputTime;
		bIsLDown = false;
	}

	if (Inputs & INPUT_MEDIUM && !bIsMDown) //Medium Attack Button
	{
		MPressed = InputTime;
		bIsMDown = true;
	}
	else if (!(Inputs & INPUT_LIGHT) && bIsMDown)
	{
		MReleased = InputTime;
		bIsMDown = false;
	}

	if (Inputs & INPUT_HEAVY && !bIsHDown) //Heavy Attack Button
	{
		HPressed = InputTime;
		bIsHDown = true;
	}
	else if (!(Inputs & INPUT_HEAVY) && bIsHDown)
	{
		HReleased = InputTime;
		bIsHDown = false;
	}

	if (Inputs & INPUT_BREAK && !bIsBDown) //Break Attack Button
	{
		BPressed = InputTime;
		bIsBDown = true;
	}
	else if (!(Inputs & INPUT_BREAK) && bIsBDown)
	{
		BReleased = InputTime;
		bIsBDown = false;
	}
}

void ABTCharacterBase::InputCountdown() //decrement input values
{
	if (Charge2Life > 0)
		Charge2Life--;
	if (Charge4Life > 0)
		Charge4Life--;
	if (Charge6Life > 0)
		Charge6Life--;
	if (Charge8Life > 0)
		Charge8Life--;

	if (Dir1 > 0)
		Dir1--;
	if (Dir2 > 0)
		Dir2--;
	if (Dir3 > 0)
		Dir3--;
	if (Dir4 > 0)
		Dir4--;
	if (Dir6 > 0)
		Dir6--;
	if (Dir7 > 0)
		Dir7--;
	if (Dir8 > 0)
		Dir8--;
	if (Dir9 > 0)
		Dir9--;

	if (DoubleDir2 > 0)
		DoubleDir2--;
	if (DoubleDir4 > 0)
		DoubleDir4--;
	if (DoubleDir6 > 0)
		DoubleDir6--;
	if (AirJump > 0)
		AirJump--;

	if (LPressed > 0)
		LPressed--;
	if (MPressed > 0)
		MPressed--;
	if (HPressed > 0)
		HPressed--;
	if (BPressed > 0)
		BPressed--;

	if (LReleased > 0)
		LReleased--;
	if (MReleased > 0)
		MReleased--;
	if (HReleased > 0)
		HReleased--;
	if (BReleased > 0)
		BReleased--;
}

void ABTCharacterBase::AnimationStateMachine()
{
	if (!SurfaceContact())
	{
		if (!ActiveTransitions())
		{
			if (!ConditionalTransitions())
				PassiveTransitions();
		}	
	}
}

bool ABTCharacterBase::ActiveTransitions() //Transitions controlled by player input and character state
{
	//Attack transitions supersede all others

	if (bIsAirborne && CurrentAnimFrame->Invincibility == FaceDown || CurrentAnimFrame->Invincibility == FaceUp && HitStun == 0 && ShatteredTime == 0 && WallStickTime <= 6)
	{
		if (bIsLDown || bIsMDown || bIsHDown || bIsBDown) //hold any attack button down to air recover once able
		{
			WallStickTime = 0;
			if (Dir6 == InputTime) //can hold a direction as well to move in that direction while recovering
			{
				if (bFacingRight)
					Velocity.X = 1;
				else
					Velocity.X = -1;
			}
			else if (Dir4 == InputTime)
			{
				if (bFacingRight)
					Velocity.X = -1;
				else
					Velocity.X = 1;
			}
			Velocity.Y = .5f;
			//make flash white and play chime
			StatusMix = 3;
			StatusTimer = 5;
			StatusColor = FVector(1, 1, 1);
			return EnterNewAnimation(AirRecovery);
		}
	}

	if (CurrentAnimation == &Stagger && HitStun == 0)
	{
		if (bIsLDown || bIsMDown || bIsHDown || bIsBDown) //hold any attack button down recover from stagger once able
		{
			//make flash white and play chime
			StatusMix = 3;
			StatusTimer = 5;
			StatusColor = FVector(1, 1, 1);
			return EnterNewAnimation(IdleStand);
		}
	}

	if (bIsGuarding && !bIsAirborne && FMath::Abs(Position.X - Opponent->Position.X) <= Opponent->CurrentAnimFrame->AutoGuardProximity && BlockStun == 0)
	{
		Velocity.X = 0;
		if (bIsCrouching)
			return EnterNewAnimation(GuardLoIn);
		else
			return EnterNewAnimation(GuardHiIn);

	}

	if (AvailableActions & AcceptJump && ((!bIsAirborne && (Dir7 > 0 || Dir8 > 0 || Dir9 > 0)) || AirJump > 0))
	{
		if (Dir7 > 0 && Dir7 > Dir9 && Dir7 > Dir8) //Most recent input is forward jump
			bForwardJump = true;
		else if (Dir9 > 0 && Dir9 > Dir7 && Dir9 > Dir8) //Most recent input is backward jump
			bBackwardJump = true;

		if (!bIsAirborne)
		{
			return EnterNewAnimation(PreJump); //ground jump has prejump frames
		}
		else // Make sure corresponding animations exist, otherwise default to neutral jump animation
		{
			if (bForwardJump && ForwardJump.Num() > 0)
				return EnterNewAnimation(ForwardJump);
			else if (bBackwardJump && BackwardJump.Num() > 0)
				return EnterNewAnimation(BackwardJump);
			else
				return EnterNewAnimation(NeutralJump);
		}
	}

	if (AvailableActions & AcceptMove && !bIsAirborne)
	{
		/*if (Taunt > 0)
			return EnterNewAnimation(Taunt); */

		if (DoubleDir6 > 0)
		{
			bIsRunning = true;
			DoubleDir6 = 0;
			if (RunStart.Num() > 0)
				return EnterNewAnimation(RunStart);
			else
				return EnterNewAnimation(RunCycle);
		}
		
		if (DoubleDir4 > 0)
		{
			DoubleDir4 = 0;
			return EnterNewAnimation(BackDash);
		}
		 
		if (Dir6 == InputTime && CurrentAnimation != &WalkForward)
			return EnterNewAnimation(WalkForward);
		
		if (Dir4 == InputTime && CurrentAnimation != &WalkBackward)
			return EnterNewAnimation(WalkBackward);
		
		if (bIsCrouching && CurrentAnimation != &CrouchDown && CurrentAnimation != &IdleCrouch && CurrentAnimation != &IdleCrouchBlink && CurrentAnimation != &CrouchIdleAction)
			return EnterNewAnimation(CrouchDown);

		if (!bIsCrouching && (CurrentAnimation == &IdleCrouch || CurrentAnimation == &CrouchDown || CurrentAnimation == &IdleCrouchBlink || CurrentAnimation == &CrouchIdleAction))
			return EnterNewAnimation(StandUp);
	}

	return false;
}

bool ABTCharacterBase::ConditionalTransitions()
{
	if (CurrentAnimation != NULL)
	{
		if (CurrentAnimation == &MidJump && Velocity.Y < 0)
			return EnterNewAnimation(JumpTransition);
		if ((CurrentAnimation == &RunStart || CurrentAnimation == &RunCycle) && !bIsRunning)
			return EnterNewAnimation(Brake);
		if ((CurrentAnimation == &GuardHi || CurrentAnimation == &GuardHiHeavy || CurrentAnimation == &GuardHiVertical || CurrentAnimation == &GuardHiIn) && BlockStun == 0 && 
			(FMath::Abs(Position.X - Opponent->Position.X) > Opponent->CurrentAnimFrame->AutoGuardProximity || !bIsGuarding))
			return EnterNewAnimation(GuardHiOut);
		if ((CurrentAnimation == &GuardLo || CurrentAnimation == &GuardLoHeavy || CurrentAnimation == &GuardLoIn) && BlockStun == 0 &&
			(FMath::Abs(Position.X - Opponent->Position.X) > Opponent->CurrentAnimFrame->AutoGuardProximity || !bIsGuarding))
			return EnterNewAnimation(GuardLoOut);
		if (CurrentAnimation == &GuardAir && BlockStun == 0)
			return EnterNewAnimation(GuardAirOut);

		if (CurrentAnimation == &HitSLIn && BlockStun == 0)
			return EnterNewAnimation(HitSLOut);
		if (CurrentAnimation == &HitSLHeavyIn && BlockStun == 0)
			return EnterNewAnimation(HitSLHeavyOut);
		if (CurrentAnimation == &HitSHIn && BlockStun == 0)
			return EnterNewAnimation(HitSHOut);
		if (CurrentAnimation == &HitSHHeavyIn && BlockStun == 0)
			return EnterNewAnimation(HitSHHeavyOut);
		if (CurrentAnimation == &HitCIn && BlockStun == 0)
			return EnterNewAnimation(HitCOut);
		if (CurrentAnimation == &HitCHeavyIn && BlockStun == 0)
			return EnterNewAnimation(HitCHeavyOut);
		if (CurrentAnimation == &WallStick && WallStickTime == 0)
			return EnterNewAnimation(WallBounce);
	}
	return false;
}

bool ABTCharacterBase::PassiveTransitions()
{
	//Animation transitions triggered by having finished the current animation
	if (AnimFrameIndex == CurrentAnimation->Num()) // When hitting the end of an animation
	{
		if (CurrentAnimFrame->bDoesCycle) //if the anim cycles and are currently at the end of the animation, play it again
		{
			if (CurrentAnimation == &IdleStand || CurrentAnimation == &IdleCrouch)
				IdleCycle++;

			if (IdleCycle == 2)
			{
				IdleCycle = 0;
				if (CurrentAnimation == &IdleStand)
					return EnterNewAnimation(IdleStandBlink);
				else
					return EnterNewAnimation(IdleCrouchBlink);
			}
			/*else if (IdleCycle == 4)
			{
				IdleCycle = 0;
				if (CurrentAnimation == &IdleStand)
					return EnterNewAnimation(StandIdleAction);
				else
					return EnterNewAnimation(CrouchIdleAction);
			}*/
			return EnterNewAnimation(*CurrentAnimation);
		}
		else //certain animations need to transition to other animations upon finishing
		{
			return ExitTimeTransitions();
		}
	}
	else if (PosePlayTime == CurrentAnimFrame->PlayDuration && AnimFrameIndex < CurrentAnimation->Num()) //Move to next frame of current animation
	{
		CurrentAnimFrame = &(*CurrentAnimation)[AnimFrameIndex];
		PosePlayTime = 0;
	}
	return false;
}

bool ABTCharacterBase::ExitTimeTransitions()
{
	if (CurrentAnimation == &PreJump) // Transition from PreJump frames to JumpAnimation
	{
		if (bForwardJump && ForwardJump.Num() > 0)
			return EnterNewAnimation(ForwardJump);
		else if (bBackwardJump && BackwardJump.Num() > 0)
			return EnterNewAnimation(BackwardJump);
		else
			return EnterNewAnimation(NeutralJump);
	}

	if (CurrentAnimation == &NeutralJump || CurrentAnimation == &ForwardJump || CurrentAnimation == &BackwardJump || CurrentAnimation == &GuardAirOut)
	{
		return EnterNewAnimation(MidJump);
	}

	if (CurrentAnimation == &JumpTransition)
	{
		return EnterNewAnimation(JumpDescent);
	}

	if (CurrentAnimation == &StandUp || CurrentAnimation == &Brake || CurrentAnimation == &WakeUpFaceDown || CurrentAnimation == &WakeUpFaceUp || CurrentAnimation == &GuardHiOut ||
		CurrentAnimation == &HitSLOut || CurrentAnimation == &HitSHOut || CurrentAnimation == &HitSLHeavyOut || CurrentAnimation == &HitSHHeavyOut || CurrentAnimation == &IdleStandBlink || CurrentAnimation == &StandIdleAction)
		return EnterNewAnimation(IdleStand);

	if (CurrentAnimation == &CrouchDown || CurrentAnimation == &GuardLoOut || CurrentAnimation == &IdleCrouchBlink || CurrentAnimation == &CrouchIdleAction ||
		CurrentAnimation == &HitCOut || CurrentAnimation == &HitCHeavyOut)
		return EnterNewAnimation(IdleCrouch);

	if (CurrentAnimation == &KnockDownFaceDown || CurrentAnimation == &Crumple)
		return EnterNewAnimation(WakeUpFaceDown);

	if (CurrentAnimation == &KnockDownFaceUp)
		return EnterNewAnimation(WakeUpFaceUp);

	if (CurrentAnimation == &GuardHiIn)
		return EnterNewAnimation(GuardHi);

	if (CurrentAnimation == &GuardLoIn)
		return EnterNewAnimation(GuardLo);

	if (CurrentAnimation == &Deflected || CurrentAnimation == &ThrowEscape)
	{
		if (!bIsAirborne)
			return EnterNewAnimation(IdleStand);
		else
			return EnterNewAnimation(MidJump);
	}

	if (CurrentAnimation == &WallBounce)
		return EnterNewAnimation(FallingForward);

	return false;
}

void ABTCharacterBase::AnimationEvents()
{
	if ((CurrentAnimFrame == &NeutralJump[0] || CurrentAnimFrame == &ForwardJump[0] || CurrentAnimFrame == &BackwardJump[0]) && PosePlayTime == 0) //Jump animation event
	{
		Jumping();
	}
	else if (CurrentAnimation == &WalkForward)
	{
		Velocity = FVector2D(WalkSpeed, 0);

		if (!bFacingRight)
			Velocity *= -1;
	}
	else if (CurrentAnimation == &WalkBackward)
	{
		Velocity = FVector2D(WalkBackSpeed, 0);

		if (bFacingRight)
			Velocity *= -1;
	}
}

bool ABTCharacterBase::QCF()
{
	return (Dir2 > 0 && Dir3 > 0 && Dir6 > 0 && Dir6 > Dir3 && Dir3 > Dir2);
}

bool ABTCharacterBase::QCB()
{
	return (Dir2 > 0 && Dir1 > 0 && Dir4 > 0 && Dir4 > Dir1 && Dir1 > Dir2);
}

bool ABTCharacterBase::HCF()
{
	return (Dir4 > 0 && Dir2 > 0 && Dir6 > 0 && Dir6 > Dir2 && Dir2 > Dir4);
}

bool ABTCharacterBase::HCB()
{
	return (Dir4 > 0 && Dir2 > 0 && Dir6 > 0 && Dir6 < Dir2 && Dir2 < Dir4);
}

bool ABTCharacterBase::DP()
{
	return (Dir6 > 0 && Dir2 > 0 && Dir3 > 0 && Dir3 > Dir2 && Dir2 > Dir6);
}

bool ABTCharacterBase::RDP()
{
	return (Dir4 > 0 && Dir2 > 0 && Dir1 > 0 && Dir1 > Dir2 && Dir2 > Dir4);
}

bool ABTCharacterBase::FC()
{
	return (Dir4 > 0 && Dir2 > 0 && Dir6 > 0 && Dir8) && ((Dir8 > Dir4 && Dir4 > Dir2 && Dir2 > Dir6) || (Dir8 > Dir6 && Dir6 > Dir2 && Dir2 > Dir4));
}

void ABTCharacterBase::RefreshMovelist()
{
	MoveList = 0;
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
	//If the opponent would be on the ground on the next frame, treat them as if they were hit while on the ground
	if (Opponent->HitStun == 0 && Opponent->bIsAirborne && Position.Y + Velocity.Y * 100 / 60.f <= 0)
	{
		Opponent->Position.Y = 0;
		Opponent->Velocity.Y = 0;
		Opponent->bIsAirborne = false;
		if (Opponent->Dir1 == InputTime || Opponent->Dir2 == InputTime || Opponent->Dir3 == InputTime)
		{
			bIsCrouching = true;
		}
	}
	

	//Opponent successfully guarded the attack
	if (((Opponent->bIsGuarding && Opponent->bIsAirborne) || (Opponent->bIsGuarding && Hitbox.AttackHeight <= High) || (Opponent->bIsGuarding && !Opponent->bIsAirborne && Opponent->bIsCrouching && Hitbox.AttackHeight == Low) ||
		(Opponent->bIsGuarding && !Opponent->bIsAirborne && !Opponent->bIsCrouching && Hitbox.AttackHeight == Overhead)) && Hitbox.AttackHeight != Unblockable)
	{
		int32 BlockstunToApply;
		FVector2D KnockbackToApply;
		if (Hitbox.BaseBlockStun == 0)
			BlockstunToApply = Hitbox.BaseHitStun * .8f;
		else
			BlockstunToApply = Hitbox.BaseBlockStun;

		if (Opponent->bIsAirborne)
		{
			if (FMath::Abs(Hitbox.PotentialAirKnockBack.X) > FMath::Abs(Hitbox.PotentialAirKnockBack.Y))
				KnockbackToApply = FVector2D(Hitbox.PotentialAirKnockBack.X, .3f);
			else
				KnockbackToApply = FVector2D(.5f * (Hitbox.PotentialAirKnockBack.X + FMath::Abs(Hitbox.PotentialAirKnockBack.Y)), .3f);
		}	
		else
		{
			if (FMath::Abs(Hitbox.PotentialKnockBack.X) > FMath::Abs(Hitbox.PotentialKnockBack.Y))
				KnockbackToApply = FVector2D(Hitbox.PotentialKnockBack.X, .3f);
			else
				KnockbackToApply = FVector2D(.5f * (Hitbox.PotentialKnockBack.X + FMath::Abs(Hitbox.PotentialKnockBack.Y)), .3f);
		}

		BlockstunToApply = FMath::Min(BlockstunToApply, 30);

		if (Opponent->JustDefense >= 0) //if the opponent Instant Blocked the attack
		{
			//Cuts blockstun by a third on Just defense, cuts by 2 frame at minimum
			if (BlockstunToApply < 9)
				BlockstunToApply -= 2;
			else
				BlockstunToApply = BlockstunToApply * 2 / 3; 

			BlockstunToApply = FMath::Max(1, BlockstunToApply);
			KnockbackToApply *= 0;
			Opponent->Durability += 35; //reward opponent for blocking with exceptional timing
			//make opponent flash white
			StatusMix = 3;
			StatusTimer = 5;
			StatusColor = FVector(1, 1, 1);
			UE_LOG(LogTemp, Warning, TEXT("JUST DEFEND")); //ui Instant block effect "Instant"
		}
		else
		{
			Opponent->ResolveRecoverTimer = 0;

			//blocked hits chip away at durability
			if (Opponent->Resolve > 0)
			{
				if (bIsAirborne)
				{
					if (Hitbox.AttackProperties & IsSuper)
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .35f);
					else if (Hitbox.AttackProperties & IsSpecial)
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .25f);
					else
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .2f);
				}
				else
				{
					if (Hitbox.AttackProperties & IsSuper)
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .25f);
					else if (Hitbox.AttackProperties & IsSpecial)
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .15f);
					else
						Opponent->Durability -= (int32)(Hitbox.BaseDamage * .1f);
				}
				
			}
			else
			{
				//deal chip damage if the opponent has no resolve left, shatter their guard if they are airborne
				if (bIsAirborne)
				{
					Opponent->bCounterHitState = true;
					AttackCalculation(Hitbox, HurtboxCenter);
					return;
				}
				else
				{
					int32 ChipDamage = (int32)(Hitbox.BaseDamage * .2f);

					if (Opponent->Health - ChipDamage <= 0 && Opponent->Health > 1) //Chip damage reduces character's life to 1 before being lethal
						ChipDamage = Opponent->Health - 1;
					else
						ChipDamage = FMath::Min(Opponent->Health, ChipDamage);

					Opponent->Health -= ChipDamage;
				}
			}
		}
		
		//apply final blockstun calculation
		Opponent->BlockStun = BlockstunToApply;

		if (Opponent->bIsCrouching) //extra blockstun if the opponent is crouching
			Opponent->BlockStun += 2;

		//apply hitstop
		Opponent->HitStop = Hitbox.BaseHitStop - 2;
		HitStop = Hitbox.BaseHitStop - 2;

		if (Opponent->bTouchingWall && KnockbackToApply.X >= 0)
		{
			if (!(Hitbox.AttackProperties & IsSuper) && !(Hitbox.AttackProperties & IsSpecial))
				KnockBack = FVector2D(.75f * KnockbackToApply.X, 0);
		}
		else
		{
			if (KnockbackToApply.X < 0)
				Opponent->KnockBack = FVector2D(.5f * KnockbackToApply.X, KnockbackToApply.Y);
			else
				Opponent->KnockBack = KnockbackToApply;
		}

		//Made contact so can cancel into other actions
		AvailableActions = Hitbox.PotentialActions;
		AvailableActions &= AcceptAll - AcceptJump;

		if (Hitbox.PotentialActions & JumpCancelOnBlock && Hitbox.PotentialActions & AcceptJump) //can only jump cancel if the move allows it on block
			AvailableActions |= AcceptJump;

		//update opponent's animation to guarding
		if (bIsAirborne)
			Opponent->EnterNewAnimation(Opponent->GuardAir);
		else
		{
			if (Opponent->bIsCrouching)
			{
				if (Hitbox.AttackLevel > 1)
					Opponent->EnterNewAnimation(Opponent->GuardLoHeavy);
				else
					Opponent->EnterNewAnimation(Opponent->GuardLo);
			}
			else
			{
				if (Hitbox.AttackLevel > 1)
					Opponent->EnterNewAnimation(Opponent->GuardHiHeavy);
				else
					Opponent->EnterNewAnimation(Opponent->GuardHi);
			}
		}

		//place and play guard effect
		//place at midpoint between hitbox center and hurtbox center
	}
	else if (Opponent->bArmorActive && Opponent->Resolve > 0 && !(Hitbox.AttackProperties & Piercing) && !(Hitbox.AttackProperties & Shatter))
	{
		Opponent->ResolveRecoverTimer = 0;
		Opponent->Durability -= Hitbox.DurabilityDamage;
		Opponent->Resolve -= Hitbox.ResolveDamage;
		HitStop = Hitbox.BaseHitStop;
		Opponent->HitStop = HitStop;
		//make opponent flash red
		StatusMix = .8f;
		StatusTimer = 5;
		StatusColor = FVector(1, 0, 0);

		//available actions are more limited when hitting an opponent's armor
		if (Hitbox.PotentialActions & AcceptSpecial)
			AvailableActions |= AcceptSpecial;
		if (Hitbox.PotentialActions & AcceptSuper)
			AvailableActions |= AcceptSuper;
		if (Hitbox.PotentialActions & AcceptJump)
			AvailableActions |= AcceptJump;
		if (Hitbox.PotentialActions & AcceptBlitz)
			AvailableActions |= AcceptBlitz;

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
	if (Hitbox.AttackHeight < Throw)
		ComboCount++;
	Opponent->BlockStun = 0;
	Opponent->WallStickTime = 0;

	if (Hitbox.AttackHeight < Throw && Opponent->CurrentAnimation == &Opponent->Crumple) //treat opponent as if airborne if hitting them in crumple
		Opponent->bIsAirborne = true;

	if (ComboCount >= 2)
	{
		//if the opponent is in an aerial hitstun animation or staggered but their hitstun is zero, they could have escaped
		if ((Opponent->HitStun == 0 && Opponent->ShatteredTime == 0 && Opponent->WallStickTime <= 6 && Opponent->bIsAirborne && 
			(Opponent->CurrentAnimFrame->Invincibility == FaceDown || Opponent->CurrentAnimFrame->Invincibility == FaceUp)) ||
			(Opponent->CurrentAnimation == &Opponent->Stagger && Opponent->HitStun == 0))
			bTrueCombo = false;
		//display UI combo counter
	}
	else
	{
		//hide UI combo counter
	}

	//calculate damage scaling based on opponent's remaining health
	float OpponentValor;

	if ((float)Health / (float)MaxHealth <= .1f)
		OpponentValor = Opponent->Valor10;
	else if ((float)Health / (float)MaxHealth <= .25f)
		OpponentValor = Opponent->Valor25;
	else if ((float)Health / (float)MaxHealth <= .5f)
		OpponentValor = Opponent->Valor50;
	else
		OpponentValor = Opponent->Valor100;

	int32 DamageToApply = OpponentValor * SpecialProration * Hitbox.BaseDamage;
	uint8 HitStunToApply = Hitbox.BaseHitStun;

	AvailableActions = Hitbox.PotentialActions;
	Opponent->CharacterHitState = Hitbox.AttackProperties;
	//apply certain modifiers based on circumstances around the hit
	if (Hitbox.AttackHeight < Throw)
	{
		if ((Opponent->bArmorActive || Opponent->bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			Opponent->ShatteredTime = 120;
			HitStunToApply *= 1.2f;
			Opponent->CharacterHitState |= Hitbox.CounterAttackProperties;
			//set shatter UI effect to play
		}
		else if ((Opponent->bArmorActive || Opponent->bCounterHitState) && !(Hitbox.AttackProperties & Piercing && Opponent->bArmorActive && Opponent->Resolve > 0))
		{
			HitStunToApply *= 1.2f;
			Opponent->CharacterHitState |= Hitbox.CounterAttackProperties;
			//set counter hit ui effect to play
		}
		else if (Hitbox.AttackProperties & Piercing && Opponent->bArmorActive && Opponent->Resolve > 0)
		{
			//set piercing ui effect to play
		}
	}
		
	if (Opponent->CurrentAnimFrame->Invincibility == OTG)
	{
		DamageToApply = (int32)(.3f * DamageToApply);
		HitStunToApply = (uint8)(.3f * HitStunToApply);
	}

	//calculate proration to apply on subsequent hits in a combo
	if (ComboCount == 1)
	{
		SpecialProration = Hitbox.InitProration; //initial proration is only applied if its the first hit of a combo
		bTrueCombo = true; //reset to true since a new combo has started
		if ((Opponent->bArmorActive || Opponent->bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			SpecialProration *= 1.2f;
			DamageToApply *= 1.2f;
		}
		else if (Opponent->bCounterHitState)
			SpecialProration *= 1.1f;
	}
	else if (ComboCount > 1 && Hitbox.ForcedProration != 1)
		SpecialProration *= Hitbox.ForcedProration; //forced proration is applied as long as the move is used in combo

	//apply damage, damage is scaled by the number of hits in a combo
	float ComboProration;
	if (ComboCount < 3)
		ComboProration = 1;
	else if (ComboCount < 5)
		ComboProration = .8f;
	else if (ComboCount < 6)
		ComboProration = .7f;
	else if (ComboCount < 7)
		ComboProration = .6f;
	else if (ComboCount < 8)
		ComboProration = .5f;
	else if (ComboCount < 9)
		ComboProration = .4f;
	else if (ComboCount < 10)
		ComboProration = .3f;
	else if (ComboCount < 11)
		ComboProration = .2f;
	else
		ComboProration = .1f;

	DamageToApply *= ComboProration;

	if (Hitbox.AttackProperties & IsSuper)
		DamageToApply = FMath::Max((int32)(OpponentValor * Hitbox.BaseDamage * .25f), DamageToApply); //Supers will always deal a minimum of 25% their base damage affected by valor

	DamageToApply = FMath::Max(1, DamageToApply); //non-super attacks will always deal a minimum of one damage

	Opponent->Health -= FMath::Min(DamageToApply, Opponent->Health);

	//apply hitstun, hitstun is scaled by how much time the opponent has spent in hitstun, supers' hitstun is never scaled
	if (Opponent->bIsAirborne && !(Hitbox.AttackProperties & IsSuper))
	{
		if (Opponent->ComboTimer > 960 && !(Hitbox.AttackProperties & IsSpecial)) //16 seconds, normal attacks will only deal 1 frame of hitstun
			HitStunToApply = 1;
		else if(Opponent->ComboTimer > 840) //14 seconds, special attacks have a minimum of 60% their base hitstun
			HitStunToApply *= .6f;
		else if (Opponent->ComboTimer > 600)//10 seconds
			HitStunToApply *= .7f;
		else if (Opponent->ComboTimer > 420)//7 seconds
			HitStunToApply *= .8f;
		else if (Opponent->ComboTimer > 300)//5 seconds
			HitStunToApply *= .9f;
	}
	Opponent->HitStun = HitStunToApply;
	if (Opponent->bIsCrouching) //two extra frames of hitstun if hitting a crouching opponent
		Opponent->HitStun += 2;

	//apply hitstop
	Opponent->HitStop = Hitbox.BaseHitStop;
	HitStop = Hitbox.BaseHitStop;

	//meter gain for each character
	if (Opponent->ShatteredTime == 0)
		Opponent->Durability += FMath::Max((int32)(Hitbox.BaseDamage * .2f), 1);

	if (ResolveRecoverTimer >= 180)
		Durability += FMath::Max((int32)(Hitbox.BaseDamage * .1f), 1);

	//Make certain actions available for hitting with an attack
	AvailableActions = Hitbox.PotentialActions;

	//Apply knockback to opponent
	if (Opponent->CurrentAnimFrame->Invincibility == OTG)
	{
		Opponent->KnockBack = FVector2D(1.1f * FMath::Abs(Hitbox.PotentialKnockBack.X), 1.f);
	}
	else if (Opponent->bIsAirborne)
	{
		if (Hitbox.PotentialAirKnockBack == FVector2D(0, 0) && Hitbox.PotentialKnockBack.Y == 0)
			Opponent->KnockBack = FVector2D(Hitbox.PotentialKnockBack.X, .5f);
		else
			Opponent->KnockBack = Hitbox.PotentialAirKnockBack;
	}
	else
	{
		Opponent->KnockBack = Hitbox.PotentialKnockBack;
	}

	//apply pushback to self when not using a special or super
	if (!(Hitbox.AttackProperties & IsSpecial) && !(Hitbox.AttackProperties & IsSuper))
	{
		float WallPushBack = 0, PushBack = 0;
		if (Opponent->bTouchingWall) //if opponent is against a wall, character has to move back instead albeit with less magnitude
		{
			if (FMath::Abs(Hitbox.PotentialKnockBack.X) > FMath::Abs(Hitbox.PotentialKnockBack.Y))
				WallPushBack = .5f * FMath::Abs(Hitbox.PotentialKnockBack.X);
			else
				WallPushBack = .25f * (FMath::Abs(Hitbox.PotentialKnockBack.X) + FMath::Abs(Hitbox.PotentialKnockBack.Y));
		}

		if (Opponent->ComboTimer > 3) //will not experience pushback during first three seconds of a combo
		{
			if (Opponent->ComboTimer < 5)
				PushBack = .75f;
			else if (Opponent->ComboTimer < 7)
				PushBack = 1.f;
			else if (Opponent->ComboTimer < 9)
				PushBack = 1.25f;
			else if (Opponent->ComboTimer < 12)
				PushBack = 1.5f;
			else if (Opponent->ComboTimer < 15)
				PushBack = 1.75f;
			else
				PushBack = 2.f;
		}

		if (WallPushBack > PushBack)
			KnockBack.X = -WallPushBack;
		else
			KnockBack.X = -PushBack;
	}

	//update opponent's animation
	if (Hitbox.AttackProperties & CanTumble)
	{
		Opponent->EnterNewAnimation(Opponent->Tumble);
	}
	else if (Hitbox.AttackProperties & CanSweep)
	{
		if (Opponent->KnockBack.Y < 0)
			Opponent->EnterNewAnimation(Opponent->FallingForward);
		else
			Opponent->EnterNewAnimation(Opponent->WallBounce);
	}
	else if (Hitbox.AttackProperties & CanLaunch)
	{
		if (Opponent->KnockBack.Y < 0)
			Opponent->EnterNewAnimation(Opponent->LaunchFallCycle);
		else
			Opponent->EnterNewAnimation(Opponent->LaunchCycle);
	}
	else if (Opponent->bIsAirborne || Opponent->KnockBack.Y > 0)
	{
		if (Hitbox.AttackProperties & CanKnockAway)
			Opponent->EnterNewAnimation(Opponent->KnockAway);
		else
			Opponent->EnterNewAnimation(Opponent->HitstunAir);
	}
	else if (!Opponent->bIsAirborne) //ground only hit states
	{
		if (Hitbox.AttackProperties & CanCrumple || Opponent->Health == 0)
			Opponent->EnterNewAnimation(Opponent->Crumple);
		else if (Hitbox.AttackProperties & CanStagger)
			Opponent->EnterNewAnimation(Opponent->Stagger);
		else if (Opponent->bIsCrouching)
		{
			if (Hitbox.AttackLevel > 2)
				Opponent->EnterNewAnimation(Opponent->HitCHeavyIn);
			else
				Opponent->EnterNewAnimation(Opponent->HitCIn);
		}
		else
		{
			if (Hitbox.AttackProperties & LowerBodyHit)
			{
				if (Hitbox.AttackLevel > 2)
					Opponent->EnterNewAnimation(Opponent->HitSLHeavyIn);
				else
					Opponent->EnterNewAnimation(Opponent->HitSLIn);
			}
			else
			{
				if (Hitbox.AttackLevel > 2)
					Opponent->EnterNewAnimation(Opponent->HitSHHeavyIn);
				else
					Opponent->EnterNewAnimation(Opponent->HitSHIn);
			}
		}
	}

	//place and play hit effect
	//place at midpoint between hitbox center and hurtbox center
	if (Hitbox.AttackProperties & PlayHitEffect)
	{

	}
}

void ABTCharacterBase::ContactThrow(FHitbox Hitbox, int32 ThrowType)
{
	Opponent->TurnAroundCheck();

	if (ThrowType == Throw || ThrowType == AirThrow)
	{
		if (Opponent->AvailableActions & ThrowTech)
		{
			bClash = true;
			Opponent->bClash = true;
			KnockBack = FVector2D(-1, 0);
			Opponent->KnockBack = FVector2D(-1, 0);
			EnterNewAnimation(ThrowEscape);
			Opponent->EnterNewAnimation(Opponent->ThrowEscape);
			//play throw escape effect
		}
		else if (Opponent->Resolute)
		{
			EnterNewAnimation(Deflected);
			//play Resolute Counter UI graphic
			Opponent->ResolveRecoverTimer = 180;
			Opponent->Durability = 101;
			if (ThrowType == AirThrow)
			{
				Opponent->EnterNewAnimation(Opponent->AirResoluteCounter);
			}
			else
			{
				Opponent->EnterNewAnimation(Opponent->ResoluteCounter);
			}
		}
	}
	else if ((ThrowType == CommandThrow || ThrowType == AirCommandThrow) && Opponent->CurrentHitbox->Num() > 0)  //check if both players have landed a command throw, to induce a throwbreak
	{
		if (((*Opponent->CurrentHitbox)[0].AttackHeight == CommandThrow && !bIsAirborne) || ((*Opponent->CurrentHitbox)[0].AttackHeight == AirCommandThrow && bIsAirborne))
		{
			for (int32 i = 0; i < Opponent->CurrentHitbox->Num() && !bClash; i++)
			{
				FVector2D OpponentHitboxCenter;
				if (bFacingRight)
					OpponentHitboxCenter = Opponent->Position + (*Opponent->CurrentHitbox)[i].Position;
				else
					OpponentHitboxCenter = FVector2D(Opponent->Position.X - (*Opponent->CurrentHitbox)[i].Position.X, Position.Y + (*Opponent->CurrentHitbox)[i].Position.Y);

				FVector2D PushboxCenter = FVector2D(Position.X, Position.Y);
				FVector2D PushboxSize = FVector2D(PushboxWidth, StandingPushBoxHeight);

				if (bIsAirborne)
				{
					PushboxCenter.Y += .5f * CrouchingPushBoxHeight + AirPushboxVerticalOffset;
					PushboxSize.Y = CrouchingPushBoxHeight;
				}
				else if (bIsCrouching)
				{
					PushboxCenter.Y += .5f * CrouchingPushBoxHeight;
					PushboxSize.Y = CrouchingPushBoxHeight;
				}
				else
					PushboxCenter.Y += .5f * StandingPushBoxHeight;

				if (RectangleOverlap(OpponentHitboxCenter, PushboxCenter, (*Opponent->CurrentHitbox)[i].Size, PushboxSize))
				{
					bClash = true;
					Opponent->bClash = true;
					KnockBack = FVector2D(-1, 0);
					Opponent->KnockBack = FVector2D(-1, 0);
					EnterNewAnimation(ThrowEscape);
					Opponent->EnterNewAnimation(Opponent->ThrowEscape);
					//play throw escape effect
				}
			}
		}

	}

	if (!bClash)
	{
		bAttackMadeContact = true;
		AttackCalculation(Hitbox, Opponent->Position);
	}
}

void ABTCharacterBase::SetSounds()
{
	if (CurrentAnimFrame->SFX != NULL)
	{
		CharacterSoundEffects->SetSound(CurrentAnimFrame->SFX);
		PlaySound = true;
	}
	else
	{
		CharacterSoundEffects->SetSound(NULL);
	}

	if (CurrentAnimFrame->VoiceLines != NULL)
	{
		CharacterVoice->SetSound(CurrentAnimFrame->VoiceLines);
		PlaySound = true;
	}
	else
	{
		CharacterVoice->SetSound(NULL);
	}
}
