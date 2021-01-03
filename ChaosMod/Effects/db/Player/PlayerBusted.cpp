#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

enum ArrestState {
	start = 0,
	startBust,
	handsUp,
	cleanup
};

static const int stopTime = 300;
static const char* cops[3] = { "s_m_y_cop_01", "s_f_y_cop_01", "csb_cop" };
static int currentMode = ArrestState::start;
static int lastModeTime = 0;
static int nextModeTime = 0;
static Object waterObj = 0;

static void OnStart() {
	Player playerId = PLAYER_ID();
	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, true);
	const char* hashKey = cops[g_random.GetRandomInt(0, 2)];

	currentMode = ArrestState::start;
	lastModeTime = 0;
	nextModeTime = 0;

	Ped cop = CreatePoolPed(6, GET_HASH_KEY(hashKey), 0.f, 0.f, 0.f, GET_ENTITY_HEADING(player));
	SET_PED_AS_COP(cop, true);
	GIVE_WEAPON_TO_PED(cop, GET_HASH_KEY("WEAPON_PISTOL"), 100, false, true); // (kolyaventuri): Cops must have weapon wielded for the arrest task to fire

	Vector3 minSize;
	Vector3 maxSize;
	Vehicle veh;
	boolean inVeh = false;

	if (IS_PED_IN_ANY_VEHICLE(player, false)) {
		veh = GET_VEHICLE_PED_IS_IN(player, false);
		SET_ENTITY_AS_MISSION_ENTITY(veh, 0, 0);
		inVeh = true;
		BRING_VEHICLE_TO_HALT(veh, 3.f, stopTime, 0);
		WAIT(stopTime);
		FREEZE_ENTITY_POSITION(veh, 1);
	}

	if (inVeh && (IS_PED_IN_ANY_BOAT(player) || IS_PED_IN_ANY_PLANE(player) || IS_PED_IN_ANY_HELI(player) || IS_PED_IN_ANY_TRAIN(player))) {
		// Calculate entity size
		Hash model = GET_ENTITY_MODEL((Entity)veh);
		Vector3 min, max, size;
		GET_MODEL_DIMENSIONS(model, &min, &max);
		size = max - min;

		// Load a platform
		Hash displayHash = GET_HASH_KEY("prop_huge_display_01");
		pos = GET_ENTITY_COORDS(player, true);
		LoadModel(displayHash);

		// Figure out where the platform can go
		float waterZ, groundZ;
		GET_WATER_HEIGHT(pos.x, pos.y, pos.z, &waterZ);
		GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z, &groundZ, 0, 0);
		float waterGround = max(waterZ, groundZ);

		float belowEntity = pos.z - size.z;
		float useZ = max(waterGround, belowEntity);

		// Spawn the invisible platform
		waterObj = CREATE_OBJECT(displayHash, pos.x, pos.y, useZ, true, true, true);
		SET_ENTITY_ROTATION(waterObj, 90, 0, 0, 2, true);
		FREEZE_ENTITY_POSITION(waterObj, 1);
		SET_ENTITY_VISIBLE(waterObj, false, false);
	}

	if (inVeh) {
		TASK_LEAVE_VEHICLE(player, veh, 256);
	}

	/*
	* (kolyaventuri): Ensure that the police cannot spawn inside of the car model.
	* It was causing issues where the stars would immediately bump to 2, given the cop was injured
	*/
	pos = GET_ENTITY_COORDS(player, true);
	Hash model = GET_ENTITY_MODEL(player);
	GET_MODEL_DIMENSIONS(model, &minSize, &maxSize);
	Vector3 modelSize = (maxSize - minSize) * 1.05f;

	// Figure out where we can TP the police to
	float offsetX = modelSize.x;
	float offsetY = modelSize.y;
	Vector3 newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, modelSize.x, modelSize.y, 0.0);
	int tries = 0;
	static const int dirs[3][2] = { {-1, 1} ,{1, -1},{-1, -1} };
	while (IS_POSITION_OCCUPIED(newPos.x, newPos.y, newPos.z, 0.5f, true, true, 0, 0, 0, 0, 0) && tries < 3) {
		newPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, dirs[tries][0] * modelSize.x, dirs[tries][1] * modelSize.y, 0.0);
		tries++;
	}
	
	if (inVeh) {
		SET_ENTITY_NO_COLLISION_ENTITY(cop, veh, false); // Fixes issues where police would stumble or colldie with vehicle
		WAIT(stopTime);
	}
	SET_ENTITY_COORDS(cop, newPos.x, newPos.y, newPos.z, 1, 0, 0, 1);

	while (currentMode < ArrestState::cleanup) {
		if (IS_PED_DEAD_OR_DYING(player, 1)) {
			// (kolyaventuri): If the player dies, give up
			SET_ENTITY_AS_MISSION_ENTITY(cop, true, true);
			DELETE_PED(&cop);
			if (DOES_ENTITY_EXIST(waterObj)) DELETE_OBJECT(&waterObj);

			if (DOES_ENTITY_EXIST(veh)) DELETE_VEHICLE(&veh);
			break;
		}

		// (kolyaventuri): Ensure player cannot move
		DISABLE_ALL_CONTROL_ACTIONS(0);
		WAIT(0);

		int currentTime = GET_GAME_TIMER();
		if (currentTime - lastModeTime > nextModeTime)
		{
			nextModeTime = 2000;
			lastModeTime = currentTime;
			currentMode++;
		}
		else {
			continue;
		}

		if (currentMode != ArrestState::cleanup) {
			// (kolyaventuri): Player must be wanted for effect to fire
			SET_PLAYER_WANTED_LEVEL(playerId, 1, false);
			SET_PLAYER_WANTED_LEVEL_NOW(playerId, false);
		}

		switch (currentMode) {
			case ArrestState::startBust:
				TASK_ARREST_PED(cop, player);
				lastModeTime = GET_GAME_TIMER();
				nextModeTime = 7500;
				currentMode++;
				break;
			case ArrestState::cleanup:
				SET_ENTITY_AS_MISSION_ENTITY(cop, true, true);
				DELETE_PED(&cop);
				if (DOES_ENTITY_EXIST(waterObj)) DELETE_OBJECT(&waterObj);
				if (DOES_ENTITY_EXIST(veh)) DELETE_VEHICLE(&veh);
				break;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BUSTED, OnStart);