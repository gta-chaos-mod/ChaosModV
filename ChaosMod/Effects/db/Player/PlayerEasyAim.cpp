#include <stdafx.h>

// Effect by ProfessorBiddle, modified

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	//detect player aiming
	if (GET_PED_CONFIG_FLAG(player, 78, 1))
	{
		//slow time
		SET_TIME_SCALE(.3);
	}
	else
	{
		SET_TIME_SCALE(1);
	}
}

static void OnStop()
{
	SET_TIME_SCALE(1);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_EASY_AIM, nullptr, OnStop, OnTick);
