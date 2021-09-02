/*
	Effect by Slothersbee, modified
*/

#include <stdafx.h>

static void OnTick()
{
	Player player = PLAYER_ID();

	Entity target;

	STOP_GAMEPLAY_CAM_SHAKING(true);

	if (GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &target))
	{
		if ((IS_ENTITY_A_PED(target) || IS_ENTITY_A_VEHICLE(target)) && !IS_ENTITY_DEAD(target, false))
		{
			Vector3 pos = GET_ENTITY_COORDS(target, false);
			SET_ENTITY_HEALTH(target, 0, 0);
			SET_ENTITY_INVINCIBLE(target, false);

			for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
			{
				ADD_EXPLOSION(pos.x, pos.y, pos.z, 9, 100.f, true, false, 3.f * g_MetaInfo.m_fChaosMultiplier, false);

				if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
				{
					WAIT(500);
				}
			}
		}
	}

}

static RegisterEffect registerEffect(EFFECT_PLAYER_POOF, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Deadly Aim",
		.Id = "player_poof",
		.IsTimed = true
	}
);