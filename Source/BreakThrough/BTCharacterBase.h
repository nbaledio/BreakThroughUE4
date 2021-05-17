// Copyright 2021 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "BTProjectileBase.h"
#include "Sigil.h"
#include "BlitzImageBase.h"
#include "BreakThroughPlayerController.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
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
	BlitzOnHitOnly = (1 << 13),
};

enum AttackProperties
{
	NoHitEffect = (1 << 0),
	Piercing = (1 << 1), //Attack ignores armor
	Shatter = (1 << 2), //Attack destroys armor
	CanAirGroundBounce = (1 << 3),
	CanGroundBounce = (1 << 4), //can be bounced against the ground
	CanWallBounce = (1 << 5), //can be bounced off of walls
	CanMidScreenWallBounce = (1 << 6), //can be bounced back after a certain number of frames
	CanWallStick = (1 << 7), //can be stuck against walls, wall stick transitions into crumple if hits the ground before wallsticktime is up
	CanSweep = (1 << 8), //sweep: no special properties
	CanLaunch = (1 << 9), //launch: no special properties, purely aesthetic
	CanStagger = (1 << 10), //stagger: can still be thrown despite being a hitstun state, need to hold button to recover back to standing position once hitstun ends
	CanCrumple = (1 << 11), //crumple: long hitstun state with preset duration, ends in facedown knockdown, can still be thrown despite being a hitstun state
	CanKnockAway = (1 << 12), //knock away: no special properties, purely aesthetic
	CanDeflect = (1 << 13), //deflect: non-deflect attacks are deflected by a hitbox with this property, character enters a hitstun state with preset duration, can still be thrown, two deflective attacks will clash normally
	CanTumble = (1 << 14), //tumbling: an airborne hitstun state that cannot be air recovered from
	ForceCrouch = (1 << 15),
	ComboThrow = (1 << 16), //Throws with this flag can hit opponents even if they are in hitstun
	AntiAir = (1 << 17),
	DisableBurst = (1 << 18),
	ReflectProjectile = (1 << 19),
	IsSpecial = (1 << 20),
	IsSuper = (1 << 21),
	IsSlash = (1 << 22),
	IsVertical = (1 << 23),
	IsHorizontal = (1 << 24),
	IsHeavy = (1 << 25),
	LowerBodyHit = (1 << 26),
	ForceStand = (1 << 27),
};

class ABTProjectileBase;
struct FProjectileState;
class ASigil;
struct FSigilState;
class ABlitzImageBase;
struct FBlitzState;
class UAnimationFrame;

USTRUCT(BlueprintType)
struct FAnimationFrame
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimationAsset* Pose;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		uint8 PlayDuration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bDoesCycle = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bSuperFlash = false; //freezes Opposing character in place

	//relinquish complete control of camera positioning to animation/character class, camera will snap to stored positions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		bool bCinematic = false;

	//camera location relative to character's position, will snap to location if bCinematic is also on, otherwise will lerp to new positions, must be a non zero vector to use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FVector CameraLocation;
	//camera rotation, will snap to rotation if bCinematic is also on, otherwise will lerp to new rotation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		FRotator CameraRotation;
	//Changes the main lighting angle on the character, snaps to stored positions while bCinematic, otherwise lerps to new positions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FRotator MainLightRotation;
	//Changes the fill light angle
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FRotator FillLightRotation;
	//overrides color of mainlight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector MainLightColor;
	//overrides color of rim light
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector RimLightColor;
	//overrides color of fill light
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		FVector FillLightColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float LightIntensity;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		USoundCue* VoiceLines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		USoundCue* SFX;
};

USTRUCT(BlueprintType)
struct FCharacterState
{
	GENERATED_BODY()

