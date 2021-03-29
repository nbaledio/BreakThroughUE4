// Copyright 2021 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakThroughPlayerController.h"
#include "BTCharacterBase.generated.h"

#define OUT

enum GuardHeight
{
	Mid,
	High,
	Overhead,
	Low,
	Unblockable,
	Throw,
	CommandThrow,
	AirThrow,
	AirCommandThrow
};

enum SpecialStates
{
	None = 0,
	ProjectileInvincible,
	StrikeInvincible,
	ThrowInvincible,
	FullInvincible,
	Intangible,
	HiCounter,
	LowCounter,
	AllCounter,
	SuperCounter,
	FaceDown,
	FaceUp,
	OTG // 30% damage and halved hitstun
};

enum CharacterActions
{
	AcceptMove = (1 << 0),
	AcceptGuard = (1 << 1),
	AcceptJump = (1 << 2),
	AcceptLight = (1 << 3),
	AcceptMedium = (1 << 4),
	AcceptHeavy = (1 << 5),
	AcceptBreak = (1 << 6),
	AcceptCommandNormal = (1 << 7),
	AcceptSpecial = (1 << 8),
	AcceptSuper = (1 << 9),
	AcceptBlitz = (1 << 10),
	AcceptAll = AcceptMove + AcceptGuard + AcceptJump + AcceptLight + AcceptMedium + AcceptHeavy + AcceptBreak + AcceptCommandNormal + AcceptSpecial + AcceptSuper + AcceptBlitz,
	ThrowTech = (1 << 11),
	JumpCancelOnBlock = (1 << 12),
};

enum AttackProperties
{
	Piercing = (1 << 0), //Attack ignores armor
	Shatter = (1 << 1), //Attack destroys armor
	CanGroundBounce = (1 << 2), //can be bounced against the ground
	CanWallBounce = (1 << 3), //can be bounced off of walls
	CanWallStick = (1 << 4), //can be stuck against walls, wall stick transitions into crumple if hits the ground before wallsticktime is up
	CanSweep = (1 << 5), //sweep: no special properties
	CanLaunch = (1 << 6), //launch: no special properties, purely aesthetic
	CanStagger = (1 << 7), //stagger: can still be thrown despite being a hitstun state, need to hold button to recover back to standing position once hitstun ends
	CanCrumple = (1 << 8), //crumple: long hitstun state with preset duration, ends in facedown knockdown, can still be thrown despite being a hitstun state
	CanKnockAway = (1 << 9), //knock away: no special properties, purely aesthetic
	CanDeflect = (1 << 10), //deflect: non-deflect attacks are deflected by a hitbox with this property, character enters a hitstun state with preset duration, can still be thrown, two deflective attacks will clash normally
	CanTumble = (1 << 11), //tumbling: an airborne hitstun state that cannot be air recovered from
	ComboThrow = (1 << 12), //Throws with this flag can hit opponents even if they are in hitstun
	PlayHitEffect = (1 << 13),
	IsSpecial = (1 << 14),
	IsSuper = (1 << 15),
	IsSlash = (1 << 16),
	IsVertical = (1 << 17),
	IsHorizontal = (1 << 18),
	LowerBodyHit = (1 << 19),
};

USTRUCT(BlueprintType)
struct FHurtbox
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hurtbox = character position + hurtbox position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position
};

USTRUCT(BlueprintType)
struct FHitbox
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Position; //position of hitbox = character position + hitbox position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dimensions")
		FVector2D Size; //hurtbox size radiates out from above position

	//attack properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bNewHit = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 PotentialActions; //denotes the actions that become available to the character upon the hitbox making contact using bit flags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackProperties; // denotes the properties an attack has on normal hit using bit flags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 CounterAttackProperties; // denotes the properties an attack has on counter using bit flags

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 DurabilityDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 ResolveDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackLevel = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackHeight = Mid;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStun = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseHitStop = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		uint8 BaseBlockStun = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float InitProration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		float ForcedProration = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialKnockBack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D PotentialAirKnockBack;
};

