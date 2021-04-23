// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "BTGameState.h"

void BTGameState::Init()
{
    if (Player[0]) //set characters as each others' opponent
    {
        if (Player[1])
        {
            Player[0]->Opponent = Player[1];
            Player[1]->Opponent = Player[0];
            CharacterStates[0] = Player[0]->CurrentState;
            CharacterStates[1] = Player[1]->CurrentState;
        }
    }
    FrameNumber = 0;
    SaveGameState();//copy the initial character states to this structure
}

void BTGameState::Update(int inputs[], int disconnect_flags)
{
    FrameNumber++;
    LoadGameState();
    if (Player[0])
    {
            Player[0]->SuperFlashSolver();

            for (int32 i = 0; i < 2; i++)
            {
                if (Player[i])
                        Player[i]->HitDetection();
            }

            for (int32 i = 0; i < 2; i++)
            {
                if (Player[i])
                    Player[i]->HitAnimation();
            }

            for (int32 i = 0; i < 2; i++)
            {
                if (disconnect_flags & (1 << i) && Player[i])
                {
                    if (Player[i]->InputHistory.Num() > FrameNumber - 2)
                            Player[i]->UpdateCharacter(Player[i]->InputHistory[FrameNumber - 2], FrameNumber);
                }
                else if (Player[i])
                        Player[i]->UpdateCharacter(inputs[i], FrameNumber);
            }

            Player[0]->VelocitySolver();

            for (int32 i = 0; i < 2; i++)
            {
                if (Player[i])
                        Player[i]->UpdatePosition();
            }

            Player[0]->PushboxSolver();
    }

    //update roundmanager 
    SaveGameState();
}

void BTGameState::LoadGameState()
{
    for (int32 i = 0; i < 2; i++)
    {
        if (Player[i] != nullptr)// && CharacterStates.Num() > i)
        {
            Player[i]->CurrentState = CharacterStates[i];
            UE_LOG(LogTemp, Warning, TEXT("OnCharacter: Player %d Position: (%f, %f)"), i, Player[i]->CurrentState.Position.X, Player[i]->CurrentState.Position.Y);
            UE_LOG(LogTemp, Warning, TEXT("SavedBTGameState: Player %d Position: (%f, %f)"), i, Player[i]->CurrentState.Position.X, Player[i]->CurrentState.Position.Y);
        }
    }

    //load roundmanager state
}

void BTGameState::SaveGameState()
{
    for (int32 i = 0; i < 2; i++)
    {
        if (Player[i] != nullptr)
        {
            CharacterStates[i] = Player[i]->CurrentState;
        }
    }

    //save roundmanager state
}

void BTGameState::DrawFrame() //call at the end of BTGameState Tick
{
    for (int32 i = 0; i < 2; i++)
    {
        if (Player[i])
            Player[i]->DrawCharacter();
    }

    //also draw round manager, ui, etc.
}
