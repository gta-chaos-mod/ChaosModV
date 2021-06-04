/*
	Effect by Last0xygen
*/

#include <stdafx.h>
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStart()
{
	Hash blimpHash = GET_HASH_KEY("blimp");
	LoadModel(blimpHash);
	Hooks::EnableScriptThreadBlock();
	Vehicle veh = CREATE_VEHICLE(blimpHash, -377.276, 1055.06, 340.962, 80, true, false, false);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	Ped player = PLAYER_PED_ID();
	SET_ENTITY_INVINCIBLE(player, true);
	SET_PED_INTO_VEHICLE(player, veh, -1);
	SET_VEHICLE_FORWARD_SPEED(veh, 36);
	TASK_LEAVE_VEHICLE(player, veh, 4160);
	WAIT(3000);
	SET_ENTITY_INVINCIBLE(player, false);
	Hooks::DisableScriptThreadBlock();
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BLIMP_STRATS, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Blimp Strats",
        .Id = "player_blimp_strats",
        .EEffectGroupType = EEffectGroupType::Teleport
    }
);
