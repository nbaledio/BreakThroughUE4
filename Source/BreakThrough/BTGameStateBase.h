// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BTGameState.h"
#include "BTNonGameState.h"
#include "BTGameStateBase.generated.h"

// Forward declarations
class UGGPONetwork;

#define NETWORK_GRAPH_STEPS 720

UENUM(BlueprintType)
enum class ENetworkGraphType : uint8
{
    PING           UMETA(DisplayName = "Ping"),
    SYNC           UMETA(DisplayName = "Fairness"),
    REMOTE_SYNC    UMETA(DisplayName = "Remote Fairness"),
};

USTRUCT(BlueprintType)
struct FNetworkGraphData {
    GENERATED_USTRUCT_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int32   Fairness;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int32   RemoteFairness;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int32   Ping;
};

USTRUCT(BlueprintType)
struct FNetworkGraphPlayer {
    GENERATED_USTRUCT_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        TArray<FNetworkGraphData> PlayerData;
};

/**
 * 
 */
UCLASS()
class BREAKTHROUGH_API ABTGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
    GGPOSession* ggpo = nullptr;

    GameState gs;
    NonGameState ngs;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

protected:
    int32 GetLocalInputs(uint8 PlayerIndex = 0);

    virtual void TickGameState();

    //Run a single frame of the game.
    virtual void RunFrame(int32 local_input);

    // Advances the game state by exactly 1 frame using the inputs specified for player 1 and player 2.
    virtual void AdvanceFrame(int32 inputs[], int32 disconnect_flags);

    // Spend our idle time in ggpo so it can use whatever time we have left over for its internal bookkeeping.
    void Idle(int32 time);
    void ExitGame();

    //Disconnects a player from the session.
    void DisconnectPlayer(int32 player);
	
private:
    //Gets a GGPOSessionCallbacks object with its callback functions assigned.
    GGPOSessionCallbacks CreateCallbacks();

    bool __cdecl BTBeginGameCallback(const char*);

    //Save the current state to a buffer and return it to GGPO via the buffer and len parameters.
    bool __cdecl BTSaveGameStateCallback(unsigned char** buffer, int32* len, int32* checksum, int32);

    // Makes our current state match the state passed in by GGPO.
    bool __cdecl BTLoadGameStateCallback(unsigned char* buffer, int32 len);

    //Log the gamestate.  Used by the synctest debugging tool.
    bool __cdecl BTLogGameState(char* filename, unsigned char* buffer, int32);

    //Free a save state buffer previously returned in SaveGameStateCallback.
    void __cdecl BTFreeBuffer(void* buffer);

    //Notification from GGPO we should step foward exactly 1 frame during a rollback.
    bool __cdecl BTAdvanceFrameCallback(int32);

    //Notification from GGPO that something has happened. Update the status text at the bottom of the screen to notify the user.
    bool __cdecl BTOnEventCallback(GGPOEvent* info);

};
