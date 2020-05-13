#include <stdafx.h>

static void OnStartLSIA()
{
	TeleportPlayer(-1388.6f, -3111.61f, 13.94f, 329.3f);
}

static RegisterEffect registerEffect1(EFFECT_TP_LSAIRPORT, OnStartLSIA);

static void OnStartMazeTower()
{
	TeleportPlayer(-75.7f, -818.62f, 326.16f, 228.09f);
}

static RegisterEffect registerEffect2(EFFECT_TP_MAZETOWER, OnStartMazeTower);

static void OnStartFortZancudo()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		TeleportPlayer(-2360.3f, 3244.83f, 92.9f, 150.23f);
	}
	else
	{
		TeleportPlayer(-2267.89f, 3121.04f, 32.5f, 72.16f);
	}
}

static RegisterEffect registerEffect3(EFFECT_TP_FORTZANCUDO, OnStartFortZancudo);

static void OnStartMountChilliad()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		TeleportPlayer(501.77f, 5604.85f, 797.91f, 174.7f);
	}
	else
	{
		TeleportPlayer(503.33f, 5531.91f, 777.45f, 190.5f);
	}
}

static RegisterEffect registerEffect4(EFFECT_TP_MOUNTCHILLIAD, OnStartMountChilliad);

static void OnStartSkyFall()
{
	TeleportPlayer(935.f, 3800.f, 2300.f, 165.f);
}

static RegisterEffect registerEffect5(EFFECT_TP_SKYFALL, OnStartSkyFall);

static void OnStartWaypoint()
{
	Vector3 coords;
	bool found = false;
	if (IS_WAYPOINT_ACTIVE())
	{
		coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		found = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			Blip blip = GET_FIRST_BLIP_INFO_ID(i);
			if (DOES_BLIP_EXIST(blip))
			{
				coords = GET_BLIP_COORDS(blip);
				found = true;

				break;
			}
		}
	}

	auto playerPed = PLAYER_PED_ID();

	if (found)
	{
		float z;
		if (!GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, 1000.f, &z, false, false))
		{
			z = GET_ENTITY_COORDS(playerPed, false).z;
		}

		TeleportPlayer(coords.x, coords.y, z, GET_ENTITY_HEADING(playerPed));
	}
}

static RegisterEffect registerEffect6(EFFECT_TP_WAYPOINT, OnStartWaypoint);

static void OnStartFront()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.f, 5.f, 0.f);

	SET_ENTITY_COORDS(playerPed, newPos.x, newPos.y, newPos.z, false, false, false, false);
}

static RegisterEffect registerEffect7(EFFECT_TP_FRONT, OnStartFront);
