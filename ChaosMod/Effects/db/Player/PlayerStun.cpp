/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

std::string effectName = "Dont_tazeme_bro";

static void OnStart()
{
	Ped plr = PLAYER_PED_ID();
	
	Vector3 pos1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(plr, 0, 1, 0);
	Vector3 pos2 = GET_ENTITY_COORDS(plr, 1);

	SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, 0, 0, 0x3656C8C1, 0, 0, 1, 100);
	
	ANIMPOSTFX_PLAY(effectName.c_str(), 0, 0);
	WAIT(4600);
	ANIMPOSTFX_STOP(effectName.c_str());
}

static RegisterEffect registerEffect(EFFECT_PLAYER_STUN, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "That's Stunning",
		.Id = "player_stun"
	}
);
