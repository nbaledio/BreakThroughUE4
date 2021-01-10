// Copyright ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/Vector.h"
#include "CharacterBase.generated.h"

#define OUT

enum GuardLevel
{
	Mid,
	Low,
	High,
	Overhead,
	Unblockable,
	Throw,
	CommandThrow
};

enum HitEffects
{
	None,
	Sweep,
	Launch,
	Stagger,
	Crumple,
	Deflected
};

UCLASS()
class BREAKTHROUGH_API ACharacterBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACharacterBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* PersonalCamera;

	//Hitboxes for the character to attack with
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox4;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* HitBox6;

	//Hurtboxes to represent the character's physical position, where they can be hit
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* UpperBody0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* UpperBody1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* UpperBody2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* LowerBody0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* LowerBody1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* LowerBody2;

	//Guardbox represents proximity to an attack, character automatically puts up guard when in this box and is holding a backward direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPaperSpriteComponent* GuardBox;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	float AnimSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bIsAirborne = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bIsCrouching = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bIsGuarding = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bAutoGuard = true;

	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
	int32 SlowMoTime = 0;

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
	//x dictates horizontal acceleration, z dictates vertical acceleration. y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector BackDashForce;
	//x dictates horizontal acceleration during forward/backward jumps, z dictates vertical acceleration, y should never be changed or touched
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector JumpForce;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties") //keeps track of non-movement acceleration to apply once hitstop is zero
	FVector KnockBack;

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

	//booleans to track to actions available to the character
	bool bAcceptMove = true;
	bool bAcceptJump = true;
	bool bAcceptGuard = true;
	bool bAcceptLight = true;
	bool bAcceptMedium = true;
	bool bAcceptHeavy = true;
	bool bAcceptBreak = true;
	bool bAcceptCommandNormal = true;
	bool bAcceptSpecial = true;
	bool bAcceptSuper = true;
	bool bAcceptBlitz = true;

	//booleans to dictate the character's current state
	bool bIsRunning = false;
	bool bArmorActive = false;
	bool bCounterHitState = false;
	bool bTouchingWall = false;
	bool bGroundBounceState = false;
	bool bWallBounceState = false;
	bool bWallStickState = false;
	int32 AdditionalHitState = 0;

	//attack properties
	bool bJumpCancellable;
	bool bLightCancellable;
	bool bMediumCancellable;
	bool bHeavyCancellable;
	bool bBreakCancellable;
	bool bCommandNormalCancellable;
	bool bSpecialCancellable;
	bool bSuperCancellable;

	int32 BaseDamage = 0;
	int32 DurabilityDamage = 0;
	int32 ResolveDamage = 0;
	int32 AttackLevel = 0;
	int32 AttackHeight = Mid;
	int32 BaseHitStun = 0;
	int32 BaseHitStop = 0;
	float InitProration = 1.f;
	float ForcedProration = 1.f;

	FVector PotentialKnockBack;
	FVector PotentialAirKnockBack;

	bool bCanGroundBounce = false;
	bool bCanWallBounce = false;
	bool bCanWallStick = false;
	bool bCanShatter = false;
	int32 AdditionalEffect = 0;
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* PushBoxSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HitBoxFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HurtBoxFolder;

	//keeps track of whether an attack has already hit something
	//attack effects are only applied based on the first overlap interaction with the attack (!bAttackMadeContact)
	bool bAttackMadeContact = false;
	//int32 Contacts = 0; //possibly unnecessary

	void HorizontalInput(float AxisValue);
	void VerticalInput(float AxisValue);
	void MoveForward();
	void MoveBackward();
	void InputCountdown();

	void LightPress();
	void MediumPress();
	void HeavyPress();
	void BreakPress();
	void LightRelease();
	void MediumRelease();
	void HeavyRelease();
	void BreakRelease();
	void LMPress();
	void HBPress();
	void MHPress();
	void LBPress();

	void TurnAroundCheck();

	void ActivateCollisionBox(OUT UPaperSpriteComponent* Collider);
	void ClearHitBox();
	void ClearHurtBox();
	void DisableAllActions(bool bDisableBlitz);
	void EnableAllActions();

	UFUNCTION()
	void SurfaceOverlapEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SurfaceOverlapExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSurfaceHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//void CheckOpponentFacing();

};
