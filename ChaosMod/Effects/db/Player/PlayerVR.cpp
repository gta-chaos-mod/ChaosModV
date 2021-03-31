#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static Ped clone;
static Vector3 coords;
static float heading;
static Vector3 rot;
static Vehicle cloneVeh;
static int camModes[3] = {0,0,0};

static void OnStart() {
	Ped player = PLAYER_PED_ID();

	// (kolyaventuri): Recall position
	heading = GET_ENTITY_HEADING(player);
	coords = GET_ENTITY_COORDS(player, true);
	rot = GET_ENTITY_ROTATION(player, 0);
	camModes[0] = GET_FOLLOW_PED_CAM_VIEW_MODE();
	camModes[1] = GET_FOLLOW_VEHICLE_CAM_VIEW_MODE();
	camModes[2] = GET_FOLLOW_VEHICLE_CAM_ZOOM_LEVEL();

	// (kolyaventuri): Create clone. CLONE_PED wasn't playing nice with entity deletion
	int type = GET_PED_TYPE(player);
	Hash model = GET_ENTITY_MODEL(player);
	clone = CreatePoolPed(type, model, coords.x, coords.y, coords.z, heading);
	CLONE_PED_TO_TARGET(player, clone); // Ensure that clone looks like player

	// (kolyaventuri): Fix an issue where the clone is placed above the player
	float groundZ = coords.z;
	GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, coords.z, &groundZ, 0, 0);
	coords.z = groundZ;
	
	SET_ENTITY_COORDS(clone, coords.x, coords.y, coords.z, 0, 0, 0, 1);
	SET_ENTITY_ROTATION(clone, rot.x, rot.y, rot.z, 0, 1);
	SET_ENTITY_INVINCIBLE(clone, true);
	FREEZE_ENTITY_POSITION(clone, true);

	// (kolyaventuri): Handle cars
	if (IS_PED_IN_ANY_VEHICLE(player, false)) {
		Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);
		Hash model = GET_ENTITY_MODEL(veh);
		Vector3 loc = GET_ENTITY_COORDS(veh, true);
		int colors = GET_VEHICLE_COLOUR_COMBINATION(veh);

		cloneVeh = CreatePoolVehicle(model, loc.x, loc.y, loc.z, GET_ENTITY_HEADING(veh));
		SET_VEHICLE_COLOUR_COMBINATION(cloneVeh, colors);
		SET_ENTITY_INVINCIBLE(cloneVeh, true);
		SET_ENTITY_NO_COLLISION_ENTITY(cloneVeh, veh, false);
		
		// Find seat player is in
		int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(model);
		int vehicleSeat = 0;
		for (int i = 0; i < maxSeats; i++) {
			if (GET_PED_IN_VEHICLE_SEAT(veh, i, false) == player) {
				vehicleSeat = i;
				break;
			}
		}

		SET_PED_INTO_VEHICLE(clone, cloneVeh, vehicleSeat);
	}

	AUDIO::PLAY_SOUND_FRONTEND(-1, "1st_Person_Transition", "PLAYER_SWITCH_CUSTOM_SOUNDSET", 0);
}

static void OnTick() {
	// (kolyaventuri): Apply camera effects
	if (GET_TIMECYCLE_TRANSITION_MODIFIER_INDEX() == -1 && GET_TIMECYCLE_MODIFIER_INDEX() == -1)
	{
		SET_TRANSITION_TIMECYCLE_MODIFIER("secret_camera", 1.5f);
	}

	Ped player = PLAYER_PED_ID();

	// (kolyaventuri): Replicate weapon
	GIVE_WEAPON_TO_PED(clone, GET_SELECTED_PED_WEAPON(player), 9999, true, true);

	// (kolyaventuri): Force first person

	SET_CINEMATIC_MODE_ACTIVE(false);

	SET_FOLLOW_PED_CAM_VIEW_MODE(4);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(4);
	SET_FOLLOW_VEHICLE_CAM_ZOOM_LEVEL(4);

	DISABLE_CONTROL_ACTION(0, 0, true);
}

static void OnStop() {
	Ped player = PLAYER_PED_ID();

	// (kolyaventuri): Snap player back to their original position
	TeleportPlayer(coords);
	SET_ENTITY_HEADING(player, heading);
	SET_ENTITY_ROTATION(player, rot.x, rot.y, rot.z, 0, 1);
	AUDIO::PLAY_SOUND_FRONTEND(-1, "1st_Person_Transition", "PLAYER_SWITCH_CUSTOM_SOUNDSET", 1);

	// (kolyaventuri): Clean up clone
	SET_ENTITY_AS_MISSION_ENTITY(clone, 0, 0);
	DELETE_PED(&clone);

	SET_ENTITY_AS_MISSION_ENTITY(cloneVeh, true, true);
	DELETE_VEHICLE(&cloneVeh);

	// (kolyaventuri): Clean up FP
	SET_FOLLOW_PED_CAM_VIEW_MODE(camModes[0]);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(camModes[1]);
	SET_FOLLOW_VEHICLE_CAM_ZOOM_LEVEL(camModes[2]);

	// (kolyaventuri): Clear camera effects
	CLEAR_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VR, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Virtual Reality",
		.Id = "player_vr",
		.IsTimed = true,
		.IsShortDuration = true
	}
);