// Copyright ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/Vector.h"
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

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	float Weight = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	float WalkSpeed = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	float WalkBackSpeed = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	float RunSpeed = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	float BlitzDashForce = 2;

	//x dictates horizontal acceleration, z dictates vertical acceleration
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector BackDashForce;

	//x dictates horizontal acceleration during forward/backward jumps, z dictates vertical acceleration
	UPROPERTY(EditDefaultsOnly, Category = "Movement Properties")
	FVector JumpForce;

	UPROPERTY(EditAnywhere)
	bool bFacingRight = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	bool bAcceptMove;
	bool bAcceptGuard;
	bool bAcceptLight;
	bool bAcceptMedium;
	bool bAcceptHeavy;
	bool bAcceptBreak;
	bool bAcceptCommandNormal;
	bool bAcceptSpecial;
	bool bAcceptSuper;
	bool bAcceptBlitz;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* PushBoxTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* PushBoxSprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BaseMesh;

	void HorizontalInput(float AxisValue);
	void VerticalInput(float AxisValue);

};
