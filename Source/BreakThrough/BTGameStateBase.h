// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BTGameState.h"
#include "BTNonGameState.h"
#include "Kismet/GameplayStatics.h"
#include "BTGameStateBase.generated.h"

// Forward declarations
class UGGPONetwork;

#define ARRAYSIZE(a) sizeof(a) / sizeof(a[0])
#define FRAME_RATE 60
#define ONE_FRAME (1.0f / FRAME_RATE)
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
    // Sets default values for this actor's properties
    ABTGameStateBase();

    GGPOSession* ggpo = nullptr;

    BTGameState gs;
    NonGameState ngs;
    uint8 FrameDelay = 0;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    bool bSessionStarted;

    float ElapsedTime;

    TArray<FNetworkGraphPlayer> NetworkGraphData;

    int32 GetLocalInputs(uint8 PlayerIndex = 0);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Characters")
    TArray<TSubclassOf<class ABTCharacterBase>> CharacterBlueprints;

    void SpawnCharacters();

    void SpawnRoundManager();

    virtual void TickGameState();

    //Run a single frame of the game.
    virtual void RunFrame(int32 local_input);

    // Advances the game state by exactly 1 frame using the inputs specified for player 1 and player 2.
    virtual void AdvanceFrame(int32 inputs[], int32 disconnect_flags = 0);

    void DrawFrame();

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

    void BeginOnlineSession();

    void OnSessionStart();

    TArray<FVector2D> GetNetworkGraphData(int32 Index, ENetworkGraphType Type, FVector2D GraphSize, int32 MinY, int32 MaxY) const;

    static float GraphValue(int32 Value, FVector2D GraphSize, int32 MinY, int32 MaxY);

    TArray<FGGPONetworkStats> GetNetworkStats();

    //Starts a GGPO game session.
    bool TryStartGGPOPlayerSession(int32 NumPlayers, const UGGPONetwork* NetworkAddresses);
    // Starts a GGPO spectating game session.
    bool TryStartGGPOSpectatorSession(const uint16 LocalPort, const int32 NumPlayers, wchar_t* HostParameter);

    void InitOnlineGame(uint16 localport, int32 num_players, GGPOPlayer* players, int32 num_spectators);

    void InitSpectator(uint16 localport, int32 num_players, char* host_ip, uint16 host_port);

};