	TArray<FAnimationFrame> CurrentAnimation;
	FAnimationFrame CurrentAnimFrame;
	TArray<FProjectileState> CurrentProjectileStates;
	TArray<FSigilState> CurrentSigilStates;
	TArray<FBlitzState> CurrentBlitzState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CurrentState")
	uint8 AnimFrameIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CurrentState")
	float PosePlayTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CurrentState")
	uint8 IdleCycle = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CurrentState")
	bool bPlaySound = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Stats")
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 SlowMoTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		uint8 WallBounceTime = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	TArray<int32> SpecialVariables; //Store any unique character variables here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	int32 Seals = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	int32 SealTimer = 0;

	//character's vitality, loses when it hits zero
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		int32 Health = 1000;
	//the resolve that must be broken through
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		uint8 Resolve = 4;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		int32 Durability = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	uint8 ResolveRecoverTimer; //Resolve starts passive recovery after 3 seconds of not being used and while not shattered, does not increment while shattered
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		float ResolvePulse; //dictates how quickly Resolve replenishes based on player actions, recovers more quickly the lower the character's life

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Position; // Y = 0 is considered the ground, X = (-/+)10 are the left and right walls respectively
	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties") //keeps track of non-movement acceleration to apply once hitstop is zero
		FVector2D KnockBack;

	//value that increasingly scales positive vertical knockback the longer a character is in a combo
	//causes a character to not be launched as high the more hits there are in a combo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	int ComboCount = 0; //keeps track of the number of hits in a combo performed by this character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	int ComboTimer = 0; //keeps track of the amount of time this character has spent in hitstun in frames
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bTrueCombo = true; //keeps track of whether or not a character could have escaped a combo at some point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	float SpecialProration = 1;

	// ints to denote active time on directional inputs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir1 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir2 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir3 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir4 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir6 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir7 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir8 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Dir9 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 DoubleDir2 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 DoubleDir6 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 DoubleDir4 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 AirJump = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool Resolute; // Set to true when no inputs are held down

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge2 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge4 = 0;
	//uint8 Charge5 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge6 = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge8 = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge2Life = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge4Life = 0;
	//uint8 Charge5Life = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge6Life = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 Charge8Life = 0;

	// ints to denote active time on button inputs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 LPressed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 MPressed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 HPressed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 BPressed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 LReleased = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 MReleased = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 HReleased = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 BReleased = 0;

	//booleans to track if buttons are being held down
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bIsLDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bIsMDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bIsHDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bIsBDown;

	//int using bit flags to track to actions available to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	int32 AvailableActions;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	int32 MoveList;

	//booleans to dictate the character's current state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bIsRunning = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bForwardJump = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bBackwardJump = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
		bool bSuperJump = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bArmorActive = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bCounterHitState = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	int32 CharacterHitState = None; //determines if character can be bounced against/ stuck to surfaces, uses AttackProperties enum

	//keeps track of whether an attack has already hit something
	//attack effects are only applied based on the first overlap interaction with the attack (!bAttackMadeContact)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bAttackMadeContact = false;
	//keeps track if an attack makes a hit, used for attacks that have a followup when they hit
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bHitSuccess = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bClash = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bBlitzing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bWin = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	bool bLose = false;

