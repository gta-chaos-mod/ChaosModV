#include <stdafx.h>

#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStartLSIA()
{
	TeleportPlayer(-1388.6f, -3111.61f, 13.94f);
}

static RegisterEffect registerEffect1(EFFECT_TP_LSAIRPORT, OnStartLSIA, EffectInfo
	{
		.Name = "Teleport To LS Airport",
		.Id = "tp_lsairport",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);
static void OnStartMazeTower()
{
	TeleportPlayer(-75.7f, -818.62f, 326.16f);
}

static RegisterEffect registerEffect2(EFFECT_TP_MAZETOWER, OnStartMazeTower, EffectInfo
	{
		.Name = "Teleport To Top Of Maze Bank Tower",
		.Id = "tp_mazebanktower",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);
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

static RegisterEffect registerEffect3(EFFECT_TP_FORTZANCUDO, OnStartFortZancudo, EffectInfo
	{
		.Name = "Teleport To Fort Zancudo",
		.Id = "tp_fortzancudo",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);
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

static RegisterEffect registerEffect4(EFFECT_TP_MOUNTCHILLIAD, OnStartMountChilliad, EffectInfo
	{
		.Name = "Teleport To Mount Chiliad",
		.Id = "tp_mountchilliad",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);
static void OnStartSkyFall()
{
	TeleportPlayer(935.f, 3800.f, 2300.f);
}

static RegisterEffect registerEffect5(EFFECT_TP_SKYFALL, OnStartSkyFall, EffectInfo
	{
		.Name = "Teleport To Heaven",
		.Id = "tp_skyfall",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);

static bool HasValidWaypointForTp()
{
	if (IS_WAYPOINT_ACTIVE())
	{
		return true;
	}

	for (int i = 0; i < 3; i++)
	{
		Blip blip = GET_FIRST_BLIP_INFO_ID(i);
		if (DOES_BLIP_EXIST(blip))
		{
			return true;
		}
	}

	return false;
}
static void OnStartWaypoint()
{
	Vector3 coords;
	bool found = false, playerBlip = false;
	if (IS_WAYPOINT_ACTIVE())
	{
		coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		found = true;
		playerBlip = true;
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

	Ped playerPed = PLAYER_PED_ID();

	if (found)
	{
		float z;
		if (!playerBlip)
		{
			z = coords.z;
		}
		else
		{
			float groundZ;
			bool useGroundZ;
			for (int i = 0; i < 100; i++)
			{
				float testZ = (i * 10.f) - 100.f;

				TeleportPlayer(coords.x, coords.y, testZ);
				if (i % 5 == 0)
				{
					WAIT(0);
				}

				useGroundZ = GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, testZ, &groundZ, false, false);
				if (useGroundZ)
				{
					break;
				}
			}

			if (useGroundZ)
			{
				z = groundZ;
			}
			else
			{
				Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

				z = playerPos.z;
			}
		}

		TeleportPlayer(coords.x, coords.y, z);
	}
}

static RegisterEffect registerEffect6(EFFECT_TP_WAYPOINT, OnStartWaypoint, EffectInfo
	{
		.Name = "Teleport To Waypoint",
		.Id = "player_tptowaypoint"
	}
);
static void OnStartFront()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.f, 50.f, 0.f);
	float groundZ;
	bool useGroundZ = GET_GROUND_Z_FOR_3D_COORD(newPos.x, newPos.y, 1000.f, &groundZ, false, false);

	TeleportPlayer(newPos.x, newPos.y, useGroundZ ? groundZ : newPos.z);
}

static RegisterEffect registerEffect7(EFFECT_TP_FRONT, OnStartFront, EffectInfo
	{
		.Name = "Teleport Player A Few Meters",
		.Id = "player_tpfront"
	}
);
static void OnStartRandom()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	float x, y, z = playerPos.z, _;
	do
	{
		x = g_Random.GetRandomInt(-3747.f, 4500.f);
		y = g_Random.GetRandomInt(-4400.f, 8022.f);

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

static RegisterEffect registerEffect8(EFFECT_TP_RANDOM, OnStartRandom, EffectInfo
	{
		.Name = "Teleport To Random Location",
		.Id = "tp_random",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);

static std::vector<Blip> GetAllMissionBlips()
{
	std::vector<Hash> excludedColors;
	switch (GET_ENTITY_MODEL(PLAYER_PED_ID()))
	{
	case 225514697: // Michael 
		excludedColors = { 43, 44 };
		break;
	case 2602752943: // Franklin
		excludedColors = { 42, 44 };
		break;
	case 2608926626: // Trevor
		excludedColors = { 42, 43 };
		break;
	default: // default
		break;
	}
	const int possibleBlipIds[] = { 66, 76, 77, 78, 79, 80, 86, 88 , 89, 96, 104, 105, 106, 107, 112, 113, 118, 120, 123, 124, 208, 209, 210, 211, 214, 267, 293, 355, 363, 381, 382, 383, 384, 385, 386, 387, 388, 389, 428, 445, 447, 448, 449, 450, 451, 452, 453, 454 };
	std::vector<Blip> validBlips;
	for (int i : possibleBlipIds)
	{
		std::vector<Blip> checkedBlips;
		Blip nextBlip = GET_FIRST_BLIP_INFO_ID(i);
		while (DOES_BLIP_EXIST(nextBlip))
		{
			int color = GET_BLIP_COLOUR(nextBlip);
			// Filter out missions for other players (ignore Trevor- and Franklin-Blips for Michael)
			if (std::find(excludedColors.begin(), excludedColors.end(), color) == excludedColors.end())
			{
				validBlips.push_back(nextBlip);
			}
			nextBlip = GET_NEXT_BLIP_INFO_ID(i);
		}
	}

	return validBlips;
}
static void OnStartMission()
{
	std::vector<Blip> validBlips = GetAllMissionBlips();
	if (validBlips.size() >= 1)
	{
		Blip randomBlip = validBlips[g_Random.GetRandomInt(0, validBlips.size() - 1)];
		Vector3 blipCoord = GET_BLIP_COORDS(randomBlip);
		TeleportPlayer(blipCoord.x, blipCoord.y, blipCoord.z, true);
	}
}

static RegisterEffect registerEffectMission(EFFECT_TP_MISSION, OnStartMission, EffectInfo
	{
		.Name = "Teleport To Random Mission",
		.Id = "tp_mission"
	}
);

/*
	Effect by MoneyWasted
*/

static std::vector<Vector3> allPossibleStores =
{
	// Convenience Stores
	{ 372.29217529297f, 326.39370727539f, 103.56636047363f },   // 247 Clinton
	{ 24.215274810791f, -1347.2624511719f, 29.497016906738f },  // 247 Strawberry
	{ 2557.1748046875f, 380.64489746094f, 108.62294006348f },   // 247 Palomino
	{ -3038.2673339844f, 584.47491455078f, 7.908935546875f },   // 247 Ineseno
	{ -3242.2670898438f, 999.76306152344f, 12.830704689026f },  // 247 Barbareno
	{ 549.44256591797f, 2671.2185058594f, 42.156513214111f },   // 247 Route 68
	{ 1959.9187011719f, 3740.0014648438f, 32.343738555908f },   // 247 Alhambra
	{ 1727.7840576172f, 6415.3408203125f, 35.037250518799f },   // 247 Senora
	{ 2677.9306640625f, 3279.3017578125f, 55.241123199463f },   // 247 Route 13
	{ -2966.3012695313f, 391.58193969727f, 15.043300628662f },  // RobsLiquor Great Ocean
	{ -1487.2850341797f, -376.92288208008f, 40.163436889648f }, // RobsLiquor Prosperity
	{ -1221.3229980469f, -908.12780761719f, 12.326356887817f }, // RobsLiquor San Andreas
	{ 1134.0545654297f, -983.3251953125f, 46.415802001953f },   // RobsLiquor El Rancho
	{ 1165.2305908203f, 2710.9692382813f, 38.157665252686f },   // RobsLiquor Route 68
	{ -705.91625976563f, -913.41326904297f, 19.215585708618f }, // LTD Vespucci
	{ -46.958980560303f, -1758.9643554688f, 29.420999526978f }, // LTD Davis
	{ 1165.1630859375f, -323.87414550781f, 69.205047607422f },  // LTD Mirror Park
	{ -1819.5125732422f, 793.64141845703f, 138.08486938477f },  // LTD Banham Canyon
	{ 1697.1395263672f, 4923.4130859375f, 42.063632965088f },   // LTD Grapeseed

	// Ammunitions
	{ 810.94f, -2157.19f, 29.62f },     // Ammunition (Cypress Flats)
	{ 21.15f, -1106.44f, 29.8f },       // Ammunition (PillBox Hill)
	{ 842.55f, -1033.72f, 28.19f },      // Ammunition (La Mesa)
	{ -662.42f, -935.19f, 21.83f },      // Ammunition (Cypress Flats)
	{ -1305.54f, -394.7f, 36.7f },    // Ammunition (MorningWood)
	{ 251.97f, -50.19f, 69.94f },       // Ammunition (Hawick)
	{ 2567.91f, 294.05f, 108.73f },      // Ammunition (Montañas Tataviam)
	{ 1693.32f, 3760.19f, 34.70f },      // Ammunition (Sandy Shores)
	{ -330.61f, 6084.02f, 31.45f },     // Ammunition (Sandy Shores)
	{ -1118.11f, 2698.71f, 18.55f },    // Ammunition (Sandy Shores)

	// Clothing Stores
	{ 72.3f, -1399.1f, 28.4f },
	{ -703.8f, -152.3f, 36.4f },
	{ -167.9f, -299.0f, 38.7f },
	{ 428.7f, -800.1f, 28.5f },
	{ -829.4f, -1073.7f, 10.3f },
	{ -1447.8f, -242.5f, 48.8f },
	{ 11.6f, 6514.2f, 30.9f },
	{ 123.6f, -219.4f, 53.6f },
	{ 1696.3f, 4829.3f, 41.1f },
	{ 618.1f, 2759.6f, 41.1f },
	{ 1190.6f, 2713.4f, 37.2f },
	{ -1193.4f, -772.3f, 16.3f },
	{ -3172.5f, 1048.1f, 19.9f },
	{ -1108.4f, 2708.9f, 18.1f }
};

static void OnStartStore()
{
	TeleportPlayer(allPossibleStores.at(g_Random.GetRandomInt(0, allPossibleStores.size() - 1)));
}

static RegisterEffect registerEffect(EFFECT_TP_TO_STORE, OnStartStore, EffectInfo
	{
		.Name = "Teleport to Random Store",
		.Id = "player_tp_store",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);

static const std::vector<EEffectType> fakeTpTypes =
{
	{EFFECT_TP_LSAIRPORT},
	{EFFECT_TP_MAZETOWER},
	{EFFECT_TP_FORTZANCUDO},
	{EFFECT_TP_MOUNTCHILLIAD},
	{EFFECT_TP_SKYFALL},
	{EFFECT_TP_MISSION},
	{EFFECT_TP_RANDOM},
	{EFFECT_TP_TO_STORE},
	{EFFECT_TP_WAYPOINT}
};

static void OnStartFakeTp()
{
	EEffectType fakeTpType = EFFECT_INVALID;

	do
	{
		EEffectType type = fakeTpTypes.at(g_Random.GetRandomInt(0, fakeTpTypes.size() - 1));

		if (type == EFFECT_TP_MISSION && GetAllMissionBlips().size() == 0)
		{
			continue;
		}

		if (type == EFFECT_TP_WAYPOINT && !HasValidWaypointForTp())
		{
			continue;
		}

		fakeTpType = type;
	} while (fakeTpType == EFFECT_INVALID);

	g_pEffectDispatcher->OverrideEffectName(EFFECT_TP_FAKE, fakeTpType);

	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Hooks::EnableScriptThreadBlock();

	SET_ENTITY_INVINCIBLE(playerPed, true);

	int oldWantedLevel = GET_PLAYER_WANTED_LEVEL(player);

	switch (fakeTpType)
	{
	case EFFECT_TP_LSAIRPORT:
		OnStartLSIA();
		SET_PLAYER_WANTED_LEVEL(player, 3, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
		break;
	case EFFECT_TP_MAZETOWER:
		OnStartMazeTower();
		break;
	case EFFECT_TP_FORTZANCUDO:
		OnStartFortZancudo();
		SET_PLAYER_WANTED_LEVEL(player, 4, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, 0);
		break;
	case EFFECT_TP_MOUNTCHILLIAD:
		OnStartMountChilliad();
		break;
	case EFFECT_TP_SKYFALL:
		OnStartSkyFall();
		break;
	case EFFECT_TP_MISSION:
		OnStartMission();
		break;
	case EFFECT_TP_RANDOM:
		OnStartRandom();
		break;
	case EFFECT_TP_TO_STORE:
		OnStartStore();
		break;
	case EFFECT_TP_WAYPOINT:
		OnStartWaypoint();
		break;
	}

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(playerPos);

	WAIT(0);

	SET_ENTITY_INVINCIBLE(playerPed, false);
	if (playerVeh)
	{
		SET_ENTITY_INVINCIBLE(playerVeh, false);
	}

	SET_PLAYER_WANTED_LEVEL(player, oldWantedLevel, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, 0);

	Hooks::DisableScriptThreadBlock();
}

static RegisterEffect registerEffectFake(EFFECT_TP_FAKE, OnStartFakeTp, EffectInfo
	{
		.Name = "Fake Teleport",
		.Id = "tp_fake"
	}
);
