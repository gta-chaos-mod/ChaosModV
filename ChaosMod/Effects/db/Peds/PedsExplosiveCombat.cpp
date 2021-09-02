#include <stdafx.h>

static void OnTick()
{
	Player player = PLAYER_ID();

	SET_EXPLOSIVE_MELEE_THIS_FRAME(player);
	SET_EXPLOSIVE_AMMO_THIS_FRAME(player);

	Vector3 impactCoords;

	for (Ped ped : GetAllPeds())
	{
		if (GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &impactCoords))
		{
			for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
			{
				ADD_EXPLOSION(impactCoords.x, impactCoords.y, impactCoords.z, 4, 9999.f, true, false, 1.f, false);

				if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
				{
					WAIT(100);
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_EXPLOSIVE_COMBAT, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Explosive Combat",
		.Id = "player_explosivecombat",
		.IsTimed = true
	}
);