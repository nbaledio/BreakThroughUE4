// Copyright ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/Vector2D.h"
#include "CharacterBase.generated.h"

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

	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
	int32 Health;

	UPROPERTY(EditDefaultsOnly, Category = "Battle Stats")
	int32 MaxHealth = 1000;

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
	float AnimSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stats")
	bool bIsAirborne;

	UPROPERTY(VisibleAnywhere, Category = "Battle Stats")
	bool bIsBlitzed;

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
	//x dictates horizontal acceleration, y dictates vertical acceleration
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector2D BackDashForce;
	//x dictates horizontal acceleration during forward/backward jumps, y dictates vertical acceleration
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector2D JumpForce;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
	FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, Category = "Movement Properties")
	FVector2D KnockBack;



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

	bool bIsRunning = true;
	bool bArmorActive = false;
	bool bCounterHitState = false;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* PushBoxSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BaseMesh;

	//keeps track of whether an attack has already hit something
	//attack effects are only applied based on the first overlap interaction with the attack (!bAttackMadeContact)
	bool bAttackMadeContact = false;
	//int32 Contacts = 0; //possibly unnecessary

	void HorizontalInput(float AxisValue);
	void VerticalInput(float AxisValue);
	void MoveForward();
	void MoveBackward();

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

	UFUNCTION()
	void SurfaceOverlapCheck(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
