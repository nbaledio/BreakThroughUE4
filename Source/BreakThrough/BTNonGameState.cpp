// Copyright 2020 ShatterPoint Games. All Rights Reserved.

#include "BTNonGameState.h"

// UE4: allow Windows platform types to avoid naming collisions
//  this must be undone at the bottom of this file
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"

#include <timeapi.h>

void NonGameState::SetConnectState(GGPOPlayerHandle handle, EPlayerConnectState state) {
    for (int i = 0; i < num_players; i++) {
        if (players[i].handle == handle) {
            players[i].connect_progress = 0;
            players[i].state = state;
            break;
        }
    }
}

void NonGameState::SetDisconnectTimeout(GGPOPlayerHandle handle, int when, int timeout) {
    for (int i = 0; i < num_players; i++) {
        if (players[i].handle == handle) {
            players[i].disconnect_start = when;
            players[i].disconnect_timeout = timeout;
            players[i].state = EPlayerConnectState::Disconnecting;
            break;
        }
    }
}

float NonGameState::GetDisconnectTime(int32 index) const
{
    FPlayerConnectionInfo info = players[index];
    return ((timeGetTime() - info.disconnect_start) * 1.0f) / info.disconnect_timeout;
}

void NonGameState::SetConnectState(EPlayerConnectState state) {
    for (int i = 0; i < num_players; i++) {
        players[i].state = state;
    }
}

void NonGameState::UpdateConnectProgress(GGPOPlayerHandle handle, int progress) {
    for (int i = 0; i < num_players; i++) {
        if (players[i].handle == handle) {
            players[i].connect_progress = progress;
            break;
        }
    }
}

int fletcher32_checksum(short* data, size_t len)
{
    int sum1 = 0xffff, sum2 = 0xffff;

    while (len) {
        size_t tlen = len > 360 ? 360 : len;
        len -= tlen;
        do {
            sum1 += *data++;
            sum2 += sum1;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}

uint32 get_time()
{
    return timeGetTime();
}

// UE4: disallow windows platform types
//  this was enabled at the top of the file
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