	//Sets the corresponding parameters on character's materials
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	FRotator MainLightRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	FRotator FillLightRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	FVector MainLightColor = FVector(1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	FVector FillLightColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	FVector RimLightColor = FVector(1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	float LightIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inputs")
	uint8 StatusTimer = 0; //only mixes with status color as long as this is greater than zero
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

	void HitAnimation();

	virtual void UpdateCharacter(int32 CurrentInputs, int32 FrameNumber);

	void VelocitySolver(); //only called once by gamestate, do not call for each character

	virtual void UpdatePosition();

	void PushboxSolver(); //only called once by gamestate, do not call for each character

	virtual void DrawCharacter();  //set material parameters from child class

	virtual void SetColor(uint8 ColorID);

	bool EnterNewAnimation(TArray<FAnimationFrame> Animation, int32 FrameNumber = 0);

	bool IsCurrentAnimation(TArray<FAnimationFrame> Animation);

	bool RectangleOverlap(FVector2D Pos1, FVector2D Pos2, FVector2D Size1, FVector2D Size2);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		TSubclassOf<class ASigil> SigilBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		TSubclassOf<class ABlitzImageBase> BlitzImageBlueprint;

	ABTCharacterBase* Opponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	FCharacterState CurrentState;

	TArray<ABTProjectileBase*> Projectiles;

	TArray<ASigil*> Sigils;

	ABlitzImageBase* BlitzImage;

	TArray<int32> InputHistory;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		int32 MaxHealth = 1000;

	//values that represent a character's resilience as a battle rages on
	//scales down damage received based on how low the character's health is
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor100;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor50;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor25;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float Valor10;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float StandingPushBoxHeight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float CrouchingPushBoxHeight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float PushboxWidth = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float AirPushboxVerticalOffset = 0;

	FVector StatusColor;
	float StatusMix; //.8f for armor hit (red), 3 for air recover and instant block (white)
	float DepthOffset;
	float LineThickness; //0-1 during cinematics, 3 during normal gameplay or Lerp from (0,1) to 3 based on camera's distance from character

	//Blitz Cancel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BC Anims")
		TArray<FAnimationFrame> FocusBlitz;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BC Anims")
		TArray<FAnimationFrame> BreakerBlitz;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BC Anims")
		TArray<FAnimationFrame> BlitzOutAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BC Anims")
		TArray<FAnimationFrame> BlitzOutStanding;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* Transform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		USkeletalMeshComponent* SmearMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* MainLightRotator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USceneComponent* MainLight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* FillLightRotator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		USceneComponent* FillLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UAudioComponent* CharacterVoice;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UAudioComponent* CharacterSoundEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* Outline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* EyeShine;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
		UMaterialInterface* SmearEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* BodyBC;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* BodySSS;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* BodyILM;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* BodyLines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SmearBody;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Textures")
		UTexture* SmearEmit;

	//Sets the corresponding parameters on character's materials
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		FRotator DefaultMainLightRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		FRotator DefaultFillLightRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		FVector DefaultMainLightColor = FVector(1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		FVector DefaultFillLightColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		FVector DefaultRimLightColor = FVector(1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
		float DefaultLightIntensity = 0;

	UMaterialInstanceDynamic* DynamicOutline;
	UMaterialInstanceDynamic* DynamicEyeShine;
	UMaterialInstanceDynamic* DynamicSmear;

	// Called when the game starts or when spawned
	virtual void BeginPlay();

	virtual void ProcessInputs(int32 Inputs);

	virtual void AnimationStateMachine();

	virtual bool NonKnockdownLanding(); //transitions triggered by touching the ground while not in a hitstun animation

	virtual bool ActiveTransitions(); //Animation transitions triggered by player input

	virtual bool ConditionalTransitions(); //Animation transitions triggered by fulfilling some condition within certain animations

	virtual bool PassiveTransitions(); //Transitions that occur without the influence of outside factors or player input

	virtual bool ExitTimeTransitions(); //Animation transitions triggered by finishing an animation, called from within PassiveTransitions

	virtual void AnimationEvents();

	virtual void CreateMaterials();

	virtual void LightSettings();

	virtual void SpawnPBS(); //spawn in character's projectiles, blitz image, and sigils

	bool BlitzCancel();

	bool QCF();

	bool QCB();

	bool HCF();

	bool HCB();

	bool DP();

	bool RDP();

	bool FC();

	void RefreshMovelist();

	void ContactHit(FHitbox Hitbox, FVector2D HurtboxCenter);

	void ContactThrow(FHitbox Hitbox, int32 ThrowType);

	virtual void ResetSmear();

	bool bShowSmear;

	/* Affects how quickly the character falls to the ground (See below for values per weight class)
		Featherweight = .95 
		Lightweight = .98
		Middleweight = 1
		Heavyweight = 1.03  */
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float Weight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D AirDashForwardOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D AirDashBackOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkBackSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float RunAcceleration = .2f; //set RunAcceleration to zero to enable dash type
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float InitRunSpeed = 2; //set InitRunSpeed to zero to disable run/dash
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float MaxRunSpeed = 5; //Caps Velocity upon hitting an opponent with an attack
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

	//number of frames that an input is active for
		uint8 InputTime = 8;

public:
	//Idle Stance Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> IdleStand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> IdleStandBlink;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> StandIdleAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> IdleCrouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> IdleCrouchBlink;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> CrouchIdleAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> TurnAroundStand;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> TurnAroundCrouch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> StandUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idle Anims")
		TArray<FAnimationFrame> CrouchDown;

	//Locomotion Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> WalkForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> WalkBackward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> PreJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> NeutralJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> ForwardJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> BackwardJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> MidJump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> JumpTransition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> JumpDescent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> RunStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> RunCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> Brake;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> BackDash;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> AirDashForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> AirDashBackward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> AirDashForwardOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> AirDashBackwardOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion Anims")
		TArray<FAnimationFrame> AirRecovery;

	//Guard Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardHiIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardHi;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardHiHeavy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardHiVertical;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardHiOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardLoIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardLo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardLoHeavy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardLoOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard Anims")
		TArray<FAnimationFrame> GuardAirOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throws")
		TArray<FAnimationFrame> ThrowAttempt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throws")
		TArray<FAnimationFrame> AirThrowAttempt;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throws")
		TArray<FAnimationFrame> ResoluteCounter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throws")
		TArray<FAnimationFrame> AirResoluteCounter;

	//Ground Hitstun Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSHIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSHOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSHHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSHHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSLIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSLOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSLHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitSLHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitCIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitCOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitCHeavyIn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> HitCHeavyOut;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> Deflected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> DeflectedAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> ThrowEscape;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> ThrowEscapeAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> Crumple;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Hitstun Anims")
		TArray<FAnimationFrame> Stagger;

	//Airborne hitstun Anims
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> HitstunAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> HitstunAirCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> KnockAway;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> LaunchCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> LaunchTransition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> LaunchFallCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> Tumble;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> Sweep;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> FallingForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> GroundBounce;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> WallBounce;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Air Hitstun Anims")
		TArray<FAnimationFrame> WallStick;

	//Knockdown/WakeUp Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KD Anims")
		TArray<FAnimationFrame> KnockDownFaceDown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KD Anims")
		TArray<FAnimationFrame> KnockDownFaceUp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KD Anims")
		TArray<FAnimationFrame> WakeUpFaceDown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KD Anims")
		TArray<FAnimationFrame> WakeUpFaceUp;

	//Win/Lose Animation Cycles ---> Actual Round Win Animations located on character that transition to the win cycles below to account for unique dialogue that may play based on opponent's character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win/Lose Anims")
		TArray<FAnimationFrame> WinCycle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win/Lose Anims")
		TArray<FAnimationFrame> WinCycle2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win/Lose Anims")
		TArray<FAnimationFrame> TimeOverLose;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win/Lose Anims")
		TArray<FAnimationFrame> LoseCycle;

private:

	//Take in information from CurrentAnimFrame
	void ProcessAnimationFrame();

	bool TurnAroundCheck();

	bool TriggerTurnAround();

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

	void UpdateInputHistory(int32 Inputs, int32 FrameNumber);

	void InputCountdown();

	void AttackCalculation(FHitbox Hitbox, FVector2D HurtboxCenter);

	void SetSounds();

	void SaveFXStates();

	void ProcessBlitz();

	void ClashDetection();

	void HitboxViewer();

	void DrawHitbox(FHitbox Box);

	void DrawHurtbox(FHurtbox Box);
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
