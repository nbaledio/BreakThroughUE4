// Copyright 2021 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTCharacterBase.h"
#include "BTCharacterACH.generated.h"

enum Moves  //keeps track of what moves have already been used in a string using bit flags
{
	n5L = (1 << 0), //used for both air and ground normals
	n5M = (1 << 1), 
	n5H = (1 << 2),
	n5B = (1 << 3),
	n2L = (1 << 4),
	n2M = (1 << 5),
	n2H = (1 << 6),
	n2B = (1 << 7),
	/*CanCrumple = (1 << 8),
	CanKnockAway = (1 << 9),
	CanDeflect = (1 << 10), 
	CanTumble = (1 << 11), 
	ComboThrow = (1 << 12), 
	PlayHitEffect = (1 << 13),
	IsSpecial = (1 << 14),
	IsSuper = (1 << 15),
	IsSlash = (1 << 16),
	IsVertical = (1 << 17),
	IsHorizontal = (1 << 18),
	LowerBodyHit = (1 << 19),*/
};

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTCharacterACH : public ABTCharacterBase
{
	GENERATED_BODY()

public:
	virtual void HitDetection() override;

	virtual void UpdateCharacter(int32 CurrentInputs) override;

	virtual void UpdatePosition() override;

	virtual void DrawCharacter() override;

protected:
	virtual bool NonKnockdownLanding() override; //transitions triggered by touching the ground while not in a hitstun animation

	virtual bool ActiveTransitions() override; //Animation transitions triggered by player input

	virtual bool ConditionalTransitions() override; //Animation transitions triggered by fulfilling some condition within certain animations

	virtual bool PassiveTransitions() override; //Transitions that occur without the influence of outside factors or player input

	virtual bool ExitTimeTransitions() override; //Animation transitions triggered by finishing an animation, called from within PassiveTransitions

	virtual void AnimationEvents() override;
	
};
