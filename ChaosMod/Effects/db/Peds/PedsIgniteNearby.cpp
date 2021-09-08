#include <stdafx.h>

static void OnStart()
{
	int count = 5;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			START_ENTITY_FIRE(ped);

			if (g_MetaInfo.m_fChaosMultiplier > 1)
			{
				Vector3 pedCoords = GET_ENTITY_COORDS(ped, true);
				ADD_EXPLOSION(pedCoords.x, pedCoords.y, pedCoords.z, 3, 40.f, true, false, 0.f, false); // Create molotov explosion at ped

				if (--count == 0)
				{
					WAIT(0);
					count = 5;
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_IGNITE_PEDS, OnStart, EffectInfo
	{
		.Name = "Ignite All Nearby Peds",
		.Id = "peds_ignite"
	}
);