/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnTick()
{
	static float verticalRecoil = 2;
	Ped player = PLAYER_PED_ID();
	if (IS_PED_SHOOTING(player)) 
	{
		float horizontalRecoil = g_random.GetRandomInt(-30, 30);
		horizontalRecoil = horizontalRecoil / 10;
		SET_GAMEPLAY_CAM_RELATIVE_PITCH(GET_GAMEPLAY_CAM_RELATIVE_PITCH() + verticalRecoil, 1.f);
		SET_GAMEPLAY_CAM_RELATIVE_HEADING(GET_GAMEPLAY_CAM_RELATIVE_HEADING() + horizontalRecoil);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_HEAVY_RECOIL, nullptr, nullptr, OnTick);
