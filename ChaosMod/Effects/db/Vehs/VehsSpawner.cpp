#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Vehicle.h"

static Vector3 GetPlayerPos()
{
	Ped playerPed = PLAYER_PED_ID();

	return GET_ENTITY_COORDS(playerPed, false);
}

static void OnStartRhino()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("RHINO"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartRhino, nullptr, nullptr, 
	{
		.Name = "Spawn Rhino",
		.Id = "spawn_rhino",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartAdder()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("ADDER"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartAdder, nullptr, nullptr, 
	{
		.Name = "Spawn Adder",
		.Id = "spawn_adder",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartDump()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("DUMP"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartDump, nullptr, nullptr, 
	{
		.Name = "Spawn Dump",
		.Id = "spawn_dump",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartMonster()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("MONSTER"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartMonster, nullptr, nullptr, 
	{
		.Name = "Spawn Monster",
		.Id = "spawn_monster",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBMX()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("BMX"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartBMX, nullptr, nullptr, 
	{
		.Name = "Spawn BMX",
		.Id = "spawn_bmx",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartTug()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("TUG"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartTug, nullptr, nullptr, 
	{
		.Name = "Spawn Tug",
		.Id = "spawn_tug",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartCargoplane()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("CARGOPLANE"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartCargoplane, nullptr, nullptr, 
	{
		.Name = "Spawn Cargo Plane",
		.Id = "spawn_cargo",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBus()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("BUS"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartBus, nullptr, nullptr, 
	{
		.Name = "Spawn Bus",
		.Id = "spawn_bus",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBlimp()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("BLIMP"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartBlimp, nullptr, nullptr, 
	{
		.Name = "Spawn Blimp",
		.Id = "spawn_blimp",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBuzzard()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("BUZZARD"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartBuzzard, nullptr, nullptr, 
	{
		.Name = "Spawn Buzzard",
		.Id = "spawn_buzzard",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartFaggio()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("FAGGIO"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartFaggio, nullptr, nullptr, 
	{
		.Name = "Spawn Faggio",
		.Id = "spawn_faggio",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRuiner3()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("RUINER3"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartRuiner3, nullptr, nullptr, 
	{
		.Name = "Spawn Ruined Ruiner",
		.Id = "spawn_ruiner3",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRandom()
{
	Vector3 playerPos                  = GetPlayerPos();

	static std::vector<Hash> vehModels = Memory::GetAllVehModels();

	if (!vehModels.empty())
	{
		Vehicle veh = CreatePoolVehicle(vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)], playerPos.x,
		                                playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(veh, 0);

		SET_VEHICLE_WHEEL_TYPE(veh, g_Random.GetRandomInt(0, 12));

		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			if (max > 0)
				SET_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, max - 1), g_Random.GetRandomInt(0, 1));

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
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);

		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, g_Random.GetRandomInt(0, 12));
	}
}

// clang-format off
REGISTER_EFFECT(OnStartRandom, nullptr, nullptr, 
	{
		.Name = "Spawn Random Vehicle",
		.Id = "spawn_random",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartBaletrailer()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("BALETRAILER"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartBaletrailer, nullptr, nullptr, 
	{
		.Name = "Spawn Bale Trailer",
		.Id = "spawn_baletrailer",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartRomero()
{
	Vector3 playerPos = GetPlayerPos();

	CreatePoolVehicle("ROMERO"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

// clang-format off
REGISTER_EFFECT(OnStartRomero, nullptr, nullptr, 
	{
		.Name = "Where's The Funeral?",
		.Id = "spawn_romero",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
// clang-format on

static void OnStartWizardBroom()
{
	static const Hash oppressorHash = "OPPRESSOR2"_hash;
	static const Hash broomHash     = "prop_tool_broom"_hash;
	LoadModel(oppressorHash);
	LoadModel(broomHash);

	Ped player        = PLAYER_PED_ID();
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
REGISTER_EFFECT(OnStartWizardBroom, nullptr, nullptr, 
	{
		.Name = "You're A Wizard, Franklin",
		.Id = "vehs_spawn_wizard_broom",
		.EffectGroupType = EffectGroupType::SpawnGeneric
	}
);
