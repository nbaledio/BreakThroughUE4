// Copyright ShatterPoint Games. All Rights Reserved.

#include "CharacterBase.h"

//Define Collision Channels
#define ECC_HitBox			ECC_GameTraceChannel1
#define ECC_HurtBox			ECC_GameTraceChannel2
#define ECC_PushBox			ECC_GameTraceChannel3
#define ECC_GuardBox		ECC_GameTraceChannel4
#define ECC_Wall			ECC_GameTraceChannel5
#define ECC_Floor			ECC_GameTraceChannel6
#define ECC_PushTrigger		ECC_GameTraceChannel7


// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PushBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Push Box"));
	RootComponent = PushBox;

	PushBoxTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Push Trigger"));
	PushBoxTrigger->SetupAttachment(RootComponent);
	PushBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACharacterBase::SurfaceOverlapEnter);

	PersonalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Personal Camera"));
	PersonalCamera->SetupAttachment(RootComponent);

	PushBoxSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Push Box Sprite"));
	PushBoxSprite->SetupAttachment(PushBoxTrigger);

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(PushBoxTrigger);

	HitBoxFolder = CreateDefaultSubobject<USceneComponent>(TEXT("Hitboxes"));
	HitBoxFolder->SetupAttachment(PushBoxTrigger);

	HitBox0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 0"));
	HitBox0->SetupAttachment(HitBoxFolder);
	HitBox1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 1"));
	HitBox1->SetupAttachment(HitBoxFolder);
	HitBox2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 2"));
	HitBox2->SetupAttachment(HitBoxFolder);
	HitBox3 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 3"));
	HitBox3->SetupAttachment(HitBoxFolder);
	HitBox4 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 4"));
	HitBox4->SetupAttachment(HitBoxFolder);
	HitBox5 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 5"));
	HitBox5->SetupAttachment(HitBoxFolder);
	HitBox6 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Hitbox 6"));
	HitBox6->SetupAttachment(HitBoxFolder);

	HurtBoxFolder = CreateDefaultSubobject<USceneComponent>(TEXT("Hurtboxes"));
	HurtBoxFolder->SetupAttachment(PushBoxTrigger);

	UpperBody0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Upper Body 0"));
	UpperBody0->SetupAttachment(HurtBoxFolder);
	UpperBody1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Upper Body 1"));
	UpperBody1->SetupAttachment(HurtBoxFolder);
	UpperBody2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Upper Body 2"));
	UpperBody2->SetupAttachment(HurtBoxFolder);

	LowerBody0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Lower Body 0"));
	LowerBody0->SetupAttachment(HurtBoxFolder);
	LowerBody1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Lower Body 1"));
	LowerBody1->SetupAttachment(HurtBoxFolder);
	LowerBody2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Lower Body 2"));
	LowerBody2->SetupAttachment(HurtBoxFolder);

	GuardBox = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Guard Box"));
	GuardBox->SetupAttachment(PushBoxTrigger);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	Velocity = FVector(0, 0, 0);
	PushBox->OnComponentHit.AddDynamic(this, &ACharacterBase::OnSurfaceHit);
	ClearHitBox();
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnAroundCheck();

	/*if (Dir2 > 0 && Dir3 > 0 && Dir6 > 0 && Dir6 > Dir3 && Dir3 > Dir2) //Testing for specific directional inputs
	{
		Dir2 = 0;
		Dir3 = 0;
		Dir6 = 0;
		UE_LOG(LogTemp, Warning, TEXT("QCF inputted"));
	}
	if (Dir2 > 0 && Dir3 > 0 && Dir6 > 0 && Dir3 > Dir2 && Dir2 > Dir6)
	{
		Dir2 = 0;
		Dir3 = 0;
		Dir6 = 0;
		UE_LOG(LogTemp, Warning, TEXT("DP inputted"));
	}*/
	if (bAcceptMove && !bIsAirborne)
	{
		if (Dir6 >= InputTime - 1)
		{
			if (bFacingRight)
				Velocity.X = WalkSpeed;
			else
				Velocity.X = -WalkSpeed;
			//play walkforward anim
		}
		else if (Dir4 >= InputTime - 1)
		{
			if (bFacingRight)
				Velocity.X = -WalkBackSpeed;
			else
				Velocity.X = WalkBackSpeed;
			//play walkback anim
		}
		else
		{
			Velocity.X = 0;
		}
	}

	if (HitStop == 0)
	{
		if (bIsAirborne && BlitzDashTime == 0) //apply gravity while character is airborne and not defying gravity
		{
			float GravCalc = (Weight * GravityScale * -10.f);
			if (SlowMoTime > 0)
				GravCalc /= 2;

			Velocity.Z += GravCalc;
		}

		if (SlowMoTime > 0)
			AddActorLocalOffset(Velocity / 120.f, true);
		else
			AddActorLocalOffset(Velocity / 60.f, true);
	}

	InputCountdown();
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ACharacterBase::HorizontalInput);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ACharacterBase::VerticalInput);
	PlayerInputComponent->BindAction(TEXT("Square"), IE_Pressed, this, &ACharacterBase::LightPress);
	PlayerInputComponent->BindAction(TEXT("Square"), IE_Released, this, &ACharacterBase::LightRelease);
	PlayerInputComponent->BindAction(TEXT("Triangle"), IE_Pressed, this, &ACharacterBase::MediumPress);
	PlayerInputComponent->BindAction(TEXT("Triangle"), IE_Released, this, &ACharacterBase::MediumRelease);
	PlayerInputComponent->BindAction(TEXT("Circle"), IE_Pressed, this, &ACharacterBase::HeavyPress);
	PlayerInputComponent->BindAction(TEXT("Circle"), IE_Released, this, &ACharacterBase::HeavyRelease);
	PlayerInputComponent->BindAction(TEXT("Cross"), IE_Pressed, this, &ACharacterBase::BreakPress);
	PlayerInputComponent->BindAction(TEXT("Cross"), IE_Released, this, &ACharacterBase::BreakRelease);
	PlayerInputComponent->BindAction(TEXT("R1"), IE_Pressed, this, &ACharacterBase::LMPress);
	PlayerInputComponent->BindAction(TEXT("R2"), IE_Pressed, this, &ACharacterBase::HBPress);
	PlayerInputComponent->BindAction(TEXT("L1"), IE_Pressed, this, &ACharacterBase::LBPress);
	PlayerInputComponent->BindAction(TEXT("L2"), IE_Pressed, this, &ACharacterBase::MHPress);

}

