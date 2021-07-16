// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "CharacterSelectForeground.h"

void UCharacterSelectForeground::PlayBackMenuSlide() 
{
	StopAnimation(BackMenuSlide);
	PlayAnimation(BackMenuSlide, 0.0f, 1, EUMGSequencePlayMode::Forward, 6.0f, false);
}

void UCharacterSelectForeground::PlayBackMenuSlideReverse()
{
	StopAnimation(BackMenuSlide);
	PlayAnimation(BackMenuSlide, 0.0f, 1, EUMGSequencePlayMode::Reverse, 6.0f, false);
}
