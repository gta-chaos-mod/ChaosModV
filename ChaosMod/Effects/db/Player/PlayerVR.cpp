#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static Ped clone;
static Vector3 coords;
static float heading;
static Vector3 rot;
static Vehicle cloneVeh;

static void OnStart() {
	Ped player = PLAYER_PED_ID();

	// (kolyaventuri): Recall position
	heading = GET_ENTITY_HEADING(player);
	coords = GET_ENTITY_COORDS(player, true);
	rot = GET_ENTITY_ROTATION(player, 0);

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
		cloneVeh = CREATE_VEHICLE(model, loc.x, loc.y, loc.z, GET_ENTITY_HEADING(veh), false, false, 0);
	}

	// (kolyaventuri): Apply camera effects
	if (GET_TIMECYCLE_TRANSITION_MODIFIER_INDEX() == -1 && GET_TIMECYCLE_MODIFIER_INDEX() == -1)
	{
		SET_TRANSITION_TIMECYCLE_MODIFIER("secret_camera", 1.5f);
	}

	AUDIO::PLAY_SOUND_FRONTEND(-1, "1st_Person_Transition", "PLAYER_SWITCH_CUSTOM_SOUNDSET", 0);
}

static void OnTick() {
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
	SET_PED_AS_NO_LONGER_NEEDED(&clone);
	DELETE_PED(&clone);

	SET_ENTITY_AS_NO_LONGER_NEEDED(&cloneVeh);
	DELETE_ENTITY(&cloneVeh);

	// (kolyaventuri): Clean up FP
	SET_FOLLOW_PED_CAM_VIEW_MODE(1);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(1);

	// (kolyaventuri): Clear camera effects
	CLEAR_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VR, OnStart, OnStop, OnTick);