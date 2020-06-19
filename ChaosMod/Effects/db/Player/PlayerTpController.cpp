#include <stdafx.h>

static void OnStartLSIA()
{
	TeleportPlayer(-1388.6f, -3111.61f, 13.94f);
}

static RegisterEffect registerEffect1(EFFECT_TP_LSAIRPORT, OnStartLSIA);

static void OnStartMazeTower()
{
	TeleportPlayer(-75.7f, -818.62f, 326.16f);
}

static RegisterEffect registerEffect2(EFFECT_TP_MAZETOWER, OnStartMazeTower);

static void OnStartFortZancudo()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		TeleportPlayer(-2360.3f, 3244.83f, 92.9f);
	}
	else
	{
		TeleportPlayer(-2267.89f, 3121.04f, 32.5f);
	}
}

static RegisterEffect registerEffect3(EFFECT_TP_FORTZANCUDO, OnStartFortZancudo);

static void OnStartMountChilliad()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
	{
		TeleportPlayer(501.77f, 5604.85f, 797.91f);
	}
	else
	{
		TeleportPlayer(503.33f, 5531.91f, 777.45f);
	}
}

static RegisterEffect registerEffect4(EFFECT_TP_MOUNTCHILLIAD, OnStartMountChilliad);

static void OnStartSkyFall()
{
	TeleportPlayer(935.f, 3800.f, 2300.f);
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

		TeleportPlayer(coords.x, coords.y, z);
	}
}

static RegisterEffect registerEffect6(EFFECT_TP_WAYPOINT, OnStartWaypoint);

static void OnStartFront()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.f, 50.f, 0.f);

	TeleportPlayer(newPos.x, newPos.y, newPos.z);
}

static RegisterEffect registerEffect7(EFFECT_TP_FRONT, OnStartFront);

static void OnStartRandom()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	float x, y, z = playerPos.z, _;
	do
	{
		x = Random::GetRandomInt(-3747.f, 4500.f);
		y = Random::GetRandomInt(-4400.f, 8022.f);
		
	}
	while (TEST_VERTICAL_PROBE_AGAINST_ALL_WATER(x, y, z, 0, &_));

	float groundZ;
	bool useGroundZ;
	for (int i = 0; i < 100; i++)
	{
		float testZ = (i * 10.f) - 100.f;

		TeleportPlayer(x, y, testZ);
		if (i % 5 == 0)
		{
			WAIT(0);
		}

		useGroundZ = GET_GROUND_Z_FOR_3D_COORD(x, y, testZ, &groundZ, false, false);
		if (useGroundZ)
		{
			break;
		}
	}

	TeleportPlayer(x, y, useGroundZ ? groundZ : z);
}

static RegisterEffect registerEffect8(EFFECT_TP_RANDOM, OnStartRandom);
