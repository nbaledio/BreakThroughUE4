// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectForeground.h"
#include "CharacterSelectInit.generated.h"

UCLASS()
class BREAKTHROUGH_API ACharacterSelectInit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterSelectInit();
	//Widgets
	UCharacterSelectForeground* CharacterSelectForeground;

	TSubclassOf<class UUserWidget> CharacterSelectForegroundWidgetClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
