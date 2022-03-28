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

static void LoadCayoPerico()
{
	std::vector<const char*> IPLs;
	IPLs = {
	"h4_islandairstrip", "h4_islandairstrip_props", "h4_islandx_mansion", "h4_islandx_mansion_props", "h4_islandx_props", "h4_islandxdock", "h4_islandxdock_props", "h4_islandxdock_props_2", "h4_islandxtower", "h4_islandx_maindock", "h4_islandx_maindock_props", "h4_islandx_maindock_props_2", "h4_IslandX_Mansion_Vault", "h4_islandairstrip_propsb", "h4_beach", "h4_beach_props", "h4_beach_bar_props", "h4_islandx_barrack_props", "h4_islandx_checkpoint", "h4_islandx_checkpoint_props", "h4_islandx_Mansion_Office", "h4_islandx_Mansion_LockUp_01", "h4_islandx_Mansion_LockUp_02", "h4_islandx_Mansion_LockUp_03", "h4_islandairstrip_hangar_props", "h4_IslandX_Mansion_B", "h4_islandairstrip_doorsclosed", "h4_Underwater_Gate_Closed", "h4_mansion_gate_closed", "h4_aa_guns", "h4_IslandX_Mansion_GuardFence", "h4_IslandX_Mansion_Entrance_Fence", "h4_IslandX_Mansion_B_Side_Fence", "h4_IslandX_Mansion_Lights", "h4_islandxcanal_props", "h4_beach_props_party", "h4_islandX_Terrain_props_06_a", "h4_islandX_Terrain_props_06_b", "h4_islandX_Terrain_props_06_c", "h4_islandX_Terrain_props_05_a", "h4_islandX_Terrain_props_05_b", "h4_islandX_Terrain_props_05_c", "h4_islandX_Terrain_props_05_d", "h4_islandX_Terrain_props_05_e", "h4_islandX_Terrain_props_05_f", "H4_islandx_terrain_01", "H4_islandx_terrain_02", "H4_islandx_terrain_03", "H4_islandx_terrain_04", "H4_islandx_terrain_05", "H4_islandx_terrain_06", "h4_ne_ipl_00", "h4_ne_ipl_01", "h4_ne_ipl_02", "h4_ne_ipl_03", "h4_ne_ipl_04", "h4_ne_ipl_05", "h4_ne_ipl_06", "h4_ne_ipl_07", "h4_ne_ipl_08", "h4_ne_ipl_09", "h4_nw_ipl_00", "h4_nw_ipl_01", "h4_nw_ipl_02", "h4_nw_ipl_03", "h4_nw_ipl_04", "h4_nw_ipl_05", "h4_nw_ipl_06", "h4_nw_ipl_07", "h4_nw_ipl_08", "h4_nw_ipl_09", "h4_se_ipl_00", "h4_se_ipl_01", "h4_se_ipl_02", "h4_se_ipl_03", "h4_se_ipl_04", "h4_se_ipl_05", "h4_se_ipl_06", "h4_se_ipl_07", "h4_se_ipl_08", "h4_se_ipl_09", "h4_sw_ipl_00", "h4_sw_ipl_01", "h4_sw_ipl_02", "h4_sw_ipl_03", "h4_sw_ipl_04", "h4_sw_ipl_05", "h4_sw_ipl_06", "h4_sw_ipl_07", "h4_sw_ipl_08", "h4_sw_ipl_09", "h4_islandx_mansion", "h4_islandxtower_veg", "h4_islandx_sea_mines", "h4_islandx", "h4_islandx_barrack_hatch", "h4_islandxdock_water_hatch", "h4_beach_party"
    };

	//Loads the mp maps, unfortuantly this loads all the mp IPLs, including the casino.
	DLC::ON_ENTER_MP();

	while (!IS_IPL_ACTIVE("h4_islandairstrip"))
	{
		WAIT(1000);
		REQUEST_IPL("h4_islandairstrip");
	}

    for (const char* i : IPLs)
    {
        REQUEST_IPL(i);
    }
    WATER::SET_DEEP_OCEAN_SCALER(0);

	//Spawn plane
	Vehicle velum = CREATE_VEHICLE(0x9C429B6A, 4478.60, -4474.47, 5.25, 152.91, 1, 1, 1);
	SET_VEHICLE_STRONG(velum, 1);
}

static void OnStartCayo()
{
    LoadCayoPerico();
    TeleportPlayer(4533.52, -4488.05, 4.0);
}

static RegisterEffect registerEffectcayo(EFFECT_TP_CAYO, OnStartCayo, EffectInfo
    {
        .Name = "Teleport To Cayo Perico",
        .Id = "player_tp_cayo"
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

static RegisterEffect registerEffect8(EFFECT_TP_FRONT, OnStartFront, EffectInfo
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

static RegisterEffect registerEffect9(EFFECT_TP_RANDOM, OnStartRandom, EffectInfo
	{
		.Name = "Teleport To Random Location",
		.Id = "tp_random",
		.EEffectGroupType = EEffectGroupType::Teleport
	}
);

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
		excludedColors = { };
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

static struct FakeTeleportInfo
{
	EEffectType type;
	Vector3 playerPos;
	Vector3 vehiclePos;
};

static const std::vector<FakeTeleportInfo> tpLocations =
{
	{EFFECT_TP_LSAIRPORT, {-1388.6f, -3111.61f, 13.94f}}, // LSIA
	{EFFECT_TP_MAZETOWER, {-75.7f, -818.62f, 326.16f}}, // Maze Tower
	{EFFECT_TP_FORTZANCUDO, {-2360.3f, 3244.83f, 92.9f}, {-2267.89f, 3121.04f, 32.5f}}, // Fort Zancudo
	{EFFECT_TP_MOUNTCHILLIAD, {501.77f, 5604.85f, 797.91f}, {503.33f, 5531.91f, 777.45f}}, // Mount Chilliad
	{EFFECT_TP_SKYFALL, {935.f, 3800.f, 2300.f}} // Heaven
};

static void OnStartFakeTp()
{
	FakeTeleportInfo selectedLocationInfo = tpLocations.at(g_Random.GetRandomInt(0, tpLocations.size() - 1));
	EEffectType overrideName = selectedLocationInfo.type;
	g_pEffectDispatcher->OverrideEffectName(EFFECT_TP_FAKE, overrideName);

	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;
	
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Hooks::EnableScriptThreadBlock();

	SET_ENTITY_INVINCIBLE(playerPed, true);
	Vector3 destinationPos = selectedLocationInfo.playerPos;
	if (playerVeh)
	{
		if (!selectedLocationInfo.vehiclePos.IsDefault()) {
			destinationPos = selectedLocationInfo.vehiclePos;
		}
		SET_ENTITY_INVINCIBLE(playerVeh, true);
	}

	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	SET_MAX_WANTED_LEVEL(0);

	TeleportPlayer(destinationPos);

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(playerPos);

	WAIT(0);

	SET_ENTITY_INVINCIBLE(playerPed, false);
	if (playerVeh)
	{
		SET_ENTITY_INVINCIBLE(playerVeh, false);
	}

	SET_MAX_WANTED_LEVEL(5);

	Hooks::DisableScriptThreadBlock();
}

static RegisterEffect registerEffectFake(EFFECT_TP_FAKE, OnStartFakeTp, EffectInfo
	{
		.Name = "Fake Teleport",
		.Id = "tp_fake"
	}
);
