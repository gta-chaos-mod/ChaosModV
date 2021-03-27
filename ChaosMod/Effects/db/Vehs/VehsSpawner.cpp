#include <stdafx.h>

static Vector3 GetPlayerPos()
{
	Ped playerPed = PLAYER_PED_ID();

	return GET_ENTITY_COORDS(playerPed, false);
}

static void OnStartRhino()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("RHINO"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect1(EFFECT_SPAWN_TANK, OnStartRhino, EffectInfo
	{
		.Name = "Spawn Rhino",
		.Id = "spawn_rhino",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartAdder()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("ADDER"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect2(EFFECT_SPAWN_ADDER, OnStartAdder, EffectInfo
	{
		.Name = "Spawn Adder",
		.Id = "spawn_adder",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartDump()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("DUMP"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect3(EFFECT_SPAWN_DUMP, OnStartDump, EffectInfo
	{
		.Name = "Spawn Dump",
		.Id = "spawn_dump",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartMonster()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("MONSTER"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect4(EFFECT_SPAWN_MONSTER, OnStartMonster, EffectInfo
	{
		.Name = "Spawn Monster",
		.Id = "spawn_monster",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartBMX()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BMX"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect5(EFFECT_SPAWN_BMX, OnStartBMX, EffectInfo
	{
		.Name = "Spawn BMX",
		.Id = "spawn_bmx",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartTug()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("TUG"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect6(EFFECT_SPAWN_TUG, OnStartTug, EffectInfo
	{
		.Name = "Spawn Tug",
		.Id = "spawn_tug",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartCargoplane()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("CARGOPLANE"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect7(EFFECT_SPAWN_CARGO, OnStartCargoplane, EffectInfo
	{
		.Name = "Spawn Cargo Plane",
		.Id = "spawn_cargo",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartBus()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BUS"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect8(EFFECT_SPAWN_BUS, OnStartBus, EffectInfo
	{
		.Name = "Spawn Bus",
		.Id = "spawn_bus",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartBlimp()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BLIMP"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect9(EFFECT_SPAWN_BLIMP, OnStartBlimp, EffectInfo
	{
		.Name = "Spawn Blimp",
		.Id = "spawn_blimp",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartBuzzard()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BUZZARD"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect10(EFFECT_SPAWN_BUZZARD, OnStartBuzzard, EffectInfo
	{
		.Name = "Spawn Buzzard",
		.Id = "spawn_buzzard",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartFaggio()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("FAGGIO"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect11(EFFECT_SPAWN_FAGGIO, OnStartFaggio, EffectInfo
	{
		.Name = "Spawn Faggio",
		.Id = "spawn_faggio",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartRuiner3()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("RUINER3"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect12(EFFECT_SPAWN_RUINER3, OnStartRuiner3, EffectInfo
	{
		.Name = "Spawn Ruined Ruiner",
		.Id = "spawn_ruiner3",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartRandom()
{
	Vector3 playerPos = GetPlayerPos();

	static std::vector<Hash> vehModels = Memory::GetAllVehModels();

	if (!vehModels.empty())
	{
		Vehicle veh = CreatePoolVehicle(vehModels[g_random.GetRandomInt(0, vehModels.size() - 1)], playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(veh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			SET_VEHICLE_MOD(veh, i, max > 0 ? g_random.GetRandomInt(0, max - 1) : 0, g_random.GetRandomInt(0, 1));

			TOGGLE_VEHICLE_MOD(veh, i, g_random.GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, g_random.GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(veh, g_random.GetRandomInt(0, 6));
	}
}

static RegisterEffect registerEffect13(EFFECT_SPAWN_RANDOM, OnStartRandom, EffectInfo
	{
		.Name = "Spawn Random Vehicle",
		.Id = "spawn_random",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartBaletrailer()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BALETRAILER"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect14(EFFECT_SPAWN_BALETRAILER, OnStartBaletrailer, EffectInfo
	{
		.Name = "Spawn Bale Trailer",
		.Id = "spawn_baletrailer",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);
static void OnStartRomero()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("ROMERO"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect15(EFFECT_SPAWN_ROMERO, OnStartRomero, EffectInfo
	{
		.Name = "Where's The Funeral?",
		.Id = "spawn_romero",
		.EffectGroupType = EffectGroupType::SPAWN_GENERIC
	}
);