#include "stdafx.h"

void att_ped_to_veh(Ped model, Vehicle vehicle, const Vector3& offset, const Vector3& rotation)
{
	if (DOES_ENTITY_EXIST(vehicle))
	{
			SET_ENTITY_LOD_DIST(model, 696969);
			ATTACH_ENTITY_TO_ENTITY(model, vehicle, -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 0, 0, 0, 0, 2, 1);
			DETACH_ENTITY(model, 1, 1);
			ATTACH_ENTITY_TO_ENTITY(model, vehicle, -1, offset.x, offset.y, offset.z, rotation.x, rotation.y, rotation.z, 0, 0, 0, 0, 2, 1);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(model, 1);

			TASK_STAND_STILL(model, -1);
			SET_PED_CAN_RAGDOLL(model, false);
			SET_ENTITY_INVINCIBLE(model, true);
			SET_PED_AS_NO_LONGER_NEEDED(&model);
	}
}

static void OnStart()
{
	Vector3 o = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
	Ped a = CreatePoolPed(4, GET_HASH_KEY("a_c_deer"), 0, -0.6, 0, 0);
	Vehicle b = CreatePoolVehicle(GET_HASH_KEY("vortex"), o.x, o.y, o.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
	SET_ENTITY_ALPHA(b, 0, 0);
	att_ped_to_veh(a, b, Vector3(0, 0, 0.07f), Vector3());
	SET_PED_INTO_VEHICLE(PLAYER_PED_ID(), b, -1);
}

static RegisterEffect reg(EFFECT_PLAYER_HORSE, OnStart, EffectInfo
	{
		.Name = "Red Dead Redemption",
		.Id = "player_horse"
	}
);