#include <stdafx.h>

#include "Util/Player.h"
#include "Util/Vehicle.h"

static int lastTpTime;
static int currentTpWaitTime;

struct VehCoord
{
	Vector3 coord;
	Vector3 vel;
	Vector3 rot;
	Vector3 rotVel;
	float speed;
};
struct PropCoord
{
	Vector3 coord;
	Vector3 vel;
	Vector3 rot;
	Vector3 rotVel;
};
static void OnStart()
{
	std::vector<Entity> vehs;
	std::vector<VehCoord> vehCoords;
	std::vector<Entity> peds;
	std::vector<Vector3> pedCoords;
	std::vector<Entity> props;
	std::vector<PropCoord> propCoords;

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false) && !IS_PED_DEAD_OR_DYING(ped, true) && DOES_ENTITY_EXIST(ped))
		{
			Vector3 vec = GET_ENTITY_COORDS(ped, false);
			float z;
			// Only consider if its not underground.
			if (GET_GROUND_Z_FOR_3D_COORD(vec.x, vec.y, vec.z, &z, false, false))
			{
				if (abs(vec.z - z) < 2.f)
					vec.z = z;
				peds.push_back(ped);
				pedCoords.push_back(vec);
			}
		}
	}

	for (auto veh : GetAllVehs())
	{
		if (DOES_ENTITY_EXIST(veh))
		{
			Vector3 vec = GET_ENTITY_COORDS(veh, false);
			float _;
			if (GET_GROUND_Z_FOR_3D_COORD(vec.x, vec.y, vec.z, &_, false, false))
			{
				vehs.push_back(veh);
				vehCoords.push_back({ vec, GET_ENTITY_VELOCITY(veh), GET_ENTITY_ROTATION(veh, 2),
				                      GET_ENTITY_ROTATION_VELOCITY(veh), GET_ENTITY_SPEED(veh) });
			}
		}
	}

	for (auto prop : GetAllProps())
	{
		if (DOES_ENTITY_EXIST(prop))
		{
			Vector3 vec = GET_ENTITY_COORDS(prop, false);
			float _;
			if (GET_GROUND_Z_FOR_3D_COORD(vec.x, vec.y, vec.z, &_, false, false))
			{
				props.push_back(prop);
				propCoords.push_back({ vec, GET_ENTITY_VELOCITY(prop), GET_ENTITY_ROTATION(prop, 2),
				                       GET_ENTITY_ROTATION_VELOCITY(prop) });
			}
		}
	}

	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::shuffle(vehCoords.begin(), vehCoords.end(), generator);
	std::shuffle(pedCoords.begin(), pedCoords.end(), generator);
	std::shuffle(propCoords.begin(), propCoords.end(), generator);

	for (int i = 0; i < vehs.size(); i++)
	{
		Entity veh = vehs[i];
		if (!DOES_ENTITY_EXIST(veh))
		{
			continue;
		}
		struct VehCoord vehCoord = vehCoords[i];
		SET_ENTITY_COORDS(veh, vehCoord.coord.x, vehCoord.coord.y, vehCoord.coord.z, false, false, false, false);
		SET_ENTITY_VELOCITY(veh, vehCoord.vel.x, vehCoord.vel.y, vehCoord.vel.z);
		SET_ENTITY_ROTATION(veh, vehCoord.rot.x, vehCoord.rot.y, vehCoord.rot.z, 2, true);
		_SET_ENTITY_ANGULAR_VELOCITY(veh, vehCoord.rotVel.x, vehCoord.rotVel.y, vehCoord.rotVel.z);
		SET_VEHICLE_FORWARD_SPEED(veh, vehCoord.speed);
	}
	for (int i = 0; i < peds.size(); i++)
	{
		Entity ped = peds[i];
		if (!DOES_ENTITY_EXIST(ped))
		{
			continue;
		}
		SET_ENTITY_COORDS(ped, pedCoords[i].x, pedCoords[i].y, pedCoords[i].z, false, false, false, false);
		float z;
		if (GET_GROUND_Z_FOR_3D_COORD(pedCoords[i].x, pedCoords[i].y, pedCoords[i].z, &z, false, false))
		{
			if (pedCoords[i].z - z >= 2.f)
				SET_PED_TO_RAGDOLL(ped, 100, 100, 2, true, true, false);
		}
	}
	for (int i = 0; i < props.size(); i++)
	{
		Entity prop = props[i];
		if (!DOES_ENTITY_EXIST(prop))
		{
			continue;
		}
		struct PropCoord propCoord = propCoords[i];
		SET_ENTITY_COORDS(prop, propCoord.coord.x, propCoord.coord.y, propCoord.coord.z, false, false, false, false);
		SET_ENTITY_VELOCITY(prop, propCoord.vel.x, propCoord.vel.y, propCoord.vel.z);
		SET_ENTITY_ROTATION(prop, propCoord.rot.x, propCoord.rot.y, propCoord.rot.z, 2, true);
		_SET_ENTITY_ANGULAR_VELOCITY(prop, propCoord.rotVel.x, propCoord.rotVel.y, propCoord.rotVel.z);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Shuffle Locations Of Everything",
		.Id = "misc_shufflecoords",
	}
);