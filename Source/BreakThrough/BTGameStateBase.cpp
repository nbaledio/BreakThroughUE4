// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTGameStateBase.h"
#include "BreakThroughPlayerController.h"
#include "include/ggponet.h"
#include "GGPOGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#define FRAME_RATE 60
#define ONE_FRAME (1.0f / FRAME_RATE)

void ABTGameStateBase::BeginPlay()
{
    Super::BeginPlay();

}

void ABTGameStateBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

int32 ABTGameStateBase::GetLocalInputs(uint8 PlayerIndex)
{
    const UObject* world = (UObject*)GetWorld();
    ABreakThroughPlayerController* Controller = Cast<ABreakThroughPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
    if (Controller)
    {
        return Controller->GetPlayerInput();
    }
    return 0;
}

void ABTGameStateBase::TickGameState()
{

}

void ABTGameStateBase::RunFrame(int32 local_input)
{
    GGPOErrorCode result = GGPO_OK;
    int disconnect_flags;
    int inputs[2] = { 0 };

    if (ngs.local_player_handle != GGPO_INVALID_HANDLE) 
    {
#if defined(SYNC_TEST)
        local_input = rand(); // test: use random inputs to demonstrate sync testing
#endif
        result = GGPONet::ggpo_add_local_input(ggpo, ngs.local_player_handle, &local_input, sizeof(local_input));
    }

    // synchronize these inputs with ggpo.  If we have enough input to proceed
    // ggpo will modify the input list with the correct inputs to use and
    // return 1.
    if (GGPO_SUCCEEDED(result)) {
        result = GGPONet::ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
        if (GGPO_SUCCEEDED(result)) {
            // inputs[0] and inputs[1] contain the inputs for p1 and p2.  Advance
            // the game by 1 frame using those inputs.
            AdvanceFrame(inputs, disconnect_flags);
        }
    }
}

void ABTGameStateBase::AdvanceFrame(int32 inputs[], int32 disconnect_flags)
{
    gs.Update(inputs, disconnect_flags);

    /*// update the checksums to display in the top of the window.  this
    // helps to detect desyncs.
    ngs.now.framenumber = gs.FrameNumber;
    ngs.now.checksum = fletcher32_checksum((short*)&gs, sizeof(gs) / 2);
    if ((gs.FrameNumber % 90) == 0) {
        ngs.periodic = ngs.now;
    }

    // Notify ggpo that we've moved forward exactly 1 frame.
    GGPONet::ggpo_advance_frame(ggpo);

    // Update the performance monitor display.
    GGPOPlayerHandle handles[MAX_PLAYERS];
    int count = 0;
    for (int i = 0; i < ngs.num_players; i++) {
        if (ngs.players[i].type == EGGPOPlayerType::REMOTE) {
            handles[count++] = ngs.players[i].handle;
        }
    }*/
}

void ABTGameStateBase::Idle(int32 time)
{
    GGPONet::ggpo_idle(ggpo, time);
}

void ABTGameStateBase::ExitGame()
{
    memset(&gs, 0, sizeof(gs));
    memset(&ngs, 0, sizeof(ngs));

    if (ggpo) {
        GGPONet::ggpo_close_session(ggpo);
        ggpo = NULL;
    }
}

void ABTGameStateBase::DisconnectPlayer(int32 player)
{
    if (player < ngs.num_players) {
        GGPOErrorCode result = GGPONet::ggpo_disconnect_player(ggpo, ngs.players[player].handle);
        if (GGPO_SUCCEEDED(result)) 
        {
            UE_LOG(LogTemp, Warning, TEXT("Disconnected player %d.\n"), player);
        }
        else 
        {
            UE_LOG(LogTemp, Warning, TEXT("Error while disconnecting player (err:%d).\n"), result);
        }
    }
}

GGPOSessionCallbacks ABTGameStateBase::CreateCallbacks()
{
    GGPOSessionCallbacks cb = { 0 };

    cb.begin_game = std::bind(&ABTGameStateBase::BTBeginGameCallback, this, std::placeholders::_1);
    cb.save_game_state = std::bind(&ABTGameStateBase::BTSaveGameStateCallback, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    cb.load_game_state = std::bind(&ABTGameStateBase::BTLoadGameStateCallback, this,
        std::placeholders::_1, std::placeholders::_2);
    cb.log_game_state = std::bind(&ABTGameStateBase::BTLogGameState, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    cb.free_buffer = std::bind(&ABTGameStateBase::BTFreeBuffer, this, std::placeholders::_1);
    cb.advance_frame = std::bind(&ABTGameStateBase::BTAdvanceFrameCallback, this, std::placeholders::_1);
    cb.on_event = std::bind(&ABTGameStateBase::BTOnEventCallback, this, std::placeholders::_1);

    return cb;
}


bool ABTGameStateBase::BTBeginGameCallback(const char*)
{
    return true;
}
bool ABTGameStateBase::BTSaveGameStateCallback(unsigned char** buffer, int32* len, int32* checksum, int32)
{
    *len = sizeof(gs);
    *buffer = (unsigned char*)malloc(*len);
    if (!*buffer) {
        return false;
    }
    memcpy(*buffer, &gs, *len);
    *checksum = fletcher32_checksum((short*)*buffer, *len / 2);
    return true;
}
bool ABTGameStateBase::BTLoadGameStateCallback(unsigned char* buffer, int32 len)
{
    memcpy(&gs, buffer, len);
    return true;
}
bool ABTGameStateBase::BTLogGameState(char* filename, unsigned char* buffer, int32)
{
    /*FILE* fp = nullptr;
    fopen_s(&fp, filename, "w");
    if (fp) {
        GameState* gamestate = (GameState*)buffer;
        fprintf(fp, "GameState object.\n");
        fprintf(fp, "  bounds: %ld,%ld x %ld,%ld.\n", gamestate->_bounds.left, gamestate->_bounds.top,
            gamestate->_bounds.right, gamestate->_bounds.bottom);
        fprintf(fp, "  num_ships: %d.\n", gamestate->_num_ships);
        for (int i = 0; i < gamestate->_num_ships; i++) {
            Ship* ship = gamestate->_ships + i;
            fprintf(fp, "  ship %d position:  %.4f, %.4f\n", i, ship->position.x, ship->position.y);
            fprintf(fp, "  ship %d velocity:  %.4f, %.4f\n", i, ship->velocity.dx, ship->velocity.dy);
            fprintf(fp, "  ship %d radius:    %d.\n", i, ship->radius);
            fprintf(fp, "  ship %d heading:   %d.\n", i, ship->heading);
            fprintf(fp, "  ship %d health:    %d.\n", i, ship->health);
            fprintf(fp, "  ship %d speed:     %d.\n", i, ship->speed);
            fprintf(fp, "  ship %d cooldown:  %d.\n", i, ship->cooldown);
            fprintf(fp, "  ship %d score:     %d.\n", i, ship->score);
            for (int j = 0; j < MAX_BULLETS; j++) {
                Bullet* bullet = ship->bullets + j;
                fprintf(fp, "  ship %d bullet %d: %.2f %.2f -> %.2f %.2f.\n", i, j,
                    bullet->position.x, bullet->position.y,
                    bullet->velocity.dx, bullet->velocity.dy);
            }
        }
        fclose(fp);
    }*/
    return true;
}
void ABTGameStateBase::BTFreeBuffer(void* buffer)
{
    free(buffer);
}
bool ABTGameStateBase::BTAdvanceFrameCallback(int32)
{
    int inputs[2] = { 0 };
    int disconnect_flags;

    // Make sure we fetch new inputs from GGPO and use those to update
    // the game state instead of reading from the keyboard.
    GGPONet::ggpo_synchronize_input(ggpo, (void*)inputs, sizeof(int) * 2, &disconnect_flags);
    AdvanceFrame(inputs, disconnect_flags);
    return true;
}
bool ABTGameStateBase::BTOnEventCallback(GGPOEvent* info)
{
    int progress;
    switch (info->code) {
    case GGPO_EVENTCODE_CONNECTED_TO_PEER:
        ngs.SetConnectState(info->u.connected.player, EPlayerConnectState::Synchronizing);
        break;
    case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
        progress = 100 * info->u.synchronizing.count / info->u.synchronizing.total;
        ngs.UpdateConnectProgress(info->u.synchronizing.player, progress);
        break;
    case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
        ngs.UpdateConnectProgress(info->u.synchronized.player, 100);
        break;
    case GGPO_EVENTCODE_RUNNING:
        ngs.SetConnectState(EPlayerConnectState::Running);
        break;
    case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
        ngs.SetDisconnectTimeout(info->u.connection_interrupted.player,
            get_time(),
            info->u.connection_interrupted.disconnect_timeout);
        break;
    case GGPO_EVENTCODE_CONNECTION_RESUMED:
        ngs.SetConnectState(info->u.connection_resumed.player, EPlayerConnectState::Running);
        break;
    case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
        ngs.SetConnectState(info->u.disconnected.player, EPlayerConnectState::Disconnected);
        break;
    case GGPO_EVENTCODE_TIMESYNC:
        FPlatformProcess::Sleep(info->u.timesync.frames_ahead / 60);
        break;
    }
    return true;
}
