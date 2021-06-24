// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "BTProjectileBase.h"
#include "BTCharacterBase.h"

// Sets default values
ABTProjectileBase::ABTProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Transform = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	RootComponent = Transform;
}

// Called when the game starts or when spawned
void ABTProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABTProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABTProjectileBase::Activate(bool FacingRight)
{
	CurrentState.bFacingRight = FacingRight;
	CurrentState.bIsActive = true;
	CurrentState.bReflected = false;
	CurrentState.bHitByOwner = false;
	CurrentState.bHitByOwner = false;
	CurrentState.bHitFriend = false;
	CurrentState.bHitByFriend = false;
	CurrentState.bHitSuccess = false;
	CurrentState.bAttackMadeContact = false;
	CurrentState.bProjectileClash = false;

	CurrentState.AnimFrameIndex = 0;
	CurrentState.FramePlayTime = 0;
}

void ABTProjectileBase::ThrowDetection()
{
	//logic to follow for throws
	if (CurrentState.bIsActive && CurrentState.HitStop == 0 && !CurrentState.bProjectileClash)
	{
		if (Owner && CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
		{
			if (Owner->Opponent)
			{
				CurrentState.bHitSuccess = false;
				if (CurrentState.CurrentAnimFrame.bCanHit && (((CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == CommandThrow || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == Throw) && !Owner->Opponent->CurrentState.bIsAirborne) ||
					((CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == AirCommandThrow || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == AirThrow) && Owner->Opponent->CurrentState.bIsAirborne)) &&
					((Owner->Opponent->CurrentState.HitStun == 0) || CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & ComboThrow || Owner->Opponent->IsCurrentAnimation(Owner->Opponent->Stagger) ||
						Owner->Opponent->IsCurrentAnimation(Owner->Opponent->Crumple)) && Owner->Opponent->CurrentState.BlockStun == 0 && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != ThrowInvincible &&
					Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != FullInvincible && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != Intangible && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != OTG)
				{
					for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
					{
						FVector2D HitboxCenter;
						if (CurrentState.bFacingRight)
							HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
						else
							HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

						FVector2D OpponentPushboxCenter = FVector2D(Owner->Opponent->CurrentState.Position.X, Owner->Opponent->CurrentState.Position.Y);
						FVector2D OpponentPushboxSize = FVector2D(Owner->Opponent->PushboxWidth, Owner->Opponent->StandingPushBoxHeight);

						if (Owner->Opponent->CurrentState.bIsAirborne)
						{
							OpponentPushboxCenter.Y += .5f * Owner->Opponent->CrouchingPushBoxHeight + Owner->Opponent->AirPushboxVerticalOffset;
							OpponentPushboxSize.Y = Owner->Opponent->CrouchingPushBoxHeight;
						}
						else if (Owner->Opponent->CurrentState.bIsCrouching)
						{
							OpponentPushboxCenter.Y += .5f * Owner->Opponent->CrouchingPushBoxHeight;
							OpponentPushboxSize.Y = Owner->Opponent->CrouchingPushBoxHeight;
						}
						else
							OpponentPushboxCenter.Y += .5f * Owner->Opponent->StandingPushBoxHeight;

						if (Owner->RectangleOverlap(HitboxCenter, OpponentPushboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, OpponentPushboxSize))
						{
							ContactThrow(CurrentState.CurrentAnimFrame.Hitboxes[i], CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight);
						}
					}
				}
			}
		}
	}
}

void ABTProjectileBase::HitDetection()
{
	if (CurrentState.bIsActive && CurrentState.HitStop == 0 && !CurrentState.bProjectileClash)
	{
		CurrentState.bHitSuccess = false;
		if (Owner && CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
		{
			if (Owner->Opponent)
			{
				if (Owner->Opponent->Projectiles.Num() > 0)
				{
					for (ABTProjectileBase* Projectile : Owner->Opponent->Projectiles) //check for clashes with opponent's projectiles
					{
						if (Projectile->CurrentState.bIsActive && Projectile->CurrentState.CurrentAnimFrame.Hitboxes.Num())
						{
							for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bProjectileClash; i++)
							{
								FVector2D HitboxCenter;
								if (CurrentState.bFacingRight)
									HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
								else
									HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

								for (uint8 j = 0; j < Projectile->CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bProjectileClash; j++)
								{
									FVector2D OpponentHitboxCenter;
									if (Owner->Opponent->CurrentState.bFacingRight)
										OpponentHitboxCenter = Owner->Opponent->CurrentState.Position + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position;
									else
										OpponentHitboxCenter = FVector2D(Owner->Opponent->CurrentState.Position.X - Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position.X,
											Owner->Opponent->CurrentState.Position.Y + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position.Y);

									if (Owner->RectangleOverlap(HitboxCenter, OpponentHitboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Size))
									{
										CurrentState.bProjectileClash = true;
										Projectile->CurrentState.bProjectileClash = true;
										//trigger clash effect
										if (CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & CanDeflect && !(Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & CanDeflect) && !(Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & IsSuper))
										{
											Projectile->CurrentState.bIsActive = false;
										}
										else if (!(CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & CanDeflect) && (Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & CanDeflect) && !(CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & IsSuper))
										{
											CurrentState.bIsActive = false;
										}
										else
										{
											Projectile->CurrentState.CurrentHits++;
											CurrentState.CurrentHits++;
										}
									}
								}
							}
						}
					}
				}
				if (!CurrentState.bProjectileClash && !CurrentState.bReflected) //look for hit
				{
					//only look for hits if there are hitboxes active, and the current hitbox has not hit anything previously
					if (!CurrentState.bAttackMadeContact && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != ProjectileInvincible)
					{
						if (CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight < Throw) //Current active attack is a strike
						{
							if (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes.Num())
							{
								if (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes.Num() > 0 && !Owner->Opponent->CurrentState.bAttackMadeContact) //only look for clashes if the opponent has an active attack out
								{
									//loop through opponent's active hitboxes
									for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
									{
										FVector2D HitboxCenter;
										if (CurrentState.bFacingRight)
											HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
										else
											HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

										for (uint8 j = 0; j < Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes.Num() && !CurrentState.bAttackMadeContact; j++)
										{
											FVector2D OpponentHitboxCenter;
											if (Owner->Opponent->CurrentState.bFacingRight)
												OpponentHitboxCenter = Owner->Opponent->CurrentState.Position + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position;
											else
												OpponentHitboxCenter = FVector2D(Owner->Opponent->CurrentState.Position.X - Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position.X,
													Owner->Opponent->CurrentState.Position.Y + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Position.Y);

											if (Owner->RectangleOverlap(HitboxCenter, OpponentHitboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].Size))
											{
												Owner->Opponent->CurrentState.bClash = true;
												Owner->Opponent->CurrentState.bAttackMadeContact = true;
												if (bAttackOwner)
												{
													CurrentState.bIsActive = false;
													Owner->CurrentState.HitStun = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].BaseHitStun;
													Owner->CurrentState.HitStop = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].BaseHitStop;
													CurrentState.HitStop = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].BaseHitStop;
													Owner->Opponent->CurrentState.HitStop = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].BaseHitStop;

													float OpponentValor;
													float OpponentHealthPercent = (float)Owner->Opponent->CurrentState.Health / (float)Owner->Opponent->MaxHealth;

													if (OpponentHealthPercent <= .1f)
														OpponentValor = Owner->Opponent->Valor10;
													else if (OpponentHealthPercent <= .25f)
														OpponentValor = Owner->Opponent->Valor25;
													else if (OpponentHealthPercent <= .5f)
														OpponentValor = Owner->Opponent->Valor50;
													else
														OpponentValor = Owner->Opponent->Valor100;

													Owner->CurrentState.Health -= OpponentValor * Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].BaseDamage;

													if (Owner->CurrentState.bIsAirborne || Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].PotentialKnockBack.Y > 0)
													{
														Owner->CurrentState.KnockBack = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].PotentialAirKnockBack;
														Owner->EnterNewAnimation(Owner->HitstunAir);
													}
													else
													{
														Owner->CurrentState.KnockBack.X = Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[j].PotentialKnockBack.X;
														Owner->EnterNewAnimation(Owner->Stagger);
													}
												}
												//projectiles with the deflect property can nullify opponents' non-super, non-deflect attacks
												else if (CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect && !(Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[0].AttackProperties & CanDeflect) &&
													!(Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[0].AttackProperties & IsSuper))
												{
													Owner->Opponent->CurrentState.AvailableActions = None;
													if (Owner->Opponent->CurrentState.bIsAirborne)
														Owner->Opponent->EnterNewAnimation(Owner->Opponent->DeflectedAir);
													else
														Owner->Opponent->EnterNewAnimation(Owner->Opponent->Deflected);

													CurrentState.HitStop = 9;
													Owner->Opponent->CurrentState.HitStop = 9;
												}
												//opponent's attack can deflect
												else if (!(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & CanDeflect) && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[0].AttackProperties & CanDeflect && !(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackProperties & IsSuper))
												{
													CurrentState.bIsActive = false;
													Owner->Opponent->CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);

													CurrentState.HitStop = 9;
													Owner->Opponent->CurrentState.HitStop = 9;
												}
												else //otherwise normal clash
												{
													CurrentState.CurrentHits++;
													CurrentState.bAttackMadeContact = true;
													CurrentState.HitStop = 15;
													Owner->Opponent->CurrentState.HitStop = 15;
													Owner->Opponent->CurrentState.AvailableActions = AcceptAll - (AcceptMove + AcceptGuard);
												}
												//play clash effect
											}
										}
									}
								}
								//loop through opponent's active hurtboxes and see if any current hitboxes overlap them
								if (CurrentState.CurrentAnimFrame.bCanHit && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != StrikeInvincible && Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility != FullInvincible &&
									!(CurrentState.CurrentAnimFrame.Hitboxes[0].AttackHeight == High && Owner->Opponent->CurrentState.bIsCrouching) && !CurrentState.bAttackMadeContact)
								{
									if (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes.Num() > 0)
									{
										for (uint8 i = 0; i < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bAttackMadeContact; i++)
										{
											FVector2D HitboxCenter;
											if (CurrentState.bFacingRight)
												HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
											else
												HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

											for (uint8 j = 0; j < Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes.Num() && !CurrentState.bAttackMadeContact; j++)
											{
												FVector2D OpponentHurtboxCenter;
												if (Owner->Opponent->CurrentState.bFacingRight)
													OpponentHurtboxCenter = Owner->Opponent->CurrentState.Position + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes[j].Position;
												else
													OpponentHurtboxCenter = FVector2D(Owner->Opponent->CurrentState.Position.X - Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes[j].Position.X, Owner->Opponent->CurrentState.Position.Y + Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes[j].Position.Y);

												if (Owner->RectangleOverlap(HitboxCenter, OpponentHurtboxCenter, CurrentState.CurrentAnimFrame.Hitboxes[i].Size, Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hurtboxes[j].Size))
												{
													CurrentState.bAttackMadeContact = true;
													if (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == SuperCounter || (((Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == AllCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight != Unblockable) ||
														(Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == HiCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight < Low) || (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == LowCounter && CurrentState.CurrentAnimFrame.Hitboxes[i].AttackHeight == Low)) &&
														!(CurrentState.CurrentAnimFrame.Hitboxes[i].AttackProperties & IsSuper))) //check if the opponent is in a counter stance that can counter the current attack
													{
														CurrentState.HitStop = 24;
														Owner->Opponent->CurrentState.bClash = true;
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
					}
				}
				else if (CurrentState.bReflected) //look for hit
				{
				}
			}
			if (bCheckFriends) //see if the hitbox hits any friendly projectile hitboxes
			{
				if (Owner)
				{
					if (Owner->Projectiles.Num() > 0)
					{
						if (CurrentState.CurrentAnimFrame.Hitboxes.Num() > 0)
						{
							for (ABTProjectileBase* Projectile : Owner->Projectiles)
							{
								if (Projectile != this && Projectile->CurrentState.CurrentAnimFrame.Hitboxes.Num())
								{
									if (Projectile->CurrentState.bIsActive && Projectile->CurrentState.CurrentAnimFrame.Hitboxes.Num())
									{
										for (uint8 i = 0; i < Projectile->CurrentState.CurrentAnimFrame.Hitboxes.Num() && !Projectile->CurrentState.bHitByFriend; i++)
										{
											FVector2D FriendHitboxCenter;
											if (CurrentState.bFacingRight)
												FriendHitboxCenter = Projectile->CurrentState.Position + Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
											else
												FriendHitboxCenter = FVector2D(Projectile->CurrentState.Position.X - Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, Projectile->CurrentState.Position.Y + Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

											for (uint8 j = 0; j < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !Projectile->CurrentState.bHitByFriend; j++)
											{
												FVector2D HitboxCenter;
												if (CurrentState.bFacingRight)
													HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[j].Position;
												else
													HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[j].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[j].Position.Y);

												if (Owner->RectangleOverlap(FriendHitboxCenter, HitboxCenter, Projectile->CurrentState.CurrentAnimFrame.Hitboxes[i].Size, CurrentState.CurrentAnimFrame.Hitboxes[j].Size))
												{
													Projectile->CurrentState.bHitByFriend = true;
													CurrentState.bHitFriend = true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			if (bCheckHitByOwner && !CurrentState.bHitByOwner) //see if the owner's attack hits this projectile
			{
				if (Owner)
				{
					if (Owner->CurrentState.CurrentAnimFrame.Hitboxes.Num())
					{
						if (Owner->CurrentState.CurrentAnimFrame.Hitboxes.Num())
						{
							if (CurrentState.CurrentAnimFrame.Hitboxes.Num())
							{
								for (uint8 i = 0; i < Owner->CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bHitByOwner; i++)
								{
									FVector2D OwnerHitboxCenter;
									if (Owner->CurrentState.bFacingRight)
										OwnerHitboxCenter = Owner->CurrentState.Position + Owner->CurrentState.CurrentAnimFrame.Hitboxes[i].Position;
									else
										OwnerHitboxCenter = FVector2D(Owner->CurrentState.Position.X - Owner->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.X, Owner->CurrentState.Position.Y + Owner->CurrentState.CurrentAnimFrame.Hitboxes[i].Position.Y);

									for (uint8 j = 0; j < CurrentState.CurrentAnimFrame.Hitboxes.Num() && !CurrentState.bHitByOwner; j++)
									{
										FVector2D HitboxCenter;
										if (CurrentState.bFacingRight)
											HitboxCenter = CurrentState.Position + CurrentState.CurrentAnimFrame.Hitboxes[j].Position;
										else
											HitboxCenter = FVector2D(CurrentState.Position.X - CurrentState.CurrentAnimFrame.Hitboxes[j].Position.X, CurrentState.Position.Y + CurrentState.CurrentAnimFrame.Hitboxes[j].Position.Y);

										if (Owner->RectangleOverlap(OwnerHitboxCenter, HitboxCenter, Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Hitboxes[i].Size, CurrentState.CurrentAnimFrame.Hitboxes[j].Size))
										{
											CurrentState.bHitByOwner = true;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	CurrentState.bProjectileClash = false;
}

void ABTProjectileBase::UpdateProjectile()
{
	//trigger stuff for certain projectiles if they make contact here

	if (bHasHits)
	{
		if (CurrentState.CurrentHits < MaxHits)
			CurrentState.bAttackMadeContact = false;
		else if (CurrentState.CurrentHits >= MaxHits)
			CurrentState.bIsActive = false;
	}

	if (bHasLifeSpan)
	{
		if (Owner->CurrentState.SlowMoTime % 2 == 0)
			CurrentState.CurrentLife++;
		if (CurrentState.CurrentLife > MaxLife)
			CurrentState.bIsActive = false;
	}
}

void ABTProjectileBase::UpdatePosition()
{
	if (CurrentState.HitStop == 0)
	{
		if (Owner)
		{
			if (Owner->CurrentState.SlowMoTime > 0)
				CurrentState.Position += CurrentState.Velocity * 100 / 120.f;
			else
				CurrentState.Position += CurrentState.Velocity * 100 / 60.f;
		}
	}
	else
	{
		CurrentState.HitStop--;
	}

	if (bCanTouchSurfaces)
	{
		if (CurrentState.Position.Y <= .5f * SurfaceBounds.Y)
			CurrentState.Position.Y = .5f * SurfaceBounds.Y;

		if (CurrentState.Position.X <= 1000 + .5f * SurfaceBounds.X)
			CurrentState.Position.X = 1000 + .5f * SurfaceBounds.X;

		if (CurrentState.Position.X >= 1000 - .5f * SurfaceBounds.X)
			CurrentState.Position.X = 1000 - .5f * SurfaceBounds.X;
	}
}

void ABTProjectileBase::DrawProjectile()
{
	/*if (CurrentState.bIsActive && CurrentState.AnimFrameIndex > MaxFrameIndex)
		SetVisibility(false);*/
}

void ABTProjectileBase::AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter)
{
	if (Hitbox.AttackHeight < Throw)
		Owner->CurrentState.ComboCount++;
	Owner->Opponent->CurrentState.BlockStun = 0;

	if (Hitbox.AttackHeight < Throw && Owner->Opponent->IsCurrentAnimation(Owner->Opponent->Crumple)) //treat opponent as if airborne if hitting them in crumple
		Owner->Opponent->CurrentState.bIsAirborne = true;

	if (Owner->CurrentState.ComboCount >= 2)
	{
		//if the opponent is in an aerial hitstun animation or staggered but their hitstun is zero, they could have escaped
		if ((Owner->Opponent->CurrentState.HitStun == 0 && Owner->Opponent->CurrentState.ShatteredTime == 0 && Owner->Opponent->CurrentState.bIsAirborne &&
			(Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == FaceDown || Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == FaceUp)) ||
			(Owner->Opponent->IsCurrentAnimation(Owner->Opponent->Stagger) && Owner->Opponent->CurrentState.HitStun == 0))
			Owner->CurrentState.bTrueCombo = false;
		//display UI combo counter
	}
	else
	{
		//hide UI combo counter
	}

	//calculate damage scaling based on opponent's remaining health
	float OpponentValor;
	float OpponentHealthPercent = (float)Owner->Opponent->CurrentState.Health / (float)Owner->Opponent->MaxHealth;

	if (OpponentHealthPercent <= .1f)
		OpponentValor = Owner->Opponent->Valor10;
	else if (OpponentHealthPercent <= .25f)
		OpponentValor = Owner->Opponent->Valor25;
	else if (OpponentHealthPercent <= .5f)
		OpponentValor = Owner->Opponent->Valor50;
	else
		OpponentValor = Owner->Opponent->Valor100;

	int32 DamageToApply = OpponentValor * Owner->CurrentState.SpecialProration * Hitbox.BaseDamage;
	uint8 HitStunToApply = Hitbox.BaseHitStun;
	FVector2D KnockBackToApply;
	uint8 HitStopToApply = Hitbox.BaseHitStop;

	if (Owner->Opponent->CurrentState.CurrentAnimFrame.Invincibility == OTG)
	{
		KnockBackToApply = FVector2D(1.1f * FMath::Abs(Hitbox.PotentialKnockBack.X), 3);
	}
	else if (Owner->Opponent->CurrentState.bIsAirborne)
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

	if (bVertPositionRelative)
	{
		if (CurrentState.Position.Y > Owner->Opponent->CurrentState.Position.Y)
			Owner->Opponent->CurrentState.KnockBack.Y *= -1;
	}


	Owner->CurrentState.AvailableActions = Hitbox.PotentialActions;
	Owner->Opponent->CurrentState.CharacterHitState = Hitbox.AttackProperties;
	//apply certain modifiers based on circumstances around the hit
	if (Hitbox.AttackHeight < Throw)
	{
		if ((Owner->Opponent->CurrentState.bArmorActive || Owner->Opponent->CurrentState.bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			//Opponent's penalty for getting shattered
			Owner->Opponent->CurrentState.ShatteredTime = 120;
			Owner->Opponent->CurrentState.ResolveRecoverTimer = 0;
			Owner->Opponent->CurrentState.ResolvePulse *= .5f;
			Owner->Opponent->CurrentState.Durability = 0;
			Owner->Opponent->CurrentState.Resolve = 0;

			//Player's reward for landing a shatter
			if (Owner->CurrentState.ResolveRecoverTimer < 180)
				Owner->CurrentState.ResolveRecoverTimer = 180;
			Owner->CurrentState.ResolvePulse += 5;

			HitStunToApply *= 1.2f;
			if (Hitbox.PotentialCounterKnockBack != FVector2D(0))
				KnockBackToApply = Hitbox.PotentialCounterKnockBack;
			Owner->Opponent->CurrentState.CharacterHitState |= Hitbox.CounterAttackProperties;
			//set shatter UI effect to play
		}
		else if ((Owner->Opponent->CurrentState.bArmorActive || Owner->Opponent->CurrentState.bCounterHitState) && !(Hitbox.AttackProperties & Piercing && Owner->Opponent->CurrentState.bArmorActive && Owner->Opponent->CurrentState.Resolve > 0))
		{
			HitStunToApply *= 1.2f;
			if (KnockBackToApply.Y > 0)
				KnockBackToApply.Y *= 1.2f;
			HitStopToApply += HitStopToApply / 2;
			Owner->Opponent->CurrentState.CharacterHitState |= Hitbox.CounterAttackProperties;
			//set counter hit ui effect to play
		}
		else if (Hitbox.AttackProperties & Piercing && Owner->Opponent->CurrentState.bArmorActive && Owner->Opponent->CurrentState.Resolve > 0)
		{
			//set piercing ui effect to play
		}
	}

	if (Owner->Opponent->CurrentState.CharacterHitState & CanMidScreenWallBounce)
		Owner->Opponent->CurrentState.WallBounceTime = 24;
	else
		Owner->Opponent->CurrentState.WallBounceTime = 0;

	if (Owner->Opponent->CurrentState.CurrentAnimation[Owner->Opponent->CurrentState.AnimFrameIndex].Invincibility == OTG)
	{
		DamageToApply = (int32)(.3f * DamageToApply);
		HitStunToApply = (uint8)(.3f * HitStunToApply);
	}

	//calculate proration to apply on subsequent hits in a combo
	if (Owner->CurrentState.ComboCount == 1)
	{
		Owner->CurrentState.SpecialProration = Hitbox.InitProration; //initial proration is only applied if its the first hit of a combo
		Owner->CurrentState.bTrueCombo = true; //reset to true since a new combo has started
		if ((Owner->Opponent->CurrentState.bArmorActive || Owner->Opponent->CurrentState.bCounterHitState) && Hitbox.AttackProperties & Shatter)
		{
			Owner->CurrentState.SpecialProration *= 1.2f;
			DamageToApply *= 1.2f;
		}
		else if (Owner->Opponent->CurrentState.bCounterHitState)
			Owner->CurrentState.SpecialProration *= 1.1f;
	}
	else if (Owner->CurrentState.ComboCount > 1 && Hitbox.ForcedProration != 1)
		Owner->CurrentState.SpecialProration *= Hitbox.ForcedProration; //forced proration is applied as long as the move is used in combo

	//apply damage, damage is scaled by the number of hits in a combo
	int32 ComboProration;
	if (Owner->CurrentState.ComboCount < 3)
		ComboProration = 10;
	else if (Owner->CurrentState.ComboCount < 5)
		ComboProration = 8;
	else if (Owner->CurrentState.ComboCount < 6)
		ComboProration = 7;
	else if (Owner->CurrentState.ComboCount < 7)
		ComboProration = 6;
	else if (Owner->CurrentState.ComboCount < 8)
		ComboProration = 5;
	else if (Owner->CurrentState.ComboCount < 9)
		ComboProration = 4;
	else if (Owner->CurrentState.ComboCount < 10)
		ComboProration = 3;
	else
		ComboProration = 2;

	DamageToApply = FMath::FloorToInt(DamageToApply * ComboProration / 10);

	if (Hitbox.AttackProperties & IsSuper)
		DamageToApply = FMath::Max((int32)(OpponentValor * Hitbox.BaseDamage * .25f), DamageToApply); //Supers will always deal a minimum of 25% their base damage affected by valor

	DamageToApply = FMath::Max(1, DamageToApply); //non-super attacks will always deal a minimum of one damage

	Owner->Opponent->CurrentState.Health -= FMath::Min(DamageToApply, Owner->Opponent->CurrentState.Health);

	if (Owner->Opponent->CurrentState.Health == 0 && Hitbox.AttackProperties & NonFatal)
		Owner->Opponent->CurrentState.Health = 1;

	//apply hitstun, hitstun is scaled by how much time the opponent has spent in hitstun, supers' hitstun is never scaled
	if (Owner->Opponent->CurrentState.bIsAirborne && !(Hitbox.AttackProperties & IsSuper))
	{
		if (Owner->Opponent->CurrentState.ComboTimer > 960 && !(Hitbox.AttackProperties & IsSpecial)) //16 seconds, normal attacks will only deal 1 frame of hitstun
			HitStunToApply = 1;
		else if (Owner->Opponent->CurrentState.ComboTimer > 840) //14 seconds, special attacks have a minimum of 60% their base hitstun
			HitStunToApply *= .6f;
		else if (Owner->Opponent->CurrentState.ComboTimer > 600)//10 seconds
			HitStunToApply *= .7f;
		else if (Owner->Opponent->CurrentState.ComboTimer > 420)//7 seconds
			HitStunToApply *= .8f;
		else if (Owner->Opponent->CurrentState.ComboTimer > 300)//5 seconds
			HitStunToApply *= .9f;
	}
	Owner->Opponent->CurrentState.HitStun = HitStunToApply;
	if (Owner->Opponent->CurrentState.bIsCrouching || Owner->Opponent->IsCurrentAnimation(Owner->Opponent->GuardLo) || Owner->Opponent->IsCurrentAnimation(Owner->Opponent->GuardLoHeavy)) //two extra frames of hitstun if hitting a crouching opponent
		Owner->Opponent->CurrentState.HitStun += 2;

	//apply hitstop
	Owner->Opponent->CurrentState.HitStop = HitStopToApply;
	CurrentState.HitStop = HitStopToApply;

	//meter gain for opponent character
	if (Owner->Opponent->CurrentState.ShatteredTime == 0)
		Owner->Opponent->CurrentState.Durability += FMath::Max((int32)(Hitbox.BaseDamage * FMath::Max(1.f, Owner->Opponent->CurrentState.ResolvePulse * .5f)), 1);

	//increase ResolvePulse
	if (!(Hitbox.AttackProperties & IsSuper))
	{
		Owner->CurrentState.ResolvePulse += (float)Hitbox.BaseDamage * .005f;
	}

	//Make certain actions available for hitting with an attack
	Owner->CurrentState.AvailableActions = Hitbox.PotentialActions;

	//Apply knockback to opponent
	if (bHoriPositionRelative)
	{
		if (CurrentState.Position.X > Owner->Opponent->CurrentState.Position.X)
			KnockBackToApply *= FVector2D(-1, 1);
		else if (CurrentState.Position.X < Owner->Opponent->CurrentState.Position.X)
		{
		}
		else if (!CurrentState.bFacingRight)
		{
			KnockBackToApply *= FVector2D(-1, 1);
		}
	}
	else if (!CurrentState.bFacingRight)
	{
		KnockBackToApply *= FVector2D(-1, 1);
	}

	Owner->Opponent->CurrentState.KnockBack = KnockBackToApply;

	//place and play hit effect
	//place at midpoint between hitbox center and hurtbox center
	if (!(Hitbox.AttackProperties & NoHitEffect))
	{
		if (Owner->Opponent)
		{
			if (Owner->SpecialVFX[0]->CurrentState.bIsActive)
			{
				Owner->Opponent->SpecialVFX[0]->Activate(Owner->IntersectCenter, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties);
			}
			else
				Owner->SpecialVFX[0]->Activate(Owner->IntersectCenter, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties);
		}
	}
}

void ABTProjectileBase::ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter)
{
	//If the opponent would be on the ground on the next frame, treat them as if they were hit while on the ground
	if (Owner->Opponent->CurrentState.HitStun == 0 && Owner->Opponent->CurrentState.bIsAirborne && CurrentState.Position.Y + CurrentState.Velocity.Y * 100 / 60.f <= 0)
	{
		Owner->Opponent->CurrentState.Position.Y = 0;
		Owner->Opponent->CurrentState.Velocity.Y = 0;
		Owner->Opponent->CurrentState.bIsAirborne = false;
		if (Owner->Opponent->CurrentState.Dir1 == 10 || Owner->Opponent->CurrentState.Dir2 == 10 || Owner->Opponent->CurrentState.Dir3 == 10)
		{
			Owner->Opponent->CurrentState.bIsCrouching = true;
		}
	}


	//Opponent successfully guarded the attack
	if (((Owner->Opponent->CurrentState.bIsGuarding && Owner->Opponent->CurrentState.bIsAirborne) || (Owner->Opponent->CurrentState.bIsGuarding && Hitbox.AttackHeight <= High) || (Owner->Opponent->CurrentState.bIsGuarding && !Owner->Opponent->CurrentState.bIsAirborne && Owner->Opponent->CurrentState.bIsCrouching && Hitbox.AttackHeight == Low) ||
		(Owner->Opponent->CurrentState.bIsGuarding && !Owner->Opponent->CurrentState.bIsAirborne && !Owner->Opponent->CurrentState.bIsCrouching && Hitbox.AttackHeight == Overhead)) && Hitbox.AttackHeight != Unblockable)
	{
		int32 BlockstunToApply;
		FVector2D KnockBackToApply;
		if (Hitbox.BaseBlockStun == 0)
			BlockstunToApply = Hitbox.BaseHitStun * .8f;
		else
			BlockstunToApply = Hitbox.BaseBlockStun;

		if (Owner->Opponent->CurrentState.bIsAirborne)
		{
			if (FMath::Abs(Hitbox.PotentialAirKnockBack.X) > FMath::Abs(Hitbox.PotentialAirKnockBack.Y) || Hitbox.PotentialAirKnockBack.X < 0)
				KnockBackToApply = FVector2D(Hitbox.PotentialAirKnockBack.X, 1.f);
			else
				KnockBackToApply = FVector2D(.5f * (Hitbox.PotentialAirKnockBack.X + FMath::Abs(Hitbox.PotentialAirKnockBack.Y)), 1.f);
		}
		else
		{
			if (FMath::Abs(Hitbox.PotentialKnockBack.X) > FMath::Abs(Hitbox.PotentialKnockBack.Y) || Hitbox.PotentialKnockBack.X < 0)
				KnockBackToApply = FVector2D(Hitbox.PotentialKnockBack.X, 0);
			else
				KnockBackToApply = FVector2D(.5f * (Hitbox.PotentialKnockBack.X + FMath::Abs(Hitbox.PotentialKnockBack.Y)), 0);
		}

		BlockstunToApply = FMath::Min(BlockstunToApply, 30);

		if (Owner->Opponent->CurrentState.JustDefense >= 0) //if the opponent Instant Blocked the attack
		{
			//Cuts blockstun by a third on Just defense, cuts by 2 frame at minimum
			if (BlockstunToApply < 9)
				BlockstunToApply -= 2;
			else
				BlockstunToApply = BlockstunToApply * 2 / 3;

			BlockstunToApply = FMath::Max(1, BlockstunToApply);
			KnockBackToApply *= 0;
			
			//reward opponent for blocking with exceptional timing
			Owner->Opponent->CurrentState.Durability += 250;
			Owner->Opponent->CurrentState.ResolvePulse += 3;
			Owner->Opponent->CurrentState.JustDefense = 0;
			if (Owner->Opponent->CurrentState.ResolveRecoverTimer < 180)
				Owner->Opponent->CurrentState.ResolveRecoverTimer = 180;

			//make opponent flash white
			Owner->Opponent->StatusMix = .75f;
			Owner->Opponent->CurrentState.StatusTimer = 5;
			Owner->Opponent->StatusColor = FVector(1);
			UE_LOG(LogTemp, Warning, TEXT("JUST DEFEND")); //ui Instant block effect "Instant"
		}
		else
		{
			Owner->Opponent->CurrentState.ResolveRecoverTimer = FMath::Max(0, (int32)Owner->Opponent->CurrentState.ResolveRecoverTimer - 24);

			//blocked hits chip away at durability
			if (Owner->Opponent->CurrentState.Resolve > 0)
			{
				if (Owner->CurrentState.bIsAirborne)
				{
					if (Hitbox.AttackProperties & AntiAir) //attacks with the anti air property must be Instant Blocked
					{
						Owner->Opponent->CurrentState.ResolvePulse--;
						AttackCalculation(Hitbox, HurtboxCenter);
						return;
					}

					if (Hitbox.AttackProperties & IsSuper)
						Owner->Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * 4);
					else if (Hitbox.AttackProperties & IsSpecial)
						Owner->Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * 3);
					else
						Owner->Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * 2);
				}
				else
				{
					if (Hitbox.AttackProperties & IsSuper)
						Owner->Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * 2);
					else if (Hitbox.AttackProperties & IsSpecial)
						Owner->Opponent->CurrentState.Durability -= (int32)(Hitbox.BaseDamage * 1.5);
					else
						Owner->Opponent->CurrentState.Durability -= Hitbox.BaseDamage;
				}

			}
			else
			{
				//deal chip damage if the opponent has no resolve left, shatter their guard if they are airborne
				if (Owner->Opponent->CurrentState.bIsAirborne)
				{
					Owner->Opponent->CurrentState.bCounterHitState = true;
					AttackCalculation(Hitbox, HurtboxCenter);
					return;
				}
				else
				{
					int32 ChipDamage = (int32)(Hitbox.BaseDamage * .2f);

					if (Owner->Opponent->CurrentState.Health - ChipDamage <= 0 && Owner->Opponent->CurrentState.Health > 1) //Chip damage reduces character's life to 1 before being lethal
						ChipDamage = Owner->Opponent->CurrentState.Health - 1;
					else
						ChipDamage = FMath::Min(Owner->Opponent->CurrentState.Health, ChipDamage);

					Owner->Opponent->CurrentState.Health -= ChipDamage;

					if (Owner->Opponent->CurrentState.Health == 0)
					{
						if (Hitbox.AttackProperties & NonFatal)
							Hitbox.AttackProperties -= NonFatal;
						AttackCalculation(Hitbox, HurtboxCenter);
						return;
					}
				}
			}
		}

		//apply final blockstun calculation
		Owner->Opponent->CurrentState.BlockStun = BlockstunToApply;

		if (Owner->Opponent->CurrentState.bIsAirborne && Owner->Opponent->CurrentState.JustDefense < 0) //extra blockstun if the opponent is airborne
		{
			Owner->Opponent->CurrentState.BlockStun += 3;
			Owner->Opponent->CurrentState.JumpsUsed = 0;
		}

		//apply hitstop
		Owner->Opponent->CurrentState.HitStop = Hitbox.BaseHitStop - 2;
		CurrentState.HitStop = Hitbox.BaseHitStop - 2;

		if (KnockBackToApply.X > 0)
			KnockBackToApply.X = FMath::Max(1.75f, KnockBackToApply.X);

		Owner->Opponent->CurrentState.KnockBack = KnockBackToApply;

		if (bHoriPositionRelative)
		{
			if (CurrentState.Position.X > Owner->Opponent->CurrentState.Position.X)
				Owner->Opponent->CurrentState.KnockBack *= FVector2D(-1, 1);
			else if (CurrentState.Position.X < Owner->Opponent->CurrentState.Position.X)
			{
			}
			else if (!CurrentState.bFacingRight)
			{
				Owner->Opponent->CurrentState.KnockBack *= FVector2D(-1, 1);
			}
		}
		else if (!CurrentState.bFacingRight)
		{
			Owner->Opponent->CurrentState.KnockBack *= FVector2D(-1, 1);
		}

		if (bVertPositionRelative)
		{
			if (CurrentState.Position.Y > Owner->Opponent->CurrentState.Position.Y)
				Owner->Opponent->CurrentState.KnockBack.Y *= -1;
		}

		//increase ResolvePulse
		Owner->CurrentState.ResolvePulse += (float)Hitbox.BaseDamage * .001f;

		//Made contact so can cancel into other actions
		Owner->CurrentState.AvailableActions = Hitbox.PotentialActions;
		Owner->CurrentState.AvailableActions &= AcceptAll - AcceptJump - AcceptBlitz;

		if (Hitbox.PotentialActions & JumpCancelOnBlock && Hitbox.PotentialActions & AcceptJump) //can only jump cancel if the move allows it on block
			Owner->CurrentState.AvailableActions |= AcceptJump;

		if (!(Hitbox.PotentialActions & BlitzOnHitOnly) && Hitbox.PotentialActions & AcceptBlitz) //can blitz cancel if it allows it on block
			Owner->CurrentState.AvailableActions |= AcceptBlitz;

		//update opponent's animation to guarding
		if (Owner->Opponent->CurrentState.bIsAirborne)
			Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardAir);
		else
		{
			if (Owner->Opponent->CurrentState.bIsCrouching)
			{
				if (Hitbox.AttackProperties & IsHeavy)
					Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardLoHeavy);
				else
					Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardLo);
			}
			else
			{
				if (Hitbox.AttackProperties & IsVertical)
					Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardHiVertical);
				else if (Hitbox.AttackProperties & IsHeavy)
					Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardHiHeavy);
				else
					Owner->Opponent->EnterNewAnimation(Owner->Opponent->GuardHi);
			}
		}

		//place and play guard effect
		//place at midpoint between hitbox center and hurtbox center
		if (Owner->Opponent)
		{
			FVector2D ImpactPoint = FVector2D(Owner->Opponent->CurrentState.Position.X - .5 * Owner->Opponent->PushboxWidth, Owner->IntersectCenter.Y);

			if (Owner->Opponent->CurrentState.bFacingRight)
				ImpactPoint.X = Owner->Opponent->CurrentState.Position.X + .5 * Owner->Opponent->PushboxWidth;

			if (Owner->SpecialVFX[1]->CurrentState.bIsActive)
			{
				Owner->Opponent->SpecialVFX[1]->Activate(ImpactPoint, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties, Guard);
			}
			else
				Owner->SpecialVFX[1]->Activate(ImpactPoint, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties, Guard);
		}
	}
	else if (Owner->Opponent->CurrentState.bArmorActive && Owner->Opponent->CurrentState.Resolve > 0 && !(Hitbox.AttackProperties & Piercing) && !(Hitbox.AttackProperties & Shatter))
	{
		Owner->Opponent->CurrentState.ResolveRecoverTimer = FMath::Max(0, (int32)Owner->Opponent->CurrentState.ResolveRecoverTimer - 48);
		Owner->Opponent->CurrentState.ResolvePulse /= 2;
		Owner->Opponent->CurrentState.Durability -= Hitbox.DurabilityDamage;
		Owner->Opponent->CurrentState.Resolve -= Hitbox.ResolveDamage;
		Owner->CurrentState.HitStop = Hitbox.BaseHitStop;
		Owner->Opponent->CurrentState.HitStop = CurrentState.HitStop;
		//make opponent flash red
		Owner->Opponent->StatusMix = .8f;
		Owner->Opponent->CurrentState.StatusTimer = 5;
		Owner->Opponent->StatusColor = FVector(1, 0, 0);

		//available actions are more limited when hitting an opponent's armor
		if (Hitbox.PotentialActions & AcceptSpecial)
			Owner->CurrentState.AvailableActions |= AcceptSpecial;
		if (Hitbox.PotentialActions & AcceptSuper)
			Owner->CurrentState.AvailableActions |= AcceptSuper;
		if (Hitbox.PotentialActions & AcceptJump)
			Owner->CurrentState.AvailableActions |= AcceptJump;
		if (Hitbox.PotentialActions & AcceptBlitz)
			Owner->CurrentState.AvailableActions |= AcceptBlitz;

		//place and play armor hit effect
		//place at midpoint between hitbox center and hurtbox center
		if (Owner->Opponent)
		{
			FVector2D ImpactPoint = FVector2D(Owner->Opponent->CurrentState.Position.X - .5 * Owner->Opponent->PushboxWidth, Owner->IntersectCenter.Y);
			uint8 ResolveHitType = Resolve;

			if (Hitbox.ResolveDamage > 0 || Hitbox.DurabilityDamage > 500)
				ResolveHitType = HeavyResolve;

			if (Owner->Opponent->CurrentState.bFacingRight)
				ImpactPoint.X = Owner->Opponent->CurrentState.Position.X + .5 * Owner->Opponent->PushboxWidth;

			if (Owner->SpecialVFX[1]->CurrentState.bIsActive)
			{
				Owner->Opponent->SpecialVFX[1]->Activate(ImpactPoint, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties, ResolveHitType);
			}
			else
				Owner->SpecialVFX[1]->Activate(ImpactPoint, Owner->Opponent->CurrentState.bFacingRight, Hitbox.AttackProperties, ResolveHitType);
		}
	}
	else //the attack hit the opponent
	{
		CurrentState.bHitSuccess = true;
		AttackCalculation(Hitbox, HurtboxCenter);
	}
}

void ABTProjectileBase::ContactThrow(FHitbox Hitbox, int32 ThrowType)
{
	CurrentState.bAttackMadeContact = true;
	CurrentState.bHitSuccess = true;
	AttackCalculation(Hitbox, Owner->Opponent->CurrentState.Position);
}

void ABTProjectileBase::CreateMaterials(){ }
