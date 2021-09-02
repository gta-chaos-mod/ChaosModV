/*
	Effect by Last0xygen, modified by Reguas
*/

#include <stdafx.h>
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStart()
{
	bool cutscenePlaying = IS_CUTSCENE_PLAYING();

	Hash blimpHash = GET_HASH_KEY("blimp");

	static const Hash michaelHash = 0x0D7114C9;
	static const Hash franclinHash = 0x9B22DBAF;
	static const Hash trevorHash = 0x9B810FA2;

	LoadModel(blimpHash);
	
	if (!cutscenePlaying)
	{
		REQUEST_CUTSCENE("fbi_1_int", 8);
	}
	
	Hooks::EnableScriptThreadBlock();
	Vehicle veh = CREATE_VEHICLE(blimpHash, -370.490, 1029.085, 345.090, 53.824, true, false, false);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	Ped player = PLAYER_PED_ID();
	SET_ENTITY_INVINCIBLE(player, true);
	SET_PED_INTO_VEHICLE(player, veh, -1);
	SET_VEHICLE_FORWARD_SPEED(veh, 45);
	TASK_LEAVE_VEHICLE(player, veh, 4160);

	int waited = 0;

	while (!IS_PED_GETTING_UP(player) && waited < 100)
	{
		WAIT(100);
		waited++;
	}

	SET_ENTITY_INVINCIBLE(player, false);

	waited = 0;

	if (!cutscenePlaying)
	{

		while (!HAS_CUTSCENE_LOADED() && waited < 100) // for proper cutscene play
		{
			WAIT(100);
		}

		REGISTER_ENTITY_FOR_CUTSCENE(player, "MICHAEL", 0, 0, 64);

		START_CUTSCENE(0);
		WAIT(8500);
		STOP_CUTSCENE_IMMEDIATELY();

	}
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
