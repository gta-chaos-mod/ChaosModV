/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{
	SET_PED_INFINITE_AMMO_CLIP(PLAYER_PED_ID(), false);
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	SET_PED_INFINITE_AMMO_CLIP(playerPed, true);
	if (IS_PED_WEAPON_READY_TO_SHOOT(playerPed))
	{
		int count = 5;
		float maxRange = GET_MAX_RANGE_OF_CURRENT_PED_WEAPON(playerPed);
		Vector3 playerCoord = GET_ENTITY_COORDS(playerPed, false);
		for (Ped ped : GetAllPeds())
		{
			REQUEST_PED_VISIBILITY_TRACKING(ped);
			if (ped != playerPed && IS_TRACKED_PED_VISIBLE(ped) && !IS_PED_DEAD_OR_DYING(ped, true) && IS_PED_HUMAN(ped) && !IS_PED_A_PLAYER(ped) && IS_TRACKED_PED_VISIBLE(ped))
			{
				Vector3 pedCoord = GET_ENTITY_COORDS(ped, 0);
				float distance = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, pedCoord.x, pedCoord.y, pedCoord.z, true);
				if (distance <= maxRange)
				{
					Vector3 headVector = GET_PED_BONE_COORDS(ped, 0x796E, 0, 0, 0); // Head
					SET_PED_SHOOTS_AT_COORD(playerPed, headVector.x, headVector.y, headVector.z, true);
					// Only 5 shots per Tick
					if (--count <= 0)
					{
						break;
					}
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_AIMBOT, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Aimbot",
		.Id = "player_aimbot",
		.IsTimed = true
	}
);