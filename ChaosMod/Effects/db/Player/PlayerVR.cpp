#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static Ped clone;
static Vector3 coords;
static float heading;

static void OnStart() {
	Ped player = PLAYER_PED_ID();

	// (kolyaventuri): Recall position
	heading = GET_ENTITY_HEADING(player);
	coords = GET_ENTITY_COORDS(player, true);

	// (kolyaventuri): Create clone
	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_CLONE_FRIENDLY", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	clone = CLONE_PED(player, heading, false , true);
	SET_ENTITY_INVINCIBLE(clone, true);

	SET_PED_RELATIONSHIP_GROUP_HASH(clone, relationshipGroup);
	FREEZE_ENTITY_POSITION(clone, true);

	// (kolyaventuri): Apply camera effects
	if (GET_TIMECYCLE_TRANSITION_MODIFIER_INDEX() == -1 && GET_TIMECYCLE_MODIFIER_INDEX() == -1)
	{
		SET_TRANSITION_TIMECYCLE_MODIFIER("secret_camera", 1.5f);
	}
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

	// (kolyaventuri): Clean up clone
	SET_ENTITY_INVINCIBLE(clone, false);
	SET_ENTITY_HEALTH(clone, 0, 0);
	SET_ENTITY_ALPHA(clone, 0, true);
	SET_PED_AS_NO_LONGER_NEEDED(&clone);
	DELETE_PED(&clone);

	// (kolyaventuri): Clean up FP
	SET_FOLLOW_PED_CAM_VIEW_MODE(1);
	SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(1);

	// (kolyaventuri): Clear camera effects
	CLEAR_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_VR, OnStart, OnStop, OnTick);