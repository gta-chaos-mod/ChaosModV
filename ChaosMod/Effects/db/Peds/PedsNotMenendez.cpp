/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Audio.h"
#include <set>

CHAOS_VAR std::set<Ped> deadPeds;

static void OnStart()
{
	deadPeds = {};
}

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 curTick         = GetTickCount64();

	bool flag               = false;
	for (Ped ped : GetAllPeds())
	{
		if (!deadPeds.contains(ped) && IS_ENTITY_DEAD(ped, false))
		{
			deadPeds.insert(ped);
			flag = true;
		}
	}

	if (flag && curTick - lastTick >= 2000)
	{
		lastTick          = curTick;

		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
		Memory::PlayAmbientSpeechAtCoordsWithIndex("BUDDY_DOWN", "s_m_y_blackops_01_white_mini_01", playerPos, 2,
		                                           "SPEECH_PARAMS_FORCE_SHOUTED");
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Not Menendez!",
		.Id = "peds_not_menendez",
		.IsTimed = true
	}
);