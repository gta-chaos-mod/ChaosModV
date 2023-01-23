#include <stdafx.h>

#include "Components/EffectDispatcher.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

#include "Util/Player.h"

static void OnStartLSIA()
{
	TeleportPlayer(-1388.6f, -3111.61f, 13.94f);
}

// clang-format off
REGISTER_EFFECT(OnStartLSIA, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To LS Airport",
		.Id = "tp_lsairport",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartMazeTower()
{
	TeleportPlayer(-75.7f, -818.62f, 326.16f);
}

// clang-format off
REGISTER_EFFECT(OnStartMazeTower, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Top Of Maze Bank Tower",
		.Id = "tp_mazebanktower",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

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

// clang-format off
REGISTER_EFFECT(OnStartFortZancudo, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Fort Zancudo",
		.Id = "tp_fortzancudo",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

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

// clang-format off
REGISTER_EFFECT(OnStartMountChilliad, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Mount Chiliad",
		.Id = "tp_mountchilliad",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartSkyFall()
{
	TeleportPlayer(935.f, 3800.f, 2300.f);
}

// clang-format off
REGISTER_EFFECT(OnStartSkyFall, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Heaven",
		.Id = "tp_skyfall",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartWaypoint()
{
	Vector3 coords;
	bool found = false, playerBlip = false;
	if (IS_WAYPOINT_ACTIVE())
	{
		coords     = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		found      = true;
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
				found  = true;

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

				z                 = playerPos.z;
			}
		}

		TeleportPlayer(coords.x, coords.y, z);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartWaypoint, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Waypoint",
		.Id = "player_tptowaypoint"
	}
);
// clang-format on

static void OnStartFront()
{
	Ped playerPed  = PLAYER_PED_ID();
	Vector3 newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.f, 50.f, 0.f);
	float groundZ;
	bool useGroundZ = GET_GROUND_Z_FOR_3D_COORD(newPos.x, newPos.y, 1000.f, &groundZ, false, false);

	TeleportPlayer(newPos.x, newPos.y, useGroundZ ? groundZ : newPos.z);
}

// clang-format off
REGISTER_EFFECT(OnStartFront, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport Player A Few Meters",
		.Id = "player_tpfront"
	}
);
// clang-format on

static void OnStartRandom()
{
	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	float x, y, z = playerPos.z, _;
	do
	{
		x = g_Random.GetRandomInt(-3747.f, 4500.f);
		y = g_Random.GetRandomInt(-4400.f, 8022.f);

	} while (TEST_VERTICAL_PROBE_AGAINST_ALL_WATER(x, y, z, 0, &_));

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

// clang-format off
REGISTER_EFFECT(OnStartRandom, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Random Location",
		.Id = "tp_random",
		.EffectGroupType = EEffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartMission()
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
		excludedColors = {};
		break;
	}
	const int possibleBlipIds[] = { 66,  76,  77,  78,  79,  80,  86,  88,  89,  96,  104, 105, 106, 107, 112, 113,
		                            118, 120, 123, 124, 208, 209, 210, 211, 214, 267, 293, 355, 363, 381, 382, 383,
		                            384, 385, 386, 387, 388, 389, 428, 445, 447, 448, 449, 450, 451, 452, 453, 454 };
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
	if (validBlips.size() >= 1)
	{
		Blip randomBlip   = validBlips[g_Random.GetRandomInt(0, validBlips.size() - 1)];
		Vector3 blipCoord = GET_BLIP_COORDS(randomBlip);
		TeleportPlayer(blipCoord.x, blipCoord.y, blipCoord.z, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartMission, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport To Random Mission",
		.Id = "tp_mission"
	}
);
// clang-format on

struct FakeTeleportInfo
{
	std::string_view type;
	Vector3 playerPos;
	Vector3 vehiclePos;
};

static const std::vector<FakeTeleportInfo> tpLocations = {
	{ "tp_lsairport", { -1388.6f, -3111.61f, 13.94f } },                                    // LSIA
	{ "tp_mazebanktower", { -75.7f, -818.62f, 326.16f } },                                  // Maze Tower
	{ "tp_fortzancudo", { -2360.3f, 3244.83f, 92.9f }, { -2267.89f, 3121.04f, 32.5f } },    // Fort Zancudo
	{ "tp_mountchilliad", { 501.77f, 5604.85f, 797.91f }, { 503.33f, 5531.91f, 777.45f } }, // Mount Chilliad
	{ "tp_skyfall", { 935.f, 3800.f, 2300.f } }                                             // Heaven
};

static int GetFakeWantedLevel(std::string_view effect)
{
	if (effect == "tp_lsairport")
	{
		return 3;
	}
	else if (effect == "tp_fortzancudo")
	{
		return 4;
	}

	return 0;
}

static void OnStartFakeTp()
{
	FakeTeleportInfo selectedLocationInfo = tpLocations.at(g_Random.GetRandomInt(0, tpLocations.size() - 1));
	auto overrideId                       = selectedLocationInfo.type;
	GetComponent<EffectDispatcher>()->OverrideEffectNameId("tp_fake", overrideId);

	Player player     = PLAYER_ID();
	Ped playerPed     = PLAYER_PED_ID();
	Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Hooks::EnableScriptThreadBlock();

	SET_ENTITY_INVINCIBLE(playerPed, true);
	Vector3 destinationPos = selectedLocationInfo.playerPos;
	if (playerVeh)
	{
		if (!selectedLocationInfo.vehiclePos.IsDefault())
		{
			destinationPos = selectedLocationInfo.vehiclePos;
		}
		SET_ENTITY_INVINCIBLE(playerVeh, true);
	}

	int currentWanted = GET_PLAYER_WANTED_LEVEL(player);
	int wanted        = GetFakeWantedLevel(selectedLocationInfo.type);
	if (wanted == 0 || wanted < currentWanted)
	{
		wanted = currentWanted;
	}

	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	SET_MAX_WANTED_LEVEL(0);
	SET_FAKE_WANTED_LEVEL(wanted);
	TeleportPlayer(destinationPos);

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(playerPos);

	WAIT(0);

	SET_ENTITY_INVINCIBLE(playerPed, false);
	if (playerVeh)
	{
		SET_ENTITY_INVINCIBLE(playerVeh, false);
	}

	SET_FAKE_WANTED_LEVEL(0);
	SET_MAX_WANTED_LEVEL(5);
	SET_PLAYER_WANTED_LEVEL(player, currentWanted, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);

	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStartFakeTp, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Teleport",
		.Id = "tp_fake"
	}
);
// clang-format on

static void OnStartFakeFakeTp()
{
	FakeTeleportInfo selectedLocationInfo = tpLocations.at(g_Random.GetRandomInt(0, tpLocations.size() - 1));
	auto overrideId                       = selectedLocationInfo.type;
	GetComponent<EffectDispatcher>()->OverrideEffectNameId("tp_fakex2", overrideId);

	Player player     = PLAYER_ID();
	Ped playerPed     = PLAYER_PED_ID();
	Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Hooks::EnableScriptThreadBlock();

	SET_ENTITY_INVINCIBLE(playerPed, true);
	Vector3 destinationPos = selectedLocationInfo.playerPos;
	if (playerVeh)
	{
		if (!selectedLocationInfo.vehiclePos.IsDefault())
		{
			destinationPos = selectedLocationInfo.vehiclePos;
		}
		SET_ENTITY_INVINCIBLE(playerVeh, true);
	}

	int currentWanted = GET_PLAYER_WANTED_LEVEL(player);
	int wanted        = GetFakeWantedLevel(selectedLocationInfo.type);
	if (wanted == 0 || wanted < currentWanted)
	{
		wanted = currentWanted;
	}

	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	SET_MAX_WANTED_LEVEL(0);
	SET_FAKE_WANTED_LEVEL(wanted);
	TeleportPlayer(destinationPos);

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(playerPos);

	WAIT(0);

	SET_ENTITY_INVINCIBLE(playerPed, false);
	if (playerVeh)
	{
		SET_ENTITY_INVINCIBLE(playerVeh, false);
	}

	SET_FAKE_WANTED_LEVEL(0);
	SET_MAX_WANTED_LEVEL(5);
	SET_PLAYER_WANTED_LEVEL(player, currentWanted, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);

	GetComponent<EffectDispatcher>()->OverrideEffectNameId("tp_fakex2", "tp_fake");

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(destinationPos);
	
	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStartFakeFakeTp, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Fake Teleport",
		.Id = "tp_fakex2"
	}
);
