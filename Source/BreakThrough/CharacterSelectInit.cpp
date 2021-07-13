// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "CharacterSelectInit.h"

// Sets default values
ACharacterSelectInit::ACharacterSelectInit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UCharacterSelectForeground> CharacterSelectForegroundWidget(TEXT("/Game/CharacterSelect/Blueprints/CharacterSelectForeground"));
	CharacterSelectForegroundWidgetClass = CharacterSelectForegroundWidget.Class;
}

// Called when the game starts or when spawned
void ACharacterSelectInit::BeginPlay()
{
	Super::BeginPlay();
	CharacterSelectForeground = CreateWidget<UCharacterSelectForeground>(GetWorld()->GetGameInstance(), CharacterSelectForegroundWidgetClass);
	CharacterSelectForeground->AddToViewport(0);
}

// Called every frame
void ACharacterSelectInit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

