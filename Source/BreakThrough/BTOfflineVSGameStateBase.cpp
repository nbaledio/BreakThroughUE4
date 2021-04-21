// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTOfflineVSGameStateBase.h"

void ABTOfflineVSGameStateBase::BeginPlay()
{
    Super::BeginPlay();
    for (uint8 i = 0; i < 2; i++)
    {
        UGameplayStatics::CreatePlayer(GetWorld(), i, true);
    }
    
    SpawnCharacters();

    gs.Init();
}

void ABTOfflineVSGameStateBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ElapsedTime += DeltaSeconds;

    int32 Inputs[2] = { 0, 0 };
    for (uint8 i = 0; i < 2; i++)
    {
        Inputs[i] = GetLocalInputs(i);
    }
    //AdvanceFrame(Inputs);
    while (ElapsedTime >= ONE_FRAME) {
        AdvanceFrame(Inputs);

        ElapsedTime -= ONE_FRAME;
    }
    //DrawFrame();
}