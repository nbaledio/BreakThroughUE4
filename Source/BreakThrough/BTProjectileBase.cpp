// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTProjectileBase.h"

// Sets default values
ABTProjectileBase::ABTProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABTProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABTProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

