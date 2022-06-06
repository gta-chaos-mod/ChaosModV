/*
Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Types.h"

struct OrbitPed
{
	Ped ped;
	float angle;
};
static std::vector<OrbitPed> orbitingPeds;
static int pedCount = 20;

static void OnStart()
{
	Ped player  = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, false);
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	int count  = 5;
	// Create new bodyguards if non available
	if (orbitingPeds.empty())
	{
		static Hash pedHash = GET_HASH_KEY("MP_M_FIBSec_01");
		LoadModel(pedHash);
		for (int i = 0; i < pedCount; i++)
		{
			Ped ped = CREATE_PED(-1, pedHash, 0, 0, 0, 0, true, false);
			SET_ENTITY_HAS_GRAVITY(ped, false);
			SET_PED_CAN_RAGDOLL(ped, false);
			SET_ENTITY_COLLISION(ped, false, true);
			SET_PED_CAN_BE_TARGETTED_BY_PLAYER(ped, player, false);
			float offset    = (360 / pedCount) * i;
			OrbitPed orbPed = { ped, offset };
			orbitingPeds.push_back(orbPed);
			if (--count == 0)
			{
				WAIT(0);
				count = 5;
			}
		}
	}
	Entity entityToCircle = player;
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		entityToCircle = GET_VEHICLE_PED_IS_IN(player, false);
	}
	Vector3 pos = GET_ENTITY_COORDS(entityToCircle, false);
	Vector3 min;
	Vector3 max;
	GET_MODEL_DIMENSIONS(GET_ENTITY_MODEL(entityToCircle), &min, &max);
	float height      = max.z - min.z;
	float zCorrection = (-height / 2) + 0.3;
	float heading     = GET_ENTITY_HEADING(entityToCircle);
	for (std::vector<OrbitPed>::iterator it = orbitingPeds.begin(); it != orbitingPeds.end();)
	{
		OrbitPed pedInfo = *it;
		if (IS_PED_DEAD_OR_DYING(pedInfo.ped, false))
		{
			SET_ENTITY_HEALTH(pedInfo.ped, 0, 0);
			SET_ENTITY_ALPHA(pedInfo.ped, 0, true);
			SET_PED_AS_NO_LONGER_NEEDED(&pedInfo.ped);
			DELETE_PED(&pedInfo.ped);
			it = orbitingPeds.erase(it);
			if (--count == 0)
			{
				WAIT(0);
				count = 5;
			}
		}
		else
		{
			Vector3 coord = GetCoordAround(entityToCircle, heading - pedInfo.angle, 3, zCorrection, true);
			SET_ENTITY_COORDS(pedInfo.ped, coord.x, coord.y, coord.z, false, false, false, false);
			SET_ENTITY_HEADING(pedInfo.ped, pedInfo.angle + 90); // Always face away from player
			TASK_STAND_STILL(pedInfo.ped, 5000);
			it->angle += 1;
			it++;
		}
	}
}

static void OnStop()
{
	int count = 5;
	for (std::vector<OrbitPed>::iterator it = orbitingPeds.begin(); it != orbitingPeds.end();)
	{
		OrbitPed pedInfo = *it;
		SET_ENTITY_HEALTH(pedInfo.ped, 0, 0);
		SET_ENTITY_ALPHA(pedInfo.ped, 0, true);
		SET_PED_AS_NO_LONGER_NEEDED(&pedInfo.ped);
		DELETE_PED(&pedInfo.ped);
		it = orbitingPeds.erase(it);
		if (--count == 0)
		{
			WAIT(0);
			count = 5;
		}
	}
	orbitingPeds.clear();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "Witness Protection",
        .Id = "misc_witness_protection",
		.IsTimed = true
    }
);