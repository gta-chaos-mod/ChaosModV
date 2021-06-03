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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
static void OnStartRandom()
{
	Vector3 playerPos = GetPlayerPos();

	static std::vector<Hash> vehModels = Memory::GetAllVehModels();

	if (!vehModels.empty())
	{
		Vehicle veh = CreatePoolVehicle(vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)], playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(veh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			SET_VEHICLE_MOD(veh, i, max > 0 ? g_Random.GetRandomInt(0, max - 1) : 0, g_Random.GetRandomInt(0, 1));

			TOGGLE_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, g_Random.GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(veh, g_Random.GetRandomInt(0, 6));
	}
}

static RegisterEffect registerEffect13(EFFECT_SPAWN_RANDOM, OnStartRandom, EffectInfo
	{
		.Name = "Spawn Random Vehicle",
		.Id = "spawn_random",
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
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
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);

static void OnStartWizardBroom()
{
	static const Hash oppressorHash = GET_HASH_KEY("OPPRESSOR2");
	static const Hash broomHash = GET_HASH_KEY("prop_tool_broom");
	LoadModel(oppressorHash);
	LoadModel(broomHash);

	Ped player = PLAYER_PED_ID();
	Vector3 playerPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, 1, 0);

	Vehicle veh = CreatePoolVehicle(oppressorHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(player));
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	SET_VEHICLE_MOD_KIT(veh, 0);
	for (int i = 0; i < 50; i++)
	{
		int max = GET_NUM_VEHICLE_MODS(veh, i);
		SET_VEHICLE_MOD(veh, i, max > 0 ? max - 1 : 0, false);
	}
	SET_ENTITY_ALPHA(veh, 0, false);
	SET_ENTITY_VISIBLE(veh, false, false);
	
	Object broom = CREATE_OBJECT(broomHash, playerPos.x, playerPos.y + 2, playerPos.z, true, false, false);
	ATTACH_ENTITY_TO_ENTITY(broom, veh, 0, 0, 0, 0.3, -80.0, 0, 0, true, false, false, false, 0, true);
}


static RegisterEffect registerEffect(EFFECT_VEHS_WIZARD_BROOM, OnStartWizardBroom, nullptr, nullptr, EffectInfo
	{
		.Name = "You're A Wizard, Franklin",
		.Id = "vehs_spawn_wizard_broom",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
