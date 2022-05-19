#include <stdafx.h>

#include "Memory/Vehicle.h"

static Vector3 GetPlayerPos()
{
	Ped playerPed = PLAYER_PED_ID();

	return GET_ENTITY_COORDS(playerPed, false);
}

static void OnStartRhino()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("RHINO"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect1(OnStartRhino, EffectInfo
	{
		.Name = "Spawn Rhino",
		.Id = "spawn_rhino",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartAdder()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("ADDER"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect2(OnStartAdder, EffectInfo
	{
		.Name = "Spawn Adder",
		.Id = "spawn_adder",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartDump()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("DUMP"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect3(OnStartDump, EffectInfo
	{
		.Name = "Spawn Dump",
		.Id = "spawn_dump",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartMonster()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("MONSTER"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect4(OnStartMonster, EffectInfo
	{
		.Name = "Spawn Monster",
		.Id = "spawn_monster",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBMX()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BMX"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect5(OnStartBMX, EffectInfo
	{
		.Name = "Spawn BMX",
		.Id = "spawn_bmx",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartTug()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("TUG"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect6(OnStartTug, EffectInfo
	{
		.Name = "Spawn Tug",
		.Id = "spawn_tug",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartCargoplane()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("CARGOPLANE"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect7(OnStartCargoplane, EffectInfo
	{
		.Name = "Spawn Cargo Plane",
		.Id = "spawn_cargo",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBus()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BUS"), playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect8(OnStartBus, EffectInfo
	{
		.Name = "Spawn Bus",
		.Id = "spawn_bus",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBlimp()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BLIMP"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect9(OnStartBlimp, EffectInfo
	{
		.Name = "Spawn Blimp",
		.Id = "spawn_blimp",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBuzzard()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BUZZARD"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect10(OnStartBuzzard, EffectInfo
	{
		.Name = "Spawn Buzzard",
		.Id = "spawn_buzzard",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartFaggio()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("FAGGIO"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect11(OnStartFaggio, EffectInfo
	{
		.Name = "Spawn Faggio",
		.Id = "spawn_faggio",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRuiner3()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("RUINER3"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect12(OnStartRuiner3, EffectInfo
	{
		.Name = "Spawn Ruined Ruiner",
		.Id = "spawn_ruiner3",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRandom()
{
	Vector3 playerPos				   = GetPlayerPos();

	static std::vector<Hash> vehModels = Memory::GetAllVehModels();

	if (!vehModels.empty())
	{
		Vehicle veh = CreatePoolVehicle(vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)], playerPos.x,
										playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(veh, 0);

		SET_VEHICLE_WHEEL_TYPE(veh, g_Random.GetRandomInt(0, 7));

		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			if (max > 0)
			{
				SET_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, max - 1), g_Random.GetRandomInt(0, 1));
			}

			TOGGLE_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, g_Random.GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(veh, g_Random.GetRandomInt(0, 6));

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255),
										  g_Random.GetRandomInt(0, 255));
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255),
											g_Random.GetRandomInt(0, 255));

		_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255),
										g_Random.GetRandomInt(0, 255));
		for (int i = 0; i < 4; i++)
		{
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
		}

		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, g_Random.GetRandomInt(0, 12));
	}
}

// clang-format off
static RegisterEffect registerEffect13(OnStartRandom, EffectInfo
	{
		.Name = "Spawn Random Vehicle",
		.Id = "spawn_random",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBaletrailer()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("BALETRAILER"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect14(OnStartBaletrailer, EffectInfo
	{
		.Name = "Spawn Bale Trailer",
		.Id = "spawn_baletrailer",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRomero()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle(GET_HASH_KEY("ROMERO"), playerPos.x, playerPos.y, playerPos.z,
					  GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
static RegisterEffect registerEffect15(OnStartRomero, EffectInfo
	{
		.Name = "Where's The Funeral?",
		.Id = "spawn_romero",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartWizardBroom()
{
	static const Hash oppressorHash = GET_HASH_KEY("OPPRESSOR2");
	static const Hash broomHash		= GET_HASH_KEY("prop_tool_broom");
	LoadModel(oppressorHash);
	LoadModel(broomHash);

	Ped player		  = PLAYER_PED_ID();
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

// clang-format off
static RegisterEffect registerEffect(OnStartWizardBroom, nullptr, nullptr, EffectInfo
	{
		.Name = "You're A Wizard, Franklin",
		.Id = "vehs_spawn_wizard_broom",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
