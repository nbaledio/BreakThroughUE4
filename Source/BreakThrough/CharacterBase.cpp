// Copyright ShatterPoint Games. All Rights Reserved.


#include "CharacterBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PushBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Push Box"));
	RootComponent = PushBox;

	PushBoxTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Push Trigger"));
	PushBoxTrigger->SetupAttachment(RootComponent);
	PushBox->OnComponentBeginOverlap.AddDynamic(this, &ACharacterBase::SurfaceOverlapCheck);

	PersonalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Personal Camera"));
	PersonalCamera->SetupAttachment(RootComponent);

	PushBoxSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Push Box Sprite"));
	PushBoxSprite->SetupAttachment(PushBoxTrigger);

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(PushBoxTrigger);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnAroundCheck();
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
		Dir4 = InputTime; //Pure backward
	}
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

void ACharacterBase::SurfaceOverlapCheck(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if PushBoxTrigger has overlapped with the floor or wall and apply appropriate behavior
	//check if overlapping with other Character's PushBoxTrigger and apply appropriate action based on state (character push, teleporting to prevent characters from occupying same space, etc.)
	//character pushing idea: (if both grounded, opponent velocity.x is zero, and self velocity.x is forward, make opponent's velocity half of self velocity)
		UE_LOG(LogTemp, Warning, TEXT("OtherComponent: %s"), *OtherComp->GetName());
}

/*void ACharacterBase::CheckOpponentFacing()
{
	called if bAcceptMove or from specific anim notifies
	if (opponent location.x > this location.x)
		bFacingRight = true;
	else if (opponent location < this location.x)
		bFacingRight = false;
}*/