USTRUCT(BlueprintType)
struct FAnimationFrame
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimationAsset* Pose;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		uint8 PlayDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bDoesCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bSuperFlash; //freezes Opposing character in place

	//relinquish complete control of camera positioning to animation/character class, camera will snap to stored positions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bCinematic;

	//camera location relative to character's position, will snap to location if bCinematic is also on, otherwise will lerp to new positions, must be a non zero vector to use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FVector CameraLocation;
	//camera rotation, will snap to rotation if bCinematic is also on, otherwise will lerp to new rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FRotator CameraRotation;
	//Changes the main lighting angle on the character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FVector MainLightVector;
	//Changes the fill light angle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FVector FillLightVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitboxes")
		TArray<FHitbox> Hitboxes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hurtboxes")
		TArray<FHurtbox> Hurtboxes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AvailableActions; //uses CharacterActions enum, determines what actions are available to the player upon entering this animation frame
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 Invincibility; //uses SpecialStates enum, determines what attacks can hit the character, used to dictate a face up or face down knockdown on airborne hitstun states
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bIsCrouching = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bArmorActive = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bCounterHitState = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 LandingLag = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AutoGuardProximity = 0; //opposing character will stop moving and automatically put up guard if they hold back while within this distance of the character

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bStopVelocityX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bStopVelocityY;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bLockPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		FVector2D Acceleration; //add to character's velocity upon entering the animation frame
};

