#include <stdafx.h>

static Hash miljethash;

static Hash pilothash;
static Vehicle veh;

static int spawnTimer = -1;
static int currenttick = 1;

static void onStart() {
	miljethash = GET_HASH_KEY("luxor");
	pilothash = GET_HASH_KEY("s_m_m_pilot_01");
	
}

static void OnTick()
{

	LoadModel(miljethash);
	LoadModel(pilothash);
	int current_time = GET_GAME_TIMER();
	currenttick++;
	if (current_time > spawnTimer + 10000) {
		spawnTimer = current_time;
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
		switch (g_Random.GetRandomInt(0, 4)) {
			case 0:
				veh = CREATE_VEHICLE(miljethash, playerPos.x, playerPos.y - 400, playerPos.z + 400, 0.0f, true, false, false);
				break;
			case 1:
				veh = CREATE_VEHICLE(miljethash, playerPos.x, playerPos.y + 400, playerPos.z + 400, 180.0f, true, false, false);
				break;
			case 2:
				veh = CREATE_VEHICLE(miljethash, playerPos.x - 400, playerPos.y, playerPos.z + 400, 270.0f, true, false, false);
				break;
			case 3:
				veh = CREATE_VEHICLE(miljethash, playerPos.x - 400, playerPos.y, playerPos.z + 400, 90.0f, true, false, false);
				break;
			default:
				veh = CREATE_VEHICLE(miljethash, playerPos.x, playerPos.y - 400, playerPos.z + 400, 0.0f, true, false, false);
				break;
		}
		Ped driver = CREATE_PED_INSIDE_VEHICLE(veh, -1, pilothash, -1, true, false);
		SET_VEHICLE_FORWARD_SPEED(veh, 40);
		SET_VEHICLE_ENGINE_ON(veh, true, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(driver, true);

		TASK_VEHICLE_MISSION_PED_TARGET(driver, veh, PLAYER_PED_ID(), 21, 9999.f, 17039360, .0f, .0f, false);

	}
	Vector3 entityCoord = GET_ENTITY_COORDS(veh, false);
	Vector3 playerCoord = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	if (!_IS_VEHICLE_DAMAGED(veh) && currenttick % 5 == 1) {
		APPLY_FORCE_TO_ENTITY(veh, 1, (entityCoord.x - playerCoord.x) * -1.f, (entityCoord.y - playerCoord.y) * -1.f, (entityCoord.z - playerCoord.z) * -1.f, 0, 0, 0, false, false, true, true, false, true);
	}

	
}



static RegisterEffect registerEffect(EFFECT_MISC_MAJOR_TURBULENCE, onStart, nullptr, OnTick, EffectInfo
	{

		.Name = "Major Turbulence",
		.Id = "misc_major_turbulence",
		.IsTimed = true
	}
);