/*Inputs are recorded using keypad notation
Below is a diagram for which floats are fired based on directional input 
and while the character is facing toward the right. The left and right columns
are switched if the character is facing toward the left.

"5" represents the neutral position and thus does not have a corresponding variable

     ^
     |
   7 8 9
<- 4 5 6 ->
   1 2 3
     |
     v
*/
void ACharacterBase::HorizontalInput(float AxisValue)
{
	if (AxisValue > .25f)
	{
		if (bFacingRight)
		{
			MoveForward();
		}
		else
		{
			MoveBackward();
		}
	}
	else if (AxisValue < -.25f)
	{
		if (!bFacingRight)
		{
			MoveForward();
		}
		else
		{
			MoveBackward();
		}
	}
}

void ACharacterBase::VerticalInput(float AxisValue)
{
	if (InputComponent->GetAxisValue(TEXT("Horizontal")) < .25f && InputComponent->GetAxisValue(TEXT("Horizontal")) > - .25f )
	{
		if (AxisValue > .25f)
		{
			Dir8 = InputTime;
		}
		else if (AxisValue < -.25f)
		{
			if (Dir2 < InputTime - 1 && Dir2 > 0 && DoubleDir2 == 0)
			{
				DoubleDir2 = InputTime;
			}

			Dir2 = InputTime;
		}
	}

}

void ACharacterBase::MoveForward()
{
	if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
	{
		Dir9 = InputTime; //Up and forward
	}
	else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
	{
		Dir3 = InputTime; // Down and Forward
	}
	else
	{
		if (Dir6 < InputTime - 1 && Dir6 > 0 && DoubleDir6 == 0)
		{
			DoubleDir6 = InputTime;
		}
			
		Dir6 = InputTime; //Pure forward
	}
}

void ACharacterBase::MoveBackward()
{
	if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
	{
		Dir7 = InputTime; //Up and Backward
	}
	else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
	{
		Dir1 = InputTime; //Down and Backward
	}
	else
	{
		if (Dir4 < InputTime - 1 && Dir4 > 0 && DoubleDir4 == 0)
		{
			DoubleDir4 = InputTime;
		}		
		Dir4 = InputTime; //Pure backward
	}
}