UCLASS()
class BREAKTHROUGH_API ABTCharacterBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABTCharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime);

	void SuperFlashSolver();

	virtual void HitDetection();

	virtual void UpdateCharacter(int32 CurrentInputs);

	void VelocitySolver(); //only called once by gamestate, do not call for each character

	virtual void UpdatePosition();

	void PushboxSolver(); //only called once by gamestate, do not call for each character

	virtual void DrawCharacter();

	ABTCharacterBase* Opponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* BaseMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay();

	virtual void ProcessInputs(int32 Inputs);

	virtual void AnimationStateMachine();

	virtual bool ActiveTransitions(); //Animation transitions triggered by player input

	virtual bool ConditionalTransitions(); //Animation transitions triggered by fulfilling some condition within certain animations

	virtual bool PassiveTransitions(); //Transitions that occur without the influence of outside factors or player input

	virtual bool ExitTimeTransitions(); //Animation transitions triggered by finishing an animation, called from within PassiveTransitions

	virtual void AnimationEvents();

	bool QCF();

	bool QCB();

	bool HCF();

	bool HCB();

	bool DP();

	bool RDP();

	bool FC();

	void RefreshMovelist();

	TArray<FAnimationFrame>* CurrentAnimation;
	FAnimationFrame* CurrentAnimFrame;
	TArray<FHitbox>* CurrentHitbox;
	TArray<FHurtbox>* CurrentHurtbox;

	uint8 AnimFrameIndex;
	uint8 PosePlayTime = 0;
	uint8 IdleCycle = 0;

	TArray<int32> InputHistory;
	int32 Seals = 0;
	int32 SealTimer = 0;

	//character's vitality, loses when it hits zero
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		int32 Health;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		int32 MaxHealth = 1000;

	//the resolve that must be broken through
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		uint8 Resolve = 4;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		int32 Durability = 100;

	uint8 ResolveRecoverTimer; //Resolve starts passive recovery after 3 seconds of not being used and while not shattered, does not increment while shattered
	uint8 RecoverInterval; //dictates how quickly Resolve replenishes, recovers more quickly the lower the character's life, doubled while in slow mo

	//values that represent a character's resilience as a battle rages on
	//scales down damage received based on how low the character's health is
	/*UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		float CurrentValor;*/
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor100;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor50;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor25;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor10;

	UPROPERTY(EditAnywhere, Category = "Battle Stats")
		bool bFacingRight = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 HitStun = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 BlockStun = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 LandingLag = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 HitStop = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 GravDefyTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 ShatteredTime = 0;
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		uint8 SlowMoTime = 0;
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		uint8 WallStickTime = 0;

	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		uint8 JumpsUsed = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsAirborne = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsGuarding = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bTouchingWall = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bTouchingOpponent = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 JustDefense = 5;

	/* Affects how quickly the character falls to the ground (See below for values per weight class)
		Featherweight = .95 
		Lightweight = .98
		Middleweight = 1
		Heavyweight = 1.03  */
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float Weight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float StandingPushBoxHeight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float CrouchingPushBoxHeight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float PushboxWidth = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float AirPushboxVerticalOffset = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkBackSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float RunAcceleration = .2f; //set RunAcceleration to zero to enable dash type
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float InitRunSpeed = 2; //set InitRunSpeed to zero to disable run/dash
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float BlitzDashForce = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		uint8 MaxJumps = 2;
	//x dictates horizontal acceleration, z dictates vertical acceleration. y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D BackDashForce;
	//x dictates horizontal acceleration during forward/backward jumps, z dictates vertical acceleration, y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D JumpForce;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Position; // Y = 0 is considered the ground, X = (-/+)10 are the left and right walls respectively
	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties") //keeps track of non-movement acceleration to apply once hitstop is zero
		FVector2D KnockBack;

	//value that increasingly scales positive vertical knockback the longer a character is in a combo
	//causes a character to not be launched as high the more hits there are in a combo
		int ComboCount = 0; //keeps track of the number of hits in a combo performed by this character
		int ComboTimer = 0; //keeps track of the amount of time this character has spent in hitstun in frames
		bool bTrueCombo = true; //keeps track of whether or not a character could have escaped a combo at some point
		float SpecialProration = 1;

	//number of frames that an input is active for
		uint8 InputTime = 10;
	// ints to denote active time on directional inputs
		uint8 Dir1 = 0;
		uint8 Dir2 = 0;
		uint8 Dir3 = 0;
		uint8 Dir4 = 0;
		uint8 Dir6 = 0;
		uint8 Dir7 = 0;
		uint8 Dir8 = 0;
		uint8 Dir9 = 0;
		uint8 DoubleDir2 = 0;
		uint8 DoubleDir6 = 0;
		uint8 DoubleDir4 = 0;
		uint8 AirJump = 0;
		bool Resolute; // Set to true when no inputs are held down

		uint8 Charge2 = 0;
		uint8 Charge4 = 0;
		//uint8 Charge5 = 0;
		uint8 Charge6 = 0;
		uint8 Charge8 = 0;

		uint8 Charge2Life = 0;
		uint8 Charge4Life = 0;
		//uint8 Charge5Life = 0;
		uint8 Charge6Life = 0;
		uint8 Charge8Life = 0;

	// ints to denote active time on button inputs
		uint8 LPressed = 0;
		uint8 MPressed = 0;
		uint8 HPressed = 0;
		uint8 BPressed = 0;
		uint8 LReleased = 0;
		uint8 MReleased = 0;
		uint8 HReleased = 0;
		uint8 BReleased = 0;
	
	//booleans to track if buttons are being held down
		bool bIsLDown;
		bool bIsMDown;
		bool bIsHDown;
		bool bIsBDown;

	//int using bit flags to track to actions available to the character
		int32 AvailableActions;
		int32 MoveList;

	//booleans to dictate the character's current state
		bool bIsRunning = false;
		bool bForwardJump = false;
		bool bBackwardJump = false;
		bool bArmorActive = false;
		bool bCounterHitState = false;
		int32 CharacterHitState = None; //determines if character should be in a specific animation state (crumple, sweep, launch, etc.), or if they can be bounced against/ stuck to surfaces, uses AttackProperties enum

	//keeps track of whether an attack has already hit something
	//attack effects are only applied based on the first overlap interaction with the attack (!bAttackMadeContact)
		bool bAttackMadeContact = false;
	//keeps track if an attack makes a hit, used for attacks that have a followup when they hit
		bool bHitSuccess = false;
		bool bClash = false;
		bool bBlitzing;

	//Sets the corresponding parameters on character's materials
		FVector MainLightVector;
		FVector FillLightVector;
		FVector MainLightColor;
		FVector FillLightColor;
		FVector RimLightColor;
		FVector StatusColor;
		float StatusMix; //.8f for armor hit (red), 3 for air recover and instant block (white)
		int32 StatusTimer; //only mixes with status color as long as this is greater than zero
		float LightIntensity;
		float OverallBrightness;
		float LineThickness; //0-1 during cinematics, 2 during normal gameplay

	//Idle Stance Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> IdleStand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> IdleStandBlink;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> StandIdleAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> IdleCrouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> IdleCrouchBlink;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> CrouchIdleAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> TurnAroundStand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> TurnAroundCrouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> StandUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> CrouchDown;

	//Locomotion Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WalkForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WalkBackward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> PreJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> NeutralJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> ForwardJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> BackwardJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> MidJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> JumpTransition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> JumpDescent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> RunStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> RunCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> Brake;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> BackDash;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> AirDashForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> AirDashBackward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> AirRecovery;

	//Guard Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardHiIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardHi;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardHiHeavy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardHiVertical;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardHiOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardLoIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardLo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardLoHeavy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardLoOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Animations")
		TArray<FAnimationFrame> GuardAirOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> ThrowEscape;

	//Ground Hitstun Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSHIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSHOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSHHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSHHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSLIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSLOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSLHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitSLHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitCIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitCOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitCHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitCHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> Deflected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> Crumple;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> Stagger;

	//Airborne hitstun animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitstunAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> HitstunAirCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> KnockAway;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> LaunchCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> LaunchTransition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> LaunchFallCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> Tumble;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WallBounce; //sweep
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> FallingForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> GroundBounce;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WallStick;

	//Blitz Cancel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> FocusBlitz;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> BreakerBlitz;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> BlitzOutAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> BlitzOutStanding;

	//Knockdown/WakeUp Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> KnockDownFaceDown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> KnockDownFaceUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WakeUpFaceDown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
		TArray<FAnimationFrame> WakeUpFaceUp;

private:

	//Take in information from CurrentAnimFrame
	void ProcessAnimationFrame();

	bool EnterNewAnimation(TArray<FAnimationFrame> Animation, int32 FrameNumber = 0);

	void TurnAroundCheck();

	void TriggerTurnAround();

	bool SurfaceContact();

	bool HitWall();

	void Guarding();

	void LandingLagCheck();
	
	void RunBraking();

	void Jumping();

	void GravityCalculation();

	void ApplyKnockBack();

	void UpdateResolve();

	void ChargeInputs(int32 Inputs);

	void DirectionalInputs(int32 Inputs);

	void ButtonInputs(int32 Inputs);

	void InputCountdown();

	bool RectangleOverlap(FVector2D Pos1, FVector2D Pos2, FVector2D Size1, FVector2D Size2);

	void ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactThrow(FHitbox Hitbox, int32 ThrowType);

	void AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter);
};

/* 
 Basic Gamestate Tick:

 P1->SuperFlash();

 P1->HitDetection();
 P2->HitDetection(); //possible to change animations and other information pertaining to hit state

 //record inputs and store in queue, process inputs based on values that may have been affected by HitDetection, 
 //transition to new animation if required and process new animation data, initial velocity changes
 P1->UpdateCharacter();
 P2->UpdateCharacter();

 P1->VelocitySolver(); // all velocity changes need to occur before this step

 P1->UpdatePosition(); //update character location based on the character's velocity and decrement various timed character state values
 P2->UpdatePosition(); //DO NOT CHANGE ANY INFO THAT THE OTHER CHARACTER NEEDS TO READ IN THEIR UPDATE

 P1->PushboxSolver();

 GameState->Timer, round count, round wins, increment frame number, update camera transform drivers
 GameState->SaveGameState

 All of the above is looped x times when a rollback needs to occur

 P1->DrawCharacter();
 P2->DrawCharacter();

 update UI, update camera position
*/
