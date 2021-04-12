// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterACH.h"

void ABTCharacterACH::HitDetection()
{
	ABTCharacterBase::HitDetection();
}

void ABTCharacterACH::UpdateCharacter(int32 CurrentInputs)
{
	ABTCharacterBase::UpdateCharacter(CurrentInputs);
}

void ABTCharacterACH::UpdatePosition()
{
	ABTCharacterBase::UpdatePosition();
}

void ABTCharacterACH::DrawCharacter()
{
	ABTCharacterBase::DrawCharacter();
}

bool ABTCharacterACH::NonKnockdownLanding()
{
	return ABTCharacterBase::NonKnockdownLanding();
}

bool ABTCharacterACH::ActiveTransitions()
{
	if ((CurrentState.AvailableActions & AcceptBlitz || (CurrentState.BlockStun > 0 && !CurrentState.bIsAirborne)) && CurrentState.Resolve > 0 &&
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
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 0);
				CurrentState.Velocity = FVector2D(BlitzDashForce, 0);
				CurrentState.GravDefyTime = 24;
				if (!CurrentState.bFacingRight)
					CurrentState.Velocity.X *= -1;

				CurrentState.bBlitzing = true;

				return EnterNewAnimation(AirDashForward);
			}
			if (CurrentState.Dir4 == InputTime) //blitz air dash backward
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 0);
				CurrentState.Velocity = FVector2D(-BlitzDashForce, 0);
				CurrentState.GravDefyTime = 24;
				if (!CurrentState.bFacingRight)
					CurrentState.Velocity.X *= -1;

				CurrentState.bBlitzing = true;

				return EnterNewAnimation(AirDashBackward);
			}
			if (CurrentState.Dir2 == InputTime) //blitz downward
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 0);
				CurrentState.Velocity = FVector2D(0, -BlitzDashForce);
				CurrentState.GravDefyTime = 0;

				CurrentState.bBlitzing = true;

				return EnterNewAnimation(JumpDescent);
			}
			if (CurrentState.AvailableActions & AcceptMove) //focus blitz
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 2);

				return EnterNewAnimation(FocusBlitz);
			}

			BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 0);
			CurrentState.bBlitzing = true;
			return EnterNewAnimation(MidJump);
		}
		else
		{
			if (CurrentState.BlockStun > 0 && CurrentState.Resolve > 1) //breaker blitz
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 1);

				CurrentState.Resolve--;

				return EnterNewAnimation(BreakerBlitz);
			}
			if (CurrentState.AvailableActions & AcceptMove) //focus blitz
			{
				BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 2);

				return EnterNewAnimation(FocusBlitz);
			}

			BlitzImage->Activate(CurrentState.Position, CurrentState.CurrentAnimFrame->Pose, CurrentState.bFacingRight, 0);
			CurrentState.bBlitzing = true;
			return EnterNewAnimation(IdleStand);
		}
	}

	return ABTCharacterBase::ActiveTransitions();
}

bool ABTCharacterACH::ConditionalTransitions()
{
	return ABTCharacterBase::ConditionalTransitions();
}

bool ABTCharacterACH::PassiveTransitions()
{
	return ABTCharacterBase::PassiveTransitions();
}

bool ABTCharacterACH::ExitTimeTransitions()
{
	return ABTCharacterBase::ExitTimeTransitions();
}

void ABTCharacterACH::AnimationEvents()
{
	ABTCharacterBase::AnimationEvents();
}