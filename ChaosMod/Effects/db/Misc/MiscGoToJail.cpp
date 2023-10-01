/*
    Effect by Rylxnd
*/

#include <stdafx.h>

static const Vector3 destination = Vector3::Init(473.1f, -1023.5f, 28.1f);

static void OnStart()
{
	static const Ped plr = PLAYER_PED_ID();
	Vector3 pos          = GET_ENTITY_COORDS(plr, false);

	Vehicle car          = CreatePoolVehicle("POLICE2"_hash, pos.x, pos.y, pos.z, GET_ENTITY_HEADING(plr));
	Ped cop              = CreatePoolPedInsideVehicle(car, 4, "S_M_Y_Cop_01"_hash, -1);

	SET_PED_INTO_VEHICLE(plr, car, 1);
	SET_VEHICLE_SIREN(car, 1);

	TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(cop, car, destination.x, destination.y, destination.z, 9999, 537395716, 10.f);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(cop, true);

	SET_ENTITY_AS_NO_LONGER_NEEDED(&cop);
	SET_ENTITY_AS_NO_LONGER_NEEDED(&car);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Bad Boys",
		.Id = "misc_go_to_jail",
		.EffectGroupType = EffectGroupType::SpawnGeneric,
	}
);
