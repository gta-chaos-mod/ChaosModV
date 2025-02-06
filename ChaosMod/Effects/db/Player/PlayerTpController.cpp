#include <stdafx.h>

#include "Effects/EnabledEffects.h"
#include "Effects/Register/RegisterEffect.h"
#include "Effects/db/Player/PlayerBlimpStrats.h"
#include "Effects/db/Player/PlayerRandomStuntJump.h"
#include "Effects/db/Player/PlayerTpToRandomStore.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"
#include "Util/Player.h"

static void OnStartLSIA()
{
	TeleportPlayer(-1388.6f, -3111.61f, 13.94f);
}

// clang-format off
REGISTER_EFFECT(OnStartLSIA, nullptr, nullptr, 
	{
		.Name = "Teleport To LS Airport",
		.Id = "tp_lsairport",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartMazeTower()
{
	TeleportPlayer(-75.7f, -818.62f, 326.16f);
}

// clang-format off
REGISTER_EFFECT(OnStartMazeTower, nullptr, nullptr, 
	{
		.Name = "Teleport To Top Of Maze Bank Tower",
		.Id = "tp_mazebanktower",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartFortZancudo()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
		TeleportPlayer(-2360.3f, 3244.83f, 92.9f);
	else
		TeleportPlayer(-2267.89f, 3121.04f, 32.5f);
}

// clang-format off
REGISTER_EFFECT(OnStartFortZancudo, nullptr, nullptr, 
	{
		.Name = "Teleport To Fort Zancudo",
		.Id = "tp_fortzancudo",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartMountChilliad()
{
	if (!IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
		TeleportPlayer(501.77f, 5604.85f, 797.91f);
	else
		TeleportPlayer(503.33f, 5531.91f, 777.45f);
}

// clang-format off
REGISTER_EFFECT(OnStartMountChilliad, nullptr, nullptr, 
	{
		.Name = "Teleport To Mount Chiliad",
		.Id = "tp_mountchilliad",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

static void OnStartSkyFall()
{
	TeleportPlayer(935.f, 3800.f, 2300.f);
}

// clang-format off
REGISTER_EFFECT(OnStartSkyFall, nullptr, nullptr, 
	{
		.Name = "Teleport To Heaven",
		.Id = "tp_skyfall",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

static bool HasValidWaypointForTp()
{
	if (IS_WAYPOINT_ACTIVE())
		return true;

	for (int i = 0; i < 3; i++)
	{
		Blip blip = GET_FIRST_BLIP_INFO_ID(i);
		if (DOES_BLIP_EXIST(blip))
			return true;
	}

	return false;
}

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
			TeleportPlayer(coords.x, coords.y, coords.z);
		else
			TeleportPlayerFindZ(coords.x, coords.y);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartWaypoint, nullptr, nullptr, 
	{
		.Name = "Teleport To Waypoint",
		.Id = "player_tptowaypoint"
	}
);
// clang-format on

static void OnStartWaypointOpposite()
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
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

		coords            = coords + coords - playerPos;

		TeleportPlayerFindZ(coords.x, coords.y);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartWaypointOpposite, nullptr, nullptr, 
	{
		.Name = "Teleport To The Opposite Side Of Waypoint",
		.Id = "player_tptowaypointopposite"
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
REGISTER_EFFECT(OnStartFront, nullptr, nullptr, 
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
		x = g_Random.GetRandomFloat(-3747.f, 4500.f);
		y = g_Random.GetRandomFloat(-4400.f, 8022.f);

	} while (TEST_VERTICAL_PROBE_AGAINST_ALL_WATER(x, y, z, 0, &_));

	TeleportPlayerFindZ(x, y);
}

// clang-format off
REGISTER_EFFECT(OnStartRandom, nullptr, nullptr, 
	{
		.Name = "Teleport To Random Location",
		.Id = "tp_random",
		.EffectGroupType = EffectGroupType::Teleport
	}
);
// clang-format on

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
				validBlips.push_back(nextBlip);
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
		Blip randomBlip   = validBlips[g_Random.GetRandomInt(0, validBlips.size() - 1)];
		Vector3 blipCoord = GET_BLIP_COORDS(randomBlip);
		TeleportPlayer(blipCoord.x, blipCoord.y, blipCoord.z, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartMission, nullptr, nullptr, 
	{
		.Name = "Teleport To Random Mission",
		.Id = "tp_mission"
	}
);
// clang-format on

static const std::vector<std::string_view> fakeTpTypes = {
	"tp_lsairport",        "tp_mazebanktower",    "tp_fortzancudo",  "tp_mountchilliad",
	"tp_skyfall",          "tp_mission",          "tp_random",       "player_tp_store",
	"player_tptowaypoint", "player_blimp_strats", "player_tp_stunt", "player_tptowaypointopposite"
};

static bool IsValidFakeTpType(std::string_view type)
{
	auto effectIdentifier = EffectIdentifier(std::string(type));
	if (!g_EnabledEffects.contains(effectIdentifier))
		return false;

	if (type == "tp_mission")
		return GetAllMissionBlips().size() > 0;
	else if (type == "player_tptowaypoint")
		return HasValidWaypointForTp();

	return true;
}

static Vector3 PerformFakeTeleport(std::string effectId)
{
	std::string fakeTpType;

	std::vector<std::string> validTpTypes;

	for (auto &type : fakeTpTypes)
		if (IsValidFakeTpType(type))
			validTpTypes.emplace_back(type);

	if (validTpTypes.empty())
		return GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	fakeTpType = validTpTypes.at(g_Random.GetRandomInt(0, validTpTypes.size() - 1));

	CurrentEffect::OverrideEffectNameFromId(fakeTpType);

	Player player     = PLAYER_ID();
	Ped playerPed     = PLAYER_PED_ID();
	Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Hooks::EnableScriptThreadBlock();

	SET_ENTITY_INVINCIBLE(playerPed, true);
	if (playerVeh)
		SET_ENTITY_INVINCIBLE(playerVeh, true);

	int oldWantedLevel = GET_PLAYER_WANTED_LEVEL(player);

	switch (GET_HASH_KEY(fakeTpType.data()))
	{
	case "tp_lsairport"_hash:
		SET_PLAYER_WANTED_LEVEL(player, 3, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		OnStartLSIA();
		break;
	case "tp_mazebanktower"_hash:
		OnStartMazeTower();
		break;
	case "tp_fortzancudo"_hash:
		SET_PLAYER_WANTED_LEVEL(player, 4, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		OnStartFortZancudo();
		break;
	case "tp_mountchilliad"_hash:
		OnStartMountChilliad();
		break;
	case "tp_skyfall"_hash:
		OnStartSkyFall();
		break;
	case "tp_mission"_hash:
		OnStartMission();
		break;
	case "tp_random"_hash:
		OnStartRandom();
		break;
	case "player_tp_store"_hash:
		OnStartTpRandomStore();
		break;
	case "player_tptowaypoint"_hash:
		OnStartWaypoint();
		break;
	case "player_blimp_strats"_hash:
		OnStartBlimpStrats(false);
		break;
	case "player_tp_stunt"_hash:
		OnStartMakeRandomStuntJump();

		// Wait for stunt jump to start
		WAIT(1000);

		while (IS_STUNT_JUMP_IN_PROGRESS())
			WAIT(0);

		break;
	case "player_tptowaypointopposite"_hash:
		OnStartWaypointOpposite();
		break;
	}

	WAIT(g_Random.GetRandomInt(3500, 6000));

	Vector3 destinationCoords = GET_ENTITY_COORDS(playerPed, false);

	TeleportPlayer(playerPos);

	WAIT(0);

	SET_ENTITY_INVINCIBLE(playerPed, false);
	if (playerVeh)
		SET_ENTITY_INVINCIBLE(playerVeh, false);

	SET_PLAYER_WANTED_LEVEL(player, oldWantedLevel, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, 0);

	return destinationCoords;
}

static void OnStartFakeTp()
{
	Hooks::EnableScriptThreadBlock();

	PerformFakeTeleport("tp_fake");

	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStartFakeTp, nullptr, nullptr, 
	{
		.Name = "Fake Teleport",
		.Id = "tp_fake",
		.HideRealNameOnStart = true
	}
);
// clang-format on

static void OnStartFakeFakeTp()
{
	Hooks::EnableScriptThreadBlock();

	Vector3 destinationPos = PerformFakeTeleport("tp_fakex2");

	CurrentEffect::OverrideEffectNameFromId("tp_fake");

	WAIT(g_Random.GetRandomInt(3500, 6000));

	TeleportPlayer(destinationPos);

	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStartFakeFakeTp, nullptr, nullptr, 
	{
		.Name = "Fake Fake Teleport",
		.Id = "tp_fakex2",
		.HideRealNameOnStart = true
	}
);