/*
	Author: Kanawanagasaki
*/

#include <algorithm>
#include <math.h>
#include <random>
#include <stdafx.h>

#include "Memory/Physics.h"
#include "Util/Text.h"

struct UfoData
{
	Object prop;
	Ped target;
	Vector3 destination = Vector3();
	Vector3 prevDestination = Vector3();
	Vector3 velocity = Vector3();
	Vector3 acceleration = Vector3();
	bool isElevating = false;
	bool isRunningAway = false;
	bool fadeOut = false;
	int opacity = 255;
};

const float UFO_RADIUS = 26.f;
const int MAX_UFO_AMOUNT = 3;
const int SPAWN_INTERVAL = 13000;
static Vector3 DROP_POS = Vector3(-1130.f, 4925.f, 350.f);

static std::vector<UfoData> ufoList = {};
static int prevTick = 0;
static int prevSpawnTick = 0;
static Camera ufoCamera = 0;

static Ped GetPedForUfo()
{
	Ped player = PLAYER_PED_ID();

	// 10% chance to select a player
	if (g_Random.GetRandomInt(1, 10) == 1)
	{
		for (auto ufo : ufoList)
			if (ufo.target == player)
				return 0;
		return player;
	}

	// ped should be closest to player
	Vector3 playerPos = GET_ENTITY_COORDS(player, false);
	auto peds = GetAllPeds().ToArray();

	Ped ret = 0;
	float minDist = 1000.f;

	for (Ped ped : peds)
	{
		// ped shouldn't be a player
		if (ped == player)
			continue;

		Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

		// ped shouldn't be in range (ignoring z) of another ufo so ufos not end up being in each other, also ped
		// shouldn't be targeted already
		for (auto ufo : ufoList)
		{
			Vector3 ufoPos = GET_ENTITY_COORDS(ufo.prop, false);

			float distanceToUfo =
				GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, ufoPos.x, ufoPos.y, ufoPos.z, 0);
			if (distanceToUfo < UFO_RADIUS * 2)
				continue;
			if (ufo.target == ped)
				continue;
		}

		float distance =
			GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, playerPos.x, playerPos.y, playerPos.z, 0);
		if (distance < minDist)
		{
			ret = ped;
			minDist = distance;
		}
	}

	return ret;
}

static void OnStart()
{
	prevSpawnTick = GET_GAME_TIMER() - SPAWN_INTERVAL;
	prevTick = GET_GAME_TIMER();
}

static Object ufoTest = 0;

static void OnTick()
{
	int currTick = GET_GAME_TIMER();
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	// attempt to create more UFOs
	if (ufoList.size() < MAX_UFO_AMOUNT && currTick - prevSpawnTick >= SPAWN_INTERVAL)
	{
		Ped ped = GetPedForUfo();
		if (ped)
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

			float angle = g_Random.GetRandomFloat(0, 6.28318f);

			Hash model = GET_HASH_KEY("p_spinning_anus_s");
			Object prop =
				CreatePoolProp(model, pedPos.x + cos(angle) * 100, pedPos.y + sin(angle) * 100, pedPos.z + 200, true);
			SET_ENTITY_COLLISION(prop, false, false);

			ufoList.push_back(UfoData{ prop, ped, Vector3(pedPos.x, pedPos.y, pedPos.z + 100.f) });
		}

		prevSpawnTick = currTick;
	}

	// seconds between ticks
	float timeDiff = static_cast<float>(currTick - prevTick) / 1000.f;

	for (int i = 0; i < ufoList.size(); i++)
	{
		UfoData& ufo = ufoList[i];

		if (!DOES_ENTITY_EXIST(ufo.prop))
		{
			ufoList.erase(ufoList.begin() + i);
			i--;
			continue;
		}

		Vector3 ufoPos = GET_ENTITY_COORDS(ufo.prop, false);

		// despawn ufo if it is far away
		if (GET_DISTANCE_BETWEEN_COORDS(ufoPos.x, ufoPos.y, ufoPos.z, playerPos.x, playerPos.y, playerPos.z, 0) > 1000)
		{
			DELETE_OBJECT(&ufo.prop);
			ufoList.erase(ufoList.begin() + i);
			i--;
			continue;
		}

		// trying to get another ped in case the ped despawned
		if (!DOES_ENTITY_EXIST(ufo.target))
		{
			Ped ped = GetPedForUfo();
			if (ped == 0)
				continue;

			ufo.target = ped;
			ufo.isElevating = false;
			ufo.isRunningAway = false;
			ufo.fadeOut = false;
		}

		Entity target = ufo.target;
		Vector3 targetPos;
		if (IS_PED_IN_ANY_VEHICLE(ufo.target, false))
		{
			Vehicle vehicle = GET_VEHICLE_PED_IS_IN(ufo.target, false);
			targetPos = GET_ENTITY_COORDS(vehicle, false);
			target = vehicle;
		}
		else
			targetPos = GET_ENTITY_COORDS(ufo.target, false);

		ufo.acceleration = ufo.destination - ufo.velocity * 1.5 - ufoPos;
		ufo.velocity = ufo.velocity + ufo.acceleration * timeDiff;
		ufoPos = ufoPos + ufo.velocity * timeDiff;
		SET_ENTITY_COORDS(ufo.prop, ufoPos.x, ufoPos.y, ufoPos.z, false, false, false, false);
		Vector3 distance = ufo.destination - ufoPos;

		// ufo logic
		// fade out is the last stage, after we drop the ped into the cannibal camp
		if (ufo.fadeOut)
		{
			if (ufo.opacity > 0)
			{
				ufo.opacity--;
				SET_ENTITY_ALPHA(ufo.prop, ufo.opacity, false);
			}
			else
			{
				DELETE_OBJECT(&ufo.prop);
				ufoList.erase(ufoList.begin() + i);
				i--;
				continue;
			}
		}
		// isRunningAway is a stage when the ufo picked up a ped and is now flying to the cannibal camp
		else if (ufo.isRunningAway)
		{
			Vector3 toDropPosVec = DROP_POS - ufo.destination;
			if (toDropPosVec.Length() < 400)
				ufo.destination = DROP_POS;
			else
				ufo.destination = ufo.destination + toDropPosVec * (350.f / toDropPosVec.Length()) * timeDiff;

			if (IS_ENTITY_A_PED(target) && !IS_PED_RAGDOLL(target))
				SET_PED_TO_RAGDOLL(target, 5000, 5000, 0, true, true, false);

			float fullPathLen = GET_DISTANCE_BETWEEN_COORDS(DROP_POS.x, DROP_POS.y, DROP_POS.z, ufo.prevDestination.x,
				ufo.prevDestination.y, ufo.prevDestination.z, 0);
			float restPathLen =
				GET_DISTANCE_BETWEEN_COORDS(DROP_POS.x, DROP_POS.y, DROP_POS.z, ufoPos.x, ufoPos.y, ufoPos.z, 0);

			float progress = 1 - restPathLen / fullPathLen;

			SET_ENTITY_COORDS(target, ufoPos.x, ufoPos.y, ufoPos.z, false, false, false, false);

			SET_ENTITY_COORDS(ufo.prop, ufoPos.x, ufoPos.y,
				ufo.prevDestination.z + (DROP_POS.z - ufo.prevDestination.z) * progress
				+ sin(progress * 3.14159f) * 500.f,
				false, false, false, false);

			if (distance.Length() < 10.f && ufo.velocity.Length() < 15.f)
			{
				ufo.isRunningAway = false;
				ufo.fadeOut = true;

				if (ufoCamera && PLAYER_PED_ID() == ufo.target)
				{
					CAM::SET_CAM_ACTIVE(ufoCamera, false);
					CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
					CAM::DESTROY_CAM(ufoCamera, true);
					ufoCamera = 0;
				}
			}
			else if (ufoCamera && PLAYER_PED_ID() == ufo.target)
			{
				CAM::SET_CAM_ACTIVE(ufoCamera, true);
				Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
				Vector3 rotRad = rot / 180.f * 3.14159f;
				Vector3 camDir = Vector3(-sin(rotRad.z) * cos(rotRad.x), cos(rotRad.z) * cos(rotRad.x), sin(rotRad.x));
				Vector3 coords = ufoPos - camDir * 100.f;

				float fov = CAM::GET_GAMEPLAY_CAM_FOV();
				CAM::SET_CAM_PARAMS(ufoCamera, coords.x, coords.y, coords.z, rot.x, rot.y, rot.z, fov, 700, 0, 0, 2);
			}
		}
		// isElevating is a stage when the ufo hovers above the target ped and elevates it inside itself
		else if (ufo.isElevating)
		{
			if (IS_ENTITY_A_PED(target) && !IS_PED_RAGDOLL(target))
				SET_PED_TO_RAGDOLL(target, 5000, 5000, 0, true, true, false);

			float zForce = .50f;
			if (ufoPos.z <= targetPos.z)
				zForce = -.75f;
			Memory::ApplyForceToEntity(target, 3, (ufoPos.x - targetPos.x) * 4 * timeDiff,
				(ufoPos.y - targetPos.y) * 4 * timeDiff, zForce, 0, 0, 0, false, false, false,
				true, false, true);

			Vector3 ufoPedDir = targetPos - ufoPos;
			if (ufoPedDir.Length() < 7)
			{
				Vector3 toDropPosVec = DROP_POS - ufoPos;
				ufo.isElevating = false;
				ufo.isRunningAway = true;
				ufo.prevDestination = ufo.destination;
				ufo.destination = ufoPos + toDropPosVec * (200.f / toDropPosVec.Length());

				SET_ENTITY_COLLISION(target, true, false);
				SET_ENTITY_COLLISION(ufo.target, true, false);

				if (PLAYER_PED_ID() == ufo.target)
				{
					ufoCamera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
					CAM::RENDER_SCRIPT_CAMS(true, true, 700, 1, 1, 1);
				}
			}
			else
				DRAW_SPOT_LIGHT(ufoPos.x, ufoPos.y, ufoPos.z, ufoPedDir.x, ufoPedDir.y, ufoPedDir.z, 255, 255, 255, 500,
					1.f, 0.f, 7.f, 1.f);
		}
		// first stage, ufo trying to position itself above the target ped
		else
		{
			if (distance.Length() < 10)
			{
				ufo.isElevating = true;
				SET_ENTITY_COLLISION(target, false, false);
			}
			else
				ufo.destination = Vector3(targetPos.x, targetPos.y, targetPos.z + 75.f);
		}
	}

	prevTick = currTick;
}

static void OnStop()
{
	for (int i = 0; i < ufoList.size(); i++)
	{
		UfoData& ufo = ufoList[i];
		Object& prop = ufo.prop;

		SET_ENTITY_COLLISION(ufo.target, true, false);
		if (IS_ENTITY_A_PED(ufo.target) && IS_PED_IN_ANY_VEHICLE(ufo.target, false))
		{
			Vehicle vehicle = GET_VEHICLE_PED_IS_IN(ufo.target, false);
			SET_ENTITY_COLLISION(vehicle, true, false);
		}

		if (DOES_ENTITY_EXIST(prop))
			DELETE_OBJECT(&prop);
	}

	ufoList.clear();

	if (ufoCamera)
	{
		CAM::SET_CAM_ACTIVE(ufoCamera, false);
		CAM::RENDER_SCRIPT_CAMS(false, true, 700, 1, 1, 1);
		CAM::DESTROY_CAM(ufoCamera, true);
		ufoCamera = 0;
	}
}

static RegisterEffect registerEffect(EFFECT_SPAWN_UFO, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Spawn UFO",
		.Id = "misc_spawnufo",
		.EEffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