void ACharacterBase::InputCountdown()
{
	if (Dir1 > 0)
		Dir1--;
	if (Dir2 > 0)
		Dir2--;
	if (Dir3 > 0)
		Dir3--;
	if (Dir4 > 0)
		Dir4--;
	if (Dir6 > 0)
		Dir6--;
	if (Dir7 > 0)
		Dir7--;
	if (Dir8 > 0)
		Dir8--;
	if (Dir9 > 0)
		Dir9--;
	if (DoubleDir2 > 0)
		DoubleDir2--;
	if (DoubleDir6 > 0)
		DoubleDir6--;
	if (DoubleDir4 > 0)
		DoubleDir4--;
	if (LPressed > 0)
		LPressed--;
	if (MPressed > 0)
		MPressed--;
	if (HPressed > 0)
		HPressed--;
	if (BPressed > 0)
		BPressed--;
	if (LReleased > 0)
		LReleased--;
	if (MReleased > 0)
		MReleased--;
	if (HReleased > 0)
		HReleased--;
	if (BReleased > 0)
		BReleased--;
}

void ACharacterBase::LightPress()
{
	if (!bIsLDown)
	{
		LPressed = InputTime;
		bIsLDown = true;
	}
}

void ACharacterBase::LightRelease()
{
	if (bIsLDown)
	{
		LReleased = InputTime;
		bIsLDown = false;
	}
}

void ACharacterBase::MediumPress()
{
	if (!bIsMDown)
	{
		MPressed = InputTime;
		bIsMDown = true;
	}
}

void ACharacterBase::MediumRelease()
{
	if (bIsMDown)
	{
		MReleased = InputTime;
		bIsMDown = false;
	}
}

void ACharacterBase::HeavyPress()
{
	if (!bIsHDown)
	{
		HPressed = InputTime;
		bIsHDown = true;
	}
}

void ACharacterBase::HeavyRelease()
{
	if (bIsHDown)
	{
		HReleased = InputTime;
		bIsHDown = false;
	}
}

void ACharacterBase::BreakPress()
{
	if (!bIsBDown)
	{
		BPressed = InputTime;
		bIsBDown = true;
	}
}

void ACharacterBase::BreakRelease()
{
	if (bIsBDown)
	{
		BReleased = InputTime;
		bIsBDown = false;
	}
}

void ACharacterBase::LMPress()
{
	if (!bIsLDown && !bIsMDown)
	{
		LPressed = InputTime;
		MPressed = InputTime;
	}
}

void ACharacterBase::HBPress()
{
	if (!bIsHDown && !bIsBDown)
	{
		HPressed = InputTime;
		BPressed = InputTime;
	}
}

void ACharacterBase::MHPress()
{
	if (!bIsMDown && !bIsHDown)
	{
		MPressed = InputTime;
		HPressed = InputTime;
	}
}

void ACharacterBase::LBPress()
{
	if (!bIsLDown && !bIsBDown)
	{
		LPressed = InputTime;
		BPressed = InputTime;
	}
}

void ACharacterBase::TurnAroundCheck()
{
	if (!bFacingRight && PushBoxTrigger->GetRelativeScale3D().X != -1)
	{
		PushBoxTrigger->SetRelativeScale3D(FVector(-1, 1, 1));
		//Trigger turnaround animation if in idle stand or crouch;
	}
	else if (bFacingRight && PushBoxTrigger->GetRelativeScale3D().X != 1)
	{
		PushBoxTrigger->SetRelativeScale3D(FVector(1, 1, 1));
		//Trigger turnaround animation if in idle stand or crouch;
	}
}

void ACharacterBase::SurfaceOverlapEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if PushBoxTrigger has overlapped with the floor or wall and apply appropriate behavior
	//check if overlapping with other Character's PushBoxTrigger and apply appropriate action based on state (character push, teleporting to prevent characters from occupying same space, etc.)
	//character pushing idea: (if both grounded, opponent velocity.x is zero, and self velocity.x is forward, make opponent's velocity half of self velocity)
		//UE_LOG(LogTemp, Warning, TEXT("OtherComponent: %s"), *OtherComp->GetName());
		
}

