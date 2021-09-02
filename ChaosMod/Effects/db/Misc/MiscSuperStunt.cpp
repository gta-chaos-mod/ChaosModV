/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStart()
{
	Ped player = PLAYER_PED_ID();
	Hash rampHash = GET_HASH_KEY("prop_mp_ramp_03");
	Vector3 playerPos = GET_ENTITY_COORDS(player, false);
	Vector3 rampPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, 5, 0);

	Vector3 min;
	Vector3 max;
	GET_MODEL_DIMENSIONS(rampHash, &min, &max);
	Vector3 rampSize = max - min;

	Object prevRamp = 0;
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		// Offset ramp to connect to the previous one
		if (prevRamp)
		{
			rampPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(prevRamp, 0, rampSize.y, rampSize.z);
		}

		Object ramp = CREATE_OBJECT(rampHash, rampPos.x, rampPos.y, rampPos.z, true, false, true);

		// Only place it on the ground if it is the first ramp
		if (!prevRamp)
		{
			PLACE_OBJECT_ON_GROUND_PROPERLY(ramp);
			rampPos = GET_ENTITY_COORDS(ramp, false);
			SET_ENTITY_COORDS(ramp, rampPos.x, rampPos.y, rampPos.z - 0.3, true, true, true, false);
		}
		else
		{
			SET_ENTITY_COORDS(ramp, rampPos.x, rampPos.y, rampPos.z, true, true, true, false);
		}

		SET_ENTITY_ROTATION(ramp, GET_ENTITY_PITCH(player), -GET_ENTITY_ROLL(player), GET_ENTITY_HEADING(player), 0, true);
		
		if (prevRamp)
		{
			SET_ENTITY_AS_NO_LONGER_NEEDED(&prevRamp);
		}

		prevRamp = ramp;
	}

	SET_ENTITY_AS_NO_LONGER_NEEDED(&prevRamp);
}

static RegisterEffect registerEffect(EFFECT_MISC_SUPER_STUNT, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Super Stunt",
		.Id = "misc_superstunt"
	}
);