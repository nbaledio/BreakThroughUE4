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
	Low,
	High,
	Overhead,
	Unblockable,
	Throw,
	CommandThrow,
	AirThrow,
	AirCommandThrow
};

enum SpecialStates
{
	ProjectileInvincible = 1,
	StrikeInvincible,
	ThrowInvincible,
	FullInvincible,
	HiCounter,
	LowCounter,
	AllCounter
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
};

enum AttackProperties
{
	None = 0,
	GroundBounce = (1 << 0),
	WallBounce = (1 << 1),
	WallStick = (1 << 2),
	Sweep = (1 << 3),
	Launch = (1 << 4),
	Stagger = (1 << 5),
	Crumple = (1 << 6),
	KnockAway = (1 << 7),
	Deflected = (1 << 8),
	Tumbling = (1 << 9),
	IsSpecial = (1 << 10),
	IsSuper = (1 << 11),
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
		int32 AttackProperties; // denotes the properties an attack has on hit using bit flags

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 DurabilityDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 ResolveDamage = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AttackHeight = Mid;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseHitStun = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseHitStop = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 BaseBlockStun = 0;
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
		int32 PlayDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hitboxes")
		TArray<FHurtbox> Hitboxes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hurtboxes")
		TArray<FHurtbox> HurtboxUpperBody;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hurtboxes")
		TArray<FHurtbox> HurtboxLowerBody;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 AvailableActions; //uses CharacterActions enum, determines what actions are available to the player upon entering this animation frame
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		int32 Invincibility; //uses SpecialStates enum
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bStopVelocityX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
		bool bStopVelocityY;
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

	virtual void UpdateCharacter();

	virtual void HitDetection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();

	//character's vitality, loses when it hits zero
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		int32 Health;
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		int32 MaxHealth = 1000;

	//the resolve that must be broken through
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		int32 Resolve = 4;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Battle Stats")
		int32 Durability = 100;

	//values that represent a character's resilience as a battle rages on
	//scales down damage received based on how low the character's health is
	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
		float CurrentValor;
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
		int32 HitStun = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 BlockStun = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 LandingLag = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 HitStop = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 BlitzDashTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		int32 ShatteredTime = 0;
	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		int32 SlowMoTime = 0;

	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
		int32 JumpsUsed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		float AnimSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsAirborne = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
		bool bIsGuarding = true;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float Weight = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float WalkBackSpeed = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float RunAcceleration = .2f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float MaxRunSpeed = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		float BlitzDashForce = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		int32 MaxJumps = 2;
	//x dictates horizontal acceleration, z dictates vertical acceleration. y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D BackDashForce;
	//x dictates horizontal acceleration during forward/backward jumps, z dictates vertical acceleration, y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
		FVector2D JumpForce;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Position;
	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
		FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties") //keeps track of non-movement acceleration to apply once hitstop is zero
		FVector2D KnockBack;

	//the influence of gravity on a character
	//if being hit, increases with the number of hits in a combo to make continuing a combo more difficult as time goes on
		float GravityScale = 1;

	//number of frames that an input is active for
		int32 InputTime = 12;
	// ints to denote active time on directional inputs
		int32 Dir1 = 0;
		int32 Dir2 = 0;
		int32 Dir3 = 0;
		int32 Dir4 = 0;
		int32 Dir6 = 0;
		int32 Dir7 = 0;
		int32 Dir8 = 0;
		int32 Dir9 = 0;
		int32 DoubleDir2 = 0;
		int32 DoubleDir6 = 0;
		int32 DoubleDir4 = 0;
		bool bAirJump = false;


	// ints to denote active time on button inputs
		int32 LPressed = 0;
		int32 MPressed = 0;
		int32 HPressed = 0;
		int32 BPressed = 0;
		int32 LReleased = 0;
		int32 MReleased = 0;
		int32 HReleased = 0;
		int32 BReleased = 0;
	
	//booleans to track if buttons are being held down
		bool bIsLDown;
		bool bIsMDown;
		bool bIsHDown;
		bool bIsBDown;

	//int using bit flags to track to actions available to the character
		int32 AvailableActions;

	//booleans to dictate the character's current state
		bool bIsRunning = false;
		bool bArmorActive = false;
		bool bCounterHitState = false;
		bool bTouchingWall = false;
		int32 CharacterHitState = None; //used if an attack puts character in a specific animation state (crumple, sweep, launch, etc.), or if they can be bounced against/ stuck to surfaces
		int32 Invincibility = None; // determines if a character is invincible, projectile immune, deflects attacks of certain heights, etc

	//keeps track of whether an attack has already hit something
	//attack effects are only applied based on the first overlap interaction with the attack (!bAttackMadeContact)
		bool bAttackMadeContact = false;
	//keeps track if an attack makes a hit, used for attacks that have a followup when they hit
		bool bHitSuccess = false;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* BaseMesh;

};

/* 
 TArray<AnimationPose> StandingAnim
*/