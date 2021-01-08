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

	PushBoxSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Push Box Sprite"));
	PushBoxSprite->SetupAttachment(RootComponent);

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	BaseMesh->SetupAttachment(RootComponent);
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

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ACharacterBase::HorizontalInput);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ACharacterBase::VerticalInput);

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
			if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
			{
				Dir9 = InputTime;//Up and forward
				UE_LOG(LogTemp, Warning, TEXT("Dir9 set."));
			}		
			else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
			{
				Dir3 = InputTime; // Down and Forward
				UE_LOG(LogTemp, Warning, TEXT("Dir3 set."));
			}
			else
			{
				Dir6 = InputTime; //Pure forward
				UE_LOG(LogTemp, Warning, TEXT("Dir6 set."));
			}
		}
		else
		{
			if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
			{
				Dir7 = InputTime; //Up and Backward
				UE_LOG(LogTemp, Warning, TEXT("Dir7 set."));
			}
			else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
			{
				Dir1 = InputTime; //Down and Backward
				UE_LOG(LogTemp, Warning, TEXT("Dir1 set."));
			}
			else
			{
				Dir4 = InputTime; //Pure backward
				UE_LOG(LogTemp, Warning, TEXT("Dir4 set."));
			}
		}
	}
	else if (AxisValue < -.25f)
	{
		if (!bFacingRight)
		{
			if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
			{
				Dir9 = InputTime;//Up and forward
				UE_LOG(LogTemp, Warning, TEXT("Dir9 set."));
			}
			else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
			{
				Dir3 = InputTime; // Down and Forward
				UE_LOG(LogTemp, Warning, TEXT("Dir3 set."));
			}
			else
			{
				Dir6 = InputTime; //Pure forward
				UE_LOG(LogTemp, Warning, TEXT("Dir6 set."));
			}
		}
		else
		{
			if (InputComponent->GetAxisValue(TEXT("Vertical")) > .25f)
			{
				Dir7 = InputTime; //Up and Backward
				UE_LOG(LogTemp, Warning, TEXT("Dir7 set."));
			}
			else if (InputComponent->GetAxisValue(TEXT("Vertical")) < -.25f)
			{
				Dir1 = InputTime; //Down and Backward
				UE_LOG(LogTemp, Warning, TEXT("Dir1 set."));
			}
			else
			{
				Dir4 = InputTime; //Pure backward
				UE_LOG(LogTemp, Warning, TEXT("Dir4 set."));
			}
		}
	}
}

void ACharacterBase::VerticalInput(float AxisValue)
{
	if (InputComponent->GetAxisValue(TEXT("Vertical")) < .25f && InputComponent->GetAxisValue(TEXT("Vertical")) > - .25f )
	{
		if (AxisValue > .25f)
		{
			Dir8 = InputTime;
			UE_LOG(LogTemp, Warning, TEXT("Dir8 set."));
		}
		else if (AxisValue < -.25f)
		{
			Dir2 = InputTime;
			UE_LOG(LogTemp, Warning, TEXT("Dir2 set."));
		}
	}

}

