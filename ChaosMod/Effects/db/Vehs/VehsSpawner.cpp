#include <stdafx.h>

static void OnStartRhino()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("RHINO"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect1(EFFECT_SPAWN_TANK, OnStartRhino);

static void OnStartAdder()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("ADDER"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect2(EFFECT_SPAWN_ADDER, OnStartAdder);

static void OnStartDump()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("DUMP"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect3(EFFECT_SPAWN_DUMP, OnStartDump);

static void OnStartMonster()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("MONSTER"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect4(EFFECT_SPAWN_MONSTER, OnStartMonster);

static void OnStartBMX()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BMX"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect5(EFFECT_SPAWN_BMX, OnStartBMX);

static void OnStartTug()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("TUG"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect6(EFFECT_SPAWN_TUG, OnStartTug);

static void OnStartCargoplane()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("CARGOPLANE"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect7(EFFECT_SPAWN_CARGO, OnStartCargoplane);

static void OnStartBus()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BUS"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect8(EFFECT_SPAWN_BUS, OnStartBus);

static void OnStartBlimp()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BLIMP"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect9(EFFECT_SPAWN_BLIMP, OnStartBlimp);

static void OnStartBuzzard()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BUZZARD"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect10(EFFECT_SPAWN_BUZZARD, OnStartBuzzard);

static void OnStartFaggio()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("FAGGIO"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect11(EFFECT_SPAWN_FAGGIO, OnStartFaggio);

static void OnStartRuiner3()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("RUINER3"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect12(EFFECT_SPAWN_RUINER3, OnStartRuiner3);

static void OnStartRandom()
{
	static std::vector<Hash> vehModels = Memory::GetAllVehModels();

	if (!vehModels.empty())
	{
		Vehicle veh = CreateTempVehicleOnPlayerPos(vehModels[Random::GetRandomInt(0, vehModels.size() - 1)], GET_ENTITY_HEADING(PLAYER_PED_ID()));

		// Also apply random upgrades
		SET_VEHICLE_MOD_KIT(veh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			SET_VEHICLE_MOD(veh, i, max > 0 ? Random::GetRandomInt(0, max - 1) : 0, Random::GetRandomInt(0, 1));

			TOGGLE_VEHICLE_MOD(veh, i, Random::GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, Random::GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(veh, Random::GetRandomInt(0, 6));
	}
}

static RegisterEffect registerEffect13(EFFECT_SPAWN_RANDOM, OnStartRandom);

static void OnStartBaletrailer()
{
	CreateTempVehicleOnPlayerPos(GET_HASH_KEY("BALETRAILER"), GET_ENTITY_HEADING(PLAYER_PED_ID()));
}

static RegisterEffect registerEffect14(EFFECT_SPAWN_BALETRAILER, OnStartBaletrailer);