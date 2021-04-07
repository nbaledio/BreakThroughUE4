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