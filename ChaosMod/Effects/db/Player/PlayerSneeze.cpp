/*
	Effect By The Genius OnlyRealNubs
*/

#include "stdafx.h"

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	Ped plr = PLAYER_PED_ID();

	if (lastTick < curTick - 3000)
	{
		lastTick = curTick;

		PLAY_FACIAL_ANIM(plr, "melee_effort_1", "facials@p_m_one@base");
		//WAIT(0);
		PLAY_PAIN(plr, 30, 8, 0);
	}
}

static void OnStart()
{
	REQUEST_ANIM_DICT("facials@p_m_one@base");
	while (!HAS_ANIM_DICT_LOADED("facials@p_m_one@base"))
	{
		WAIT(0);
	}
}

static RegisterEffect registerEffect(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Bless You",
		.Id = "player_sneeze",
		.IsTimed = true,
		.IsShortDuration = true,
	}
);