void ACharacterBase::SurfaceOverlapExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if PushBoxTrigger has overlapped with the floor or wall and apply appropriate behavior
	//check if overlapping with other Character's PushBoxTrigger and apply appropriate action based on state (character push, teleporting to prevent characters from occupying same space, etc.)
	//character pushing idea: (if both grounded, opponent velocity.x is zero, and self velocity.x is forward, make opponent's velocity half of self velocity)
	UE_LOG(LogTemp, Warning, TEXT("OtherComponent: %s"), *OtherComp->GetName());
	if (OtherComp->GetCollisionObjectType() == ECC_Floor)
	{
		bIsAirborne = true;
		UE_LOG(LogTemp, Warning, TEXT("Character has left floor."));
	}
	else if (OtherComp->GetCollisionObjectType() == ECC_Wall)
	{
		bTouchingWall = false;
	}
}

void ACharacterBase::ClearHitBox()
{
	HitBox0->SetVisibility(false);
	HitBox1->SetVisibility(false);
	HitBox2->SetVisibility(false);
	HitBox3->SetVisibility(false);
	HitBox4->SetVisibility(false);
	HitBox5->SetVisibility(false);
	HitBox6->SetVisibility(false);

	HitBox0->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox5->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox6->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttackMadeContact = false;

	bJumpCancellable = false;
	bLightCancellable = false;
	bMediumCancellable = false;
	bHeavyCancellable = false;
	bBreakCancellable = false;
	bCommandNormalCancellable = false;
	bSpecialCancellable = false;
	bSuperCancellable = false;

	BaseDamage = 0;
	DurabilityDamage = 0;
	ResolveDamage = 0;
	AttackLevel = 0;
	AttackHeight = Mid;
	BaseHitStun = 0;
	BaseHitStop = 0;
	InitProration = 1.f;
	ForcedProration = 1.f;

	PotentialKnockBack = FVector(0,0,0);
	PotentialAirKnockBack = FVector(0,0,0);

	bCanGroundBounce = false;
	bCanWallBounce = false;
	bCanWallStick = false;
	bCanShatter = false;
	AdditionalEffect = None;
}

void ACharacterBase::ClearHurtBox()
{
	UpperBody0->SetVisibility(false);
	UpperBody1->SetVisibility(false);
	UpperBody2->SetVisibility(false);
	LowerBody0->SetVisibility(false);
	LowerBody1->SetVisibility(false);
	LowerBody2->SetVisibility(false);

	UpperBody0->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UpperBody1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UpperBody2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LowerBody0->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LowerBody1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LowerBody2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACharacterBase::DisableAllActions(bool bDisableBlitz)
{
	bAcceptMove = false;
	bAcceptJump = false;
	bAcceptGuard = false;
	bAcceptLight = false;
	bAcceptMedium = false;
	bAcceptHeavy = false;
	bAcceptBreak = false;
	bAcceptCommandNormal = false;
	bAcceptSpecial = false;
	bAcceptSuper = false;
	bAcceptBlitz = !bDisableBlitz;

	bIsRunning = false;
	bArmorActive = false;
	bCounterHitState = false;
}

void ACharacterBase::EnableAllActions()
{
	bAcceptMove = true;
	bAcceptJump = true;
	bAcceptGuard = true;
	bAcceptLight = true;
	bAcceptMedium = true;
	bAcceptHeavy = true;
	bAcceptBreak = true;
	bAcceptCommandNormal = true;
	bAcceptSpecial = true;
	bAcceptSuper = true;
	bAcceptBlitz = true;

	bArmorActive = false;
	bCounterHitState = false;
	bGroundBounceState = false;
	bWallBounceState = false;
	bWallStickState = false;
}

void ACharacterBase::ActivateCollisionBox(OUT UPaperSpriteComponent* Collider)
{
	//Can be used to set hitbox and hurtbox to active
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Also add whether to display perhaps based on an option in GameMode? 
	//if (/*add condition*/) {Collider->SetVisibility(true)}
}

void ACharacterBase::OnSurfaceHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//character-surface interactions (i.e. wall bounce, wall stick, ground bounce, knockdown, landing)
	if (OtherComp->GetCollisionObjectType() == ECC_Floor)
	{
		bIsAirborne = false;
		Velocity.Z = 0;
		UE_LOG(LogTemp, Warning, TEXT("PushBox has hit floor."));
	}
	else if (OtherComp->GetCollisionObjectType() == ECC_Wall)
	{
		bTouchingWall = true;
		Velocity.X = 0;
	}
}

/*void ACharacterBase::CheckOpponentFacing()
{
	called if bAcceptMove or from specific anim notifies
	if (opponent location.x > this location.x)
		bFacingRight = true;
	else if (opponent location < this location.x)
		bFacingRight = false;
}*/
