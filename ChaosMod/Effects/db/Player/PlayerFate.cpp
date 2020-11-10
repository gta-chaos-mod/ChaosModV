/*
	Effect by Vladimir Gotsin
*/

#include <stdafx.h>

int scaleForm = 0;
int lastTime = 0;
int waitTime = 2000;
Ped playerPed;
float playerHeading = 0;
float heading = 0;
Vector3 playerPos;
Vehicle veh;
int count = 0;
Vector3 coords;
bool found, playerBlip;
int rand_int;
Player player;


void PrintMessage(std::string message);

static void OnStartBadRNGDie()
{
	playerPed = PLAYER_PED_ID();
	rand_int = g_random.GetRandomInt(1, 6);
	std::string message = "Rolling...";

	PrintMessage(message);

	switch (rand_int)
	{
	case 1:
		playerHeading = GET_ENTITY_HEADING(playerPed);
		heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

		playerPos = GET_ENTITY_COORDS(playerPed, false);

		static const Hash bulldozerModel = GET_HASH_KEY("bulldozer");
		LoadModel(bulldozerModel);

		veh = CREATE_VEHICLE(bulldozerModel, playerPos.x, playerPos.y, playerPos.z, heading, true, true, true);

		SET_PED_INTO_VEHICLE(playerPed, veh, -1);

		PrintMessage("You rolled 1! - Spawned The Slowest Vehicle!");
		break;
	case 2:
		REMOVE_ALL_PED_WEAPONS(playerPed, false);

		PrintMessage("You rolled 2! - Removed all weapons!");
		break;
	case 3:
		for (int i = 0; i < 3; i++)
		{
			char statNameFull[32];
			sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

			Hash hash = GET_HASH_KEY(statNameFull);

			STAT_SET_INT(hash, 0, true);
		}

		PrintMessage("You rolled 3! - Removed all money!");
		break;
	case 4:
		TeleportPlayer(0, 7600, 10, false);

		PrintMessage("You rolled 4! - Teleport away!");
		break;
	case 5:
		SET_ENTITY_HEALTH(playerPed, 0, 0);
		SET_PED_ARMOUR(playerPed, 0);

		PrintMessage("You rolled 5! - Kill Player");
		break;
	case 6:
		player = PLAYER_ID();
		SET_PLAYER_WANTED_LEVEL(player, 5, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);

		PrintMessage("You rolled 6! - 5 stars");
		break;
	default:
		break;
	}
}

static RegisterEffect registerEffect(EFFECT_BAD_RNG_DIE, OnStartBadRNGDie, nullptr, nullptr);

static void OnStartGoodRNGDie()
{
	playerPed = PLAYER_PED_ID();
	rand_int = g_random.GetRandomInt(1, 6);

	std::string message = "Rolling...";

	PrintMessage(message);

	std::array<std::string, 94> weaponNames = {
			"WEAPON_DAGGER", "WEAPON_BAT", "WEAPON_BOTTLE", "WEAPON_CROWBAR", "WEAPON_FLASHLIGHT", "WEAPON_HAZARDCAN",
			"WEAPON_GOLFCLUB", "WEAPON_HAMMER", "WEAPON_HATCHET", "WEAPON_KNUCKLE", "WEAPON_KNIFE", "WEAPON_MACHETE",
			"WEAPON_SWITCHBLADE", "WEAPON_NIGHTSTICK", "WEAPON_WRENCH", "WEAPON_BATTLEAXE", "WEAPON_POOLCUE",
			"WEAPON_STONE_HATCHET", "WEAPON_PISTOL", "WEAPON_PISTOL_MK2", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL",
			"WEAPON_STUNGUN", "WEAPON_PISTOL50", "WEAPON_SNSPISTOL", "WEAPON_SNSPISTOL_MK2", "WEAPON_HEAVYPISTOL",
			"WEAPON_VINTAGEPISTOL", "WEAPON_FLAREGUN", "WEAPON_MARKSMANPISTOL", "WEAPON_REVOLVER", "WEAPON_REVOLVER_MK2",
			"WEAPON_DOUBLEACTION", "WEAPON_RAYPISTOL", "WEAPON_CERAMICPISTOL", "WEAPON_NAVYREVOLVER", "WEAPON_MICROSMG",
			"WEAPON_SMG", "WEAPON_SMG_MK2", "WEAPON_ASSAULTSMG", "WEAPON_COMBATPDW", "WEAPON_MACHINEPISTOL",
			"WEAPON_MINISMG", "WEAPON_RAYCARBINE", "WEAPON_PUMPSHOTGUN", "WEAPON_PUMPSHOTGUN_MK2", "WEAPON_SAWNOFFSHOTGUN",
			"WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN", "WEAPON_MUSKET", "WEAPON_HEAVYSHOTGUN", "WEAPON_DBSHOTGUN",
			"WEAPON_AUTOSHOTGUN", "WEAPON_ASSAULTRIFLE", "WEAPON_ASSAULTRIFLE_MK2", "WEAPON_CARBINERIFLE",
			"WEAPON_CARBINERIFLE_MK2", "WEAPON_ADVANCEDRIFLE", "WEAPON_SPECIALCARBINE", "WEAPON_SPECIALCARBINE_MK2",
			"WEAPON_BULLPUPRIFLE", "WEAPON_BULLPUPRIFLE_MK2", "WEAPON_COMPACTRIFLE", "WEAPON_MG", "WEAPON_COMBATMG",
			"WEAPON_COMBATMG_MK2", "WEAPON_GUSENBERG", "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_HEAVYSNIPER_MK2",
			"WEAPON_MARKSMANRIFLE", "WEAPON_MARKSMANRIFLE_MK2", "WEAPON_RPG", "WEAPON_GRENADELAUNCHER", "WEAPON_GRENADELAUNCHER_SMOKE",
			"WEAPON_MINIGUN", "WEAPON_FIREWORK", "WEAPON_RAILGUN", "WEAPON_HOMINGLAUNCHER", "WEAPON_COMPACTLAUNCHER",
			"WEAPON_RAYMINIGUN", "WEAPON_GRENADE", "WEAPON_BZGAS", "WEAPON_MOLOTOV", "WEAPON_STICKYBOMB", "WEAPON_PROXMINE",
			"WEAPON_SNOWBALL", "WEAPON_PIPEBOMB", "WEAPON_BALL", "WEAPON_SMOKEGRENADE", "WEAPON_FLARE", "WEAPON_PETROLCAN",
			"GADGET_PARACHUTE", "WEAPON_FIREEXTINGUISHER"
	};

	switch (rand_int)
	{
	case 1:
		playerHeading = GET_ENTITY_HEADING(playerPed);
		heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

		playerPos = GET_ENTITY_COORDS(playerPed, false);

		static const Hash pariahModel = GET_HASH_KEY("PARIAH");
		LoadModel(pariahModel);

		veh = CREATE_VEHICLE(pariahModel, playerPos.x, playerPos.y, playerPos.z, heading, true, true, true);

		SET_PED_INTO_VEHICLE(playerPed, veh, -1);

		count = 3;

		for (Vehicle veh : GetAllVehs())
		{
			SET_VEHICLE_MOD_KIT(veh, 0);
			for (int i = 0; i < 50; i++)
			{
				int max = GET_NUM_VEHICLE_MODS(veh, i);
				if (max > 0)
				{
					SET_VEHICLE_MOD(veh, i, max - 1, true);
				}

				TOGGLE_VEHICLE_MOD(veh, i, true);
			}

			SET_VEHICLE_TYRES_CAN_BURST(veh, false);
			SET_VEHICLE_WINDOW_TINT(veh, 1);

			SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255));
			SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255));

			_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255), g_random.GetRandomInt(0, 255));
			for (int i = 0; i < 4; i++)
			{
				_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
			}

			_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, g_random.GetRandomInt(0, 12));

			if (--count == 0)
			{
				count = 3;

				WAIT(0);
			}
		}

		PrintMessage("You rolled 1! - Spawned The Fastest Car!");
		break;
	case 2:
		for (const auto& weapon: weaponNames) {
			const char* cstr = weapon.c_str();
			Hash weaponHash = GET_HASH_KEY(cstr);
			GIVE_WEAPON_TO_PED(playerPed, weaponHash, 9999, false, true);
		}

		PrintMessage("You rolled 2! - Given all weapons!");
		break;
	case 3:
		for (int i = 0; i < 3; i++)
		{
			char statNameFull[32];
			sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

			Hash hash = GET_HASH_KEY(statNameFull);

			STAT_SET_INT(hash, 2147483647, true);
		}

		PrintMessage("You rolled 3! - Given max money!");
		break;
	case 4:
		found = false, playerBlip = false;
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

		PrintMessage("You rolled 4! - Teleport to waypoint!");
		break;
	case 5:
		SET_ENTITY_HEALTH(playerPed, 200, 200);
		SET_PED_ARMOUR(playerPed, 100);

		PrintMessage("You rolled 5! - Max Health and Armour");
		break;
	case 6:
		player = PLAYER_ID();
		SET_PLAYER_WANTED_LEVEL(player, 0, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);

		PrintMessage("You rolled 6! - Remove Wanted Level");
		break;
	default:
		break;
	}
}

static void PrintMessage(std::string message) {
	scaleForm = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
	{
		WAIT(0);
	}
	lastTime = GetTickCount64();
	waitTime = 2000;
	SET_TIME_SCALE(0.1f);
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SHOW_SHARD_RANKUP_MP_MESSAGE");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(message.c_str());
	END_SCALEFORM_MOVIE_METHOD();

	while (GetTickCount64() - lastTime < waitTime)
	{
		WAIT(0);
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);
	}
	SET_TIME_SCALE(1);
	scaleForm = 0;
}

static RegisterEffect registerEffect2(EFFECT_GOOD_RNG_DIE, OnStartGoodRNGDie, nullptr, nullptr);