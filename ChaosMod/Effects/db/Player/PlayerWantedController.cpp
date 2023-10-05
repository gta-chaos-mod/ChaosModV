#include <math.h>
#include <stdafx.h>

#include "Components/EffectDispatcher.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStartFive()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartFive, nullptr, nullptr, EffectInfo
	{
		.Name = "5 Wanted Stars",
		.Id = "player_5stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartPlusTwo()
{
	Ped player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartPlusTwo, nullptr, nullptr, EffectInfo
	{
		.Name = "+2 Wanted Stars",
		.Id = "player_plus2stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnTickNeverWanted()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickNeverWanted, EffectInfo
	{
		.Name = "Never Wanted",
		.Id = "player_neverwanted",
		.IsTimed = true,
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartThree()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 3, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartThree, nullptr, nullptr, EffectInfo
	{
		.Name = "3 Wanted Stars",
		.Id = "player_3stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartOne()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 1, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartOne, nullptr, nullptr, EffectInfo
	{
		.Name = "1 Wanted Star",
		.Id = "player_1star",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

enum WantedLevelType
{
	SET,
	ADD
};

struct FakeWantedInfo
{
	std::string Name;
	WantedLevelType WantedType;
	int Stars;
};

static std::vector<FakeWantedInfo> ms_FakeWantedLevels = {
	{ "5 Wanted Stars", SET, 5 },
	{ "3 Wanted Stars", SET, 3 },
	{ "1 Wanted Star", SET, 1 },
	{ "+2 Wanted Stars", ADD, 2 },
};

int lastLevel;
int lastLevelf;

static void OnStartFake()
{
	FakeWantedInfo selectedInfo  = ms_FakeWantedLevels.at(g_Random.GetRandomInt(0, ms_FakeWantedLevels.size() - 1));
	WantedLevelType selectedType = selectedInfo.WantedType;

	Hooks::EnableScriptThreadBlock();

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->OverrideEffectName("player_fakestars", selectedInfo.Name);
	}

	Player player = PLAYER_ID();
	lastLevel     = GET_PLAYER_WANTED_LEVEL(player);
	lastLevelf    = GET_FAKE_WANTED_LEVEL();

	switch (selectedType)
	{
	case SET:
	{
		SET_FAKE_WANTED_LEVEL(selectedInfo.Stars);
		break;
	}
	case ADD:
	{
		int wl = [&]()
		{
			int l = selectedInfo.Stars + (lastLevelf == 0 ? lastLevel : lastLevelf);
			if (l > 5)
				l = 5;
			return l;
		}();
		SET_FAKE_WANTED_LEVEL(wl);
		break;
	}
	}

	WAIT(g_Random.GetRandomInt(2500, 6500));

	Hooks::DisableScriptThreadBlock();
	SET_FAKE_WANTED_LEVEL(lastLevelf);
}

// clang-format off
REGISTER_EFFECT(OnStartFake, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Wanted Level",
		.Id = "player_fakestars",
		.HideRealNameOnStart = true,
		.IncompatibleWith = { "player_neverwanted" },
        .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

//
// Effect by Veloscocity
// and moneywasted too i guess but he didnt really do anything
//

struct Squad
{
	Vehicle veh;
	mutable std::vector<Ped> soldiers;
};

#define PI 3.1415927f
static const Hash marineModel  = "s_m_y_marine_01"_hash;
static const Hash marineModel2 = "s_m_y_marine_02"_hash;
static const Hash marineModel3 = "s_m_y_marine_03"_hash;
static const Hash marineModel4 = "s_m_m_marine_01"_hash;
static const Hash armyGroup    = "army"_hash;
static const Hash playerGroup  = "PLAYER"_hash;

static Ped AddSoldier(Vehicle veh, int seat, bool driveby = true)
{
	LoadModel(marineModel);
	Ped soldier = CreatePoolPedInsideVehicle(veh, 29, marineModel, seat);
	SET_MODEL_AS_NO_LONGER_NEEDED(marineModel);
	SET_PED_RELATIONSHIP_GROUP_HASH(soldier, armyGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(soldier, true);
	SET_PED_CONFIG_FLAG(soldier, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(soldier, 2, driveby);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 46, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 22, true);

	GIVE_WEAPON_TO_PED(soldier, "WEAPON_PISTOL_MK2"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(soldier, "WEAPON_CARBINERIFLE_MK2"_hash, 9999, true, true);
	SET_PED_ACCURACY(soldier, 40);

	return soldier;
}

static Ped AddVeteranSoldier(Vehicle veh, int seat, bool driveby = true)
{
	Ped soldier = CreatePoolPedInsideVehicle(veh, 29, marineModel4, seat);
	SET_PED_RELATIONSHIP_GROUP_HASH(soldier, armyGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(soldier, true);
	SET_PED_CONFIG_FLAG(soldier, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(soldier, 2, driveby);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 46, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 22, true);
	SET_PED_SUFFERS_CRITICAL_HITS(soldier, false);

	GIVE_WEAPON_TO_PED(soldier, "WEAPON_PISTOL_MK2"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(soldier, "WEAPON_CARBINERIFLE_MK2"_hash, 9999, true, true);
	GIVE_WEAPON_COMPONENT_TO_PED(soldier, "WEAPON_PISTOL_MK2"_hash, "COMPONENT_PISTOL_MK2_CLIP_FMJ"_hash);
	GIVE_WEAPON_COMPONENT_TO_PED(soldier, "WEAPON_CARBINERIFLE_MK2"_hash, "COMPONENT_CARBINERIFLE_MK2_CLIP_FMJ"_hash);
	SET_PED_ACCURACY(soldier, 55);
	return soldier;
}

static Ped AddMachineGunner(Vehicle veh, int seat, bool barrage)
{
	Ped soldier = CreatePoolPedInsideVehicle(veh, 29, marineModel3, seat);
	SET_PED_RELATIONSHIP_GROUP_HASH(soldier, armyGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(soldier, true);
	SET_PED_CONFIG_FLAG(soldier, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(soldier, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 46, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 22, true);
	SET_PED_SUFFERS_CRITICAL_HITS(soldier, false);

	GIVE_WEAPON_TO_PED(soldier, "WEAPON_PISTOL_MK2"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(soldier, "WEAPON_COMBATMG_MK2"_hash, 9999, true, true);
	SET_PED_ACCURACY(soldier, 35);
	SET_PED_FIRING_PATTERN(soldier, 0xC6EE6B4C);
	if (barrage)
	{
		SET_PED_COMBAT_ATTRIBUTES(soldier, 3, false);
	}
	else
	{
		SET_PED_COMBAT_ATTRIBUTES(soldier, 2, false);
	}

	return soldier;
}

static Ped AddRocketeer(Vehicle veh, int seat, bool driveby = false)
{
	Ped soldier = CreatePoolPedInsideVehicle(veh, 29, marineModel2, seat);
	SET_PED_RELATIONSHIP_GROUP_HASH(soldier, armyGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(soldier, true);
	SET_PED_CONFIG_FLAG(soldier, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(soldier, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 46, true);

	// GIVE_WEAPON_TO_PED(soldier, "WEAPON_PISTOL_MK2"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(soldier, "WEAPON_RPG"_hash, 9999, true, true);
	SET_PED_ACCURACY(soldier, 67);
	SET_PED_MOVE_RATE_OVERRIDE(soldier, 1.3f);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 2, driveby);
	SET_PED_COMBAT_ATTRIBUTES(soldier, 13, true);
	return soldier;
}

// mode == 0: car
// mode == 1: boat
// mode == 2: helicopter
static Vehicle SpawnPersuingVehicle(Hash modelHash, float cheatPower, int mode)
{
	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Vector3 playerVel = GET_ENTITY_VELOCITY(playerPed) * 4;
	float aov         = (ATAN(2 / GET_GAMEPLAY_CAM_FOV()) / 2) * 57.29578f;
	Vector3 newPos;
	float camHeading = GET_GAMEPLAY_CAM_ROT(2).z;
	float newHeading;
	int attempts = 0;

	while (true)
	{
		float heading = g_Random.GetRandomFloat(0.f, 360.f);
		float x2      = sin((360 - heading) * PI / 180);
		float y2      = cos((360 - heading) * PI / 180);
		int distance  = g_Random.GetRandomInt(80, 180);
		newPos = Vector3(playerPos.x + playerVel.x + (x2 * distance), playerPos.y + playerVel.y + (y2 * distance),
		                 playerPos.z);
		if (mode == 0)
		{
			if (!GET_NTH_CLOSEST_VEHICLE_NODE_FAVOUR_DIRECTION(newPos.x, newPos.y, newPos.z, playerPos.x, playerPos.y,
			                                                   playerPos.z, 1, &newPos, &newHeading,
			                                                   attempts > 2 ? 1 : 0, 3.0, 0))
			{
				continue;
			}
		}
		else if (mode == 1)
		{
			if (!GET_NTH_CLOSEST_VEHICLE_NODE_FAVOUR_DIRECTION(newPos.x, newPos.y, newPos.z, playerPos.x, playerPos.y,
			                                                   playerPos.z, 1, &newPos, &newHeading, 3, 3.0, 0))
			{
				continue;
			}
		}
		else if (mode == 2)
		{
			newPos.z += 50;
			newHeading = 360 - heading;
		}
		Vector3 dis = newPos - GET_GAMEPLAY_CAM_COORD();
		float angle = fmod(360 - (atan2(dis.x, dis.y) * 57.29578), 360.f);
		camHeading  = fmod(360 + camHeading, 360.f);
		if ((dis.Length() < 500 || attempts > 4)
		    && (dis.Length() >= 300 || (abs(angle - camHeading) > aov && 360 - abs(angle - camHeading) > aov)))
		{
			break;
		}
		attempts++;
	}

	Vehicle veh = CreatePoolVehicle(modelHash, newPos.x, newPos.y, newPos.z + (mode == 0), newHeading);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	if (mode == 0)
	{
		SET_VEHICLE_ON_GROUND_PROPERLY(veh, 5);
	}
	SET_VEHICLE_FORWARD_SPEED(veh, GET_VEHICLE_MODEL_ESTIMATED_MAX_SPEED(modelHash) / 2);
	SET_VEHICLE_CHEAT_POWER_INCREASE(veh, cheatPower); // Make it easier to catch up
	return veh;
}

static Squad SpawnCrusader(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("crusader"_hash, 4, 0);

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 0));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 0));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, -1));
		squad.soldiers.push_back(AddSoldier(squad.veh, 0));
	}
	if (g_Random.GetRandomInt(0, 2) == 0)
	{
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 1, false));
	}
	if (g_Random.GetRandomInt(0, 4) == 0)
	{
		squad.soldiers.push_back(AddRocketeer(squad.veh, 2));
	}
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}

	return squad;
}

static Squad SpawnBarrage(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("barrage"_hash, 6, 0);

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 0));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 0));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, -1));
		squad.soldiers.push_back(AddSoldier(squad.veh, 0));
	}
	int ran = g_Random.GetRandomInt(0, 2);
	if (ran != 0)
	{
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 1, true));
		if (ran != 1)
		{
			squad.soldiers.push_back(AddMachineGunner(squad.veh, 2, true));
		}
	}
	else
	{
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 2, true));
	}
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}

	return squad;
}

static Squad SpawnBrickade(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("brickade"_hash, 4, 0);
	SET_VEHICLE_COLOURS(squad.veh, 110, 15);

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 0));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 0));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, -1));
		squad.soldiers.push_back(AddSoldier(squad.veh, 0));
	}

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 1));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 3));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 3));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, 1));
		squad.soldiers.push_back(AddSoldier(squad.veh, 3));
	}

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 2));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 4));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 4));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, 2));
		squad.soldiers.push_back(AddSoldier(squad.veh, 4));
	}
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}

	return squad;
}

static Squad SpawnBuzzard(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("buzzard2"_hash, 4, 2);

	squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1, false));

	switch(g_Random.GetRandomInt(0, 2))
	{
	case 0:
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 1, true));
		break;
	case 1:
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 1));
		break;
	default:
		squad.soldiers.push_back(AddSoldier(squad.veh, 1));
		break;
	}

	switch(g_Random.GetRandomInt(0, 4))
	{
	case 0:
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 2, true));
		break;
	case 1:
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 2));
		break;
	default:
		squad.soldiers.push_back(AddSoldier(squad.veh, 2));
		break;
	}
	
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}

	return squad;
}

static Squad SpawnTank(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("rhino"_hash, 4, 0);

	Ped ped = AddRocketeer(squad.veh, -1);
	TASK_COMBAT_HATED_TARGETS_AROUND_PED(ped, 600, 0);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	squad.soldiers.push_back(ped);

	return squad;
}

static Squad SpawnDinghy(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("dinghy5"_hash, 4, 1);

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1, false));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 0, false));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 0, false));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, -1, false));
		squad.soldiers.push_back(AddSoldier(squad.veh, 0, false));
	}
	squad.soldiers.push_back(AddMachineGunner(squad.veh, 3, true));
	
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}


	return squad;
}

static Squad SpawnPatrolBoat(void)
{
	Squad squad;
	Ped playerPed = PLAYER_PED_ID();
	squad.veh     = SpawnPersuingVehicle("patrolboat"_hash, 4, 1);

	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddVeteranSoldier(squad.veh, -1, false));
		int r = g_Random.GetRandomInt(0, 2);
		if (r == 0)
		{
			squad.soldiers.push_back(AddVeteranSoldier(squad.veh, 0, false));
		}
		else if (r == 1)
		{
			squad.soldiers.push_back(AddSoldier(squad.veh, 0, false));
		}
	}
	else
	{
		squad.soldiers.push_back(AddSoldier(squad.veh, -1, false));
		squad.soldiers.push_back(AddSoldier(squad.veh, 0, false));
	}
	squad.soldiers.push_back(AddMachineGunner(squad.veh, 1, true));
	if (g_Random.GetRandomInt(0, 1) == 0)
	{
		squad.soldiers.push_back(AddMachineGunner(squad.veh, 2, true));
	}
	for (auto s : squad.soldiers)
	{
		TASK_COMBAT_HATED_TARGETS_AROUND_PED(s, 600, 0);
		SET_PED_HEARING_RANGE(s, 9999.f);
	}

	return squad;
}

static std::vector<Squad> squads;
static DWORD64 lastTick = 0;

static void ClearSquad(Squad squad)
{
	for (auto p : squad.soldiers)
	{
		SET_PED_AS_NO_LONGER_NEEDED(&p);
	}
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&squad.veh);
}

static void ClearSquads(void)
{
	for (auto s : squads)
	{
		ClearSquad(s);
	}
	squads.clear();
}

static void OnStartSix(void)
{
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, armyGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, armyGroup);

	Player player = PLAYER_ID();

	lastLevelf    = GET_FAKE_WANTED_LEVEL();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	SET_FAKE_WANTED_LEVEL(6);
	lastTick = GET_GAME_TIMER();

	squads.push_back(SpawnCrusader());
	squads.push_back(SpawnBarrage());
	squads.push_back(SpawnBuzzard());
}

static void OnTickSix(void)
{
	auto curTick  = GET_GAME_TIMER();
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_DEAD_OR_DYING(playerPed, 1) && squads.size())
	{
		ClearSquads();
	}
	else if (curTick > lastTick + 4000)
	{
		lastTick += 4000;
		Player player = PLAYER_ID();
		int level     = GET_PLAYER_WANTED_LEVEL(player);
		int levelf    = GET_FAKE_WANTED_LEVEL();

		if (level != 5)
		{
			SET_PLAYER_WANTED_LEVEL(player, 5, false);
			SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		}
		if (levelf != 6)
		{
			lastLevelf = levelf;
			SET_FAKE_WANTED_LEVEL(6);
		}
		REPORT_POLICE_SPOTTED_PLAYER(player);

		if (squads.size() != 0)
		{
			Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false) + (GET_ENTITY_VELOCITY(playerPed) * 4);
			int s             = 0;
			int i             = 0;
			std::vector<Squad> emptySquads;

			while (i < squads.size())
			{
				Squad &squad = squads[i];
				float distance = GET_ENTITY_COORDS(squad.veh, false).DistanceTo(playerPos);
				int j = 0;
				while(j < squad.soldiers.size())
				{
					Ped ped = squad.soldiers[j];
					if (!DOES_ENTITY_EXIST(ped) || IS_PED_DEAD_OR_DYING(ped, false) 
					 || GET_ENTITY_COORDS(ped, false).DistanceTo(playerPos) > 450.f)
					{
						SET_PED_AS_NO_LONGER_NEEDED(&ped);
						squad.soldiers.erase(squad.soldiers.begin() + j);
					}
					else
					{
						j++;
					}
				}

				if(squad.soldiers.empty())
				{
					SET_VEHICLE_AS_NO_LONGER_NEEDED(&squad.veh);
					squads.erase(squads.begin() + i);
				}
				else
				{
					i++;
				}
			}
		}

		if (squads.size() < 4)
		{
			int waterCount = 0;
			Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
			float waterZ;

			for(int i = 0; i < 3; i++)
			{
				GET_WATER_HEIGHT(playerPos.x + g_Random.GetRandomFloat(-100.f, 100.f), playerPos.y + g_Random.GetRandomFloat(-100.f, 100.f), playerPos.z + 100.f, &waterZ);
				waterCount += (waterZ > 0);
			}

			if(waterCount < 2)
			{
				switch (g_Random.GetRandomInt(0, 5))
				{
				case 0:
					squads.push_back(SpawnTank());
					break;
				case 1:
					squads.push_back(SpawnBuzzard());
					break;
				case 2:
					squads.push_back(SpawnBarrage());
					break;
				case 3:
					squads.push_back(SpawnBrickade());
					break;
				default:
					squads.push_back(SpawnCrusader());
					break;
				}
			}
			else
			{
				int r = g_Random.GetRandomInt(0, 5);
				
				if (r == 0)
				{
					squads.push_back(SpawnPatrolBoat());
				}
				else if (r < 3)
				{
					squads.push_back(SpawnBuzzard());
				}
				else
				{
					squads.push_back(SpawnDinghy());
				}
			}
		}
	}
}

static void OnStopSix(void)
{
	Player player = PLAYER_ID();
	ClearSquads();
	SET_RELATIONSHIP_BETWEEN_GROUPS(3, armyGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(3, playerGroup, armyGroup);
	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	if (GET_FAKE_WANTED_LEVEL() == 6)
	{
		SET_FAKE_WANTED_LEVEL(lastLevelf);
	}
}

// clang-format off
REGISTER_EFFECT(OnStartSix, OnStopSix, OnTickSix, EffectInfo
	{
		.Name = "!6 Wanted Stars",
		.Id = "player_6stars",
		.IsTimed = true,
		.IncompatibleWith = { "player_neverwanted" },
        .EffectGroupType = EffectGroupType::WantedLevel,
	}
);
