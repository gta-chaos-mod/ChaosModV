#include <stdafx.h>

static Hash miljethash;

static Hash pilothash;
static Vehicle veh;

static int spawnTimer = -1;
static int currenttick = 1;

static int offsetx;
static int offsety;


static void OnStart() {
	miljethash = GET_HASH_KEY("luxor");
	pilothash = GET_HASH_KEY("s_m_m_pilot_01");
}


static void OnTick()
{

	LoadModel(miljethash);
	LoadModel(pilothash);
	int current_time = GET_GAME_TIMER();
	
	currenttick++;
	if (current_time > spawnTimer + 4000) {
		spawnTimer = current_time;
		Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);
		offsetx = g_Random.GetRandomInt(-85, 85);
		offsety = g_Random.GetRandomInt(-85, 85);
		int variant = g_Random.GetRandomInt(0, 4);
		int spawnOffsetX = 0;
		int spawnOffsetY = 0;
		float rotation = 90.0f * variant;
		if (variant % 2 == 0) {
			spawnOffsetY = variant == 0 ? -400 : 400;
		}
		else {
			spawnOffsetX = variant == 3 ? -400 : 400;
		}

		veh = CreatePoolVehicle(miljethash, playerPos.x + offsetx + spawnOffsetX, playerPos.y + offsety + spawnOffsetY, playerPos.z + 400, rotation);


		Ped driver = CreatePoolPedInsideVehicle(veh, -1, pilothash, -1);
		SET_VEHICLE_FORWARD_SPEED(veh, 55);
		SET_VEHICLE_ENGINE_ON(veh, true, true, false);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(driver, true);

		TASK_VEHICLE_MISSION_PED_TARGET(driver, veh, PLAYER_PED_ID(), 21, 9999.f, 17039360, .0f, .0f, false);
		
	}
	Vector3 entityCoord = GET_ENTITY_COORDS(veh, false);
	Vector3 playerCoord = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	if (!_IS_VEHICLE_DAMAGED(veh) && currenttick % 7 == 1 && GET_DISTANCE_BETWEEN_COORDS(entityCoord.x, entityCoord.y, entityCoord.z, playerCoord.x, playerCoord.y, playerCoord.z, true) > 150.0f) {
		APPLY_FORCE_TO_ENTITY(veh, 1, (entityCoord.x - (playerCoord.x + offsetx)) * -1.f, (entityCoord.y - (playerCoord.y + offsety)) * -1.f, (entityCoord.z - playerCoord.z) * -1.f, 0, 0, 0, false, false, true, true, false, true);
	}

	
}



static RegisterEffect registerEffect(EFFECT_MISC_MAJOR_TURBULENCE, OnStart, nullptr, OnTick, EffectInfo
	{

		.Name = "Major Turbulence",
		.Id = "misc_major_turbulence",
		.IsTimed = true
	}
);