/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

std::string effectName = "Dont_tazeme_bro";

static void OnStart()
{
	Vector3 pos1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER_PED_ID(), 0, 1, 0);
	Vector3 pos2 = GET_ENTITY_COORDS(PLAYER_PED_ID(), 1);

	Ped owner = GET_RANDOM_PED_AT_COORD(pos1.x, pos1.y, pos1.z, 50, 50, 50, -1);

	SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, 0, 0, 0x3656C8C1, owner, 1, 0, 100);
	
	ANIMPOSTFX_PLAY(effectName.c_str(), 0, 0);
	WAIT(4600);
	ANIMPOSTFX_STOP(effectName.c_str());
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_PLAYER_STUN, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "That's Stunning",
		.Id = "player_stun"
	}
);