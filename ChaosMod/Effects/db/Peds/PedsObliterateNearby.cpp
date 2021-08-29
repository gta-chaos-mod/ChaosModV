#include <stdafx.h>

static void OnStart()
{
	REQUEST_NAMED_PTFX_ASSET("scr_xm_orbital");
	REQUEST_NAMED_PTFX_ASSET("scr_xm_orbital_blast");

	int count = 5;

	std::vector<Ped> peds;
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			Vector3 pos = GET_ENTITY_COORDS(ped, false);

			USE_PARTICLE_FX_ASSET("scr_xm_orbital");

			START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_xm_orbital_blast", pos.x, pos.y, pos.z, .0f, .0f, .0f, 1.f, 0, 0, 0, 0);

			PLAY_SOUND_FROM_COORD(-1, "DLC_XM_Explosions_Orbital_Cannon", pos.x, pos.y, pos.z, 0, 1, 0, 0);

			ADD_EXPLOSION(pos.x, pos.y, pos.z, 9, 100.f, true, false, 3.f, false);

			// In case they're explosion proof
			SET_ENTITY_HEALTH(ped, 0, false);

			peds.push_back(ped);

			if (--count == 0)
			{
				count = 5;

				// Too many particles in 1 frame = won't show
				// So we wait a bit
				WAIT(0);
			}
		}
	}

	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier - 1; i++)
	{
		WAIT(100);

		for (Ped ped : peds)
		{
			if (ped && DOES_ENTITY_EXIST(ped))
			{
				Vector3 pos = GET_ENTITY_COORDS(ped, false);

				USE_PARTICLE_FX_ASSET("scr_xm_orbital");

				START_NETWORKED_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_xm_orbital_blast", pos.x, pos.y, pos.z, .0f, .0f, .0f, 1.f, 0, 0, 0, 0);

				PLAY_SOUND_FROM_COORD(-1, "DLC_XM_Explosions_Orbital_Cannon", pos.x, pos.y, pos.z, 0, 1, 0, 0);

				ADD_EXPLOSION(pos.x, pos.y, pos.z, 9, 100.f, true, false, 3.f, false);

				if (--count == 0)
				{
					count = 5;

					// Too many particles in 1 frame = won't show
					// So we wait a bit
					WAIT(0);
				}
			}
		}
	}

	REMOVE_NAMED_PTFX_ASSET("scr_xm_orbital");
	REMOVE_NAMED_PTFX_ASSET("scr_xm_orbital_blast");
}

static RegisterEffect registerEffect(EFFECT_NEARBY_PEDS_OBLITERATE, OnStart, EffectInfo
	{
		.Name = "Obliterate All Nearby Peds",
		.Id = "peds_obliterate"
	}
);