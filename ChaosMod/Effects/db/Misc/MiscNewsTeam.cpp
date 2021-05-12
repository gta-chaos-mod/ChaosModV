/*
	Effect by Last0xygen
*/

#include <stdafx.h>


static Vector3 GetCoordsAround(Vector3 pos, float radius)
{
	int randOffset = g_random.GetRandomInt(0, 360);
	Vector3 res;
	res.x = pos.x + (COS(randOffset) * radius);
	res.y = pos.y + (SIN(randOffset) * radius);
	res.z = pos.z + 50;
	return res;
}

static Vehicle heli = 0;
static Ped pilot = 0;
static Camera heliCam = 0;
static int lastPositionGoal = 0;
static int scaleForm = 0;
static Vector3 targetCoords;

static void OnStart()
{
	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, false);


	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_NEWS_TEAM", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(2, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(2, playerGroup, relationshipGroup);

	// Spawn Helicopter
	Hash heliHash = GET_HASH_KEY("frogger");
	LoadModel(heliHash);
	Vector3 aroundCoords;
	float groundZ;
	bool isValidCoord = false;
	do {
		aroundCoords = GetCoordsAround(pos, 70);
		isValidCoord = GET_GROUND_Z_FOR_3D_COORD(aroundCoords.x, aroundCoords.y, aroundCoords.z, &groundZ, false, false);
	} while (!isValidCoord);
	groundZ = max(groundZ, aroundCoords.z);
	heli = CREATE_VEHICLE(heliHash, aroundCoords.x, aroundCoords.y, groundZ, 0, true, false, false);
	SET_VEHICLE_ENGINE_ON(heli, true, true, true);
	SET_VEHICLE_FORWARD_SPEED(heli, 0); // Needed, so the heli doesn't fall down instantly
	MODIFY_VEHICLE_TOP_SPEED(heli, 100);
	SET_HELI_BLADES_SPEED(heli, 1);
	WAIT(0);
	// Spawn Pilot
	Hash pilotModel = GET_HASH_KEY("csb_reporter");
	LoadModel(pilotModel);
	pilot = CREATE_PED_INSIDE_VEHICLE(heli, 26, pilotModel, -1, true, true);
	SET_PED_RELATIONSHIP_GROUP_HASH(pilot, relationshipGroup);

	heliCam = CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", 0, 0, 0, 0, 0, 0, 50, true, 2);
	Vector3 rearBottomLeft, frontTopRight;
	GET_MODEL_DIMENSIONS(heliHash, &rearBottomLeft, &frontTopRight);
	Vector3 camCoords;
	camCoords.y = frontTopRight.y / 2;
	camCoords.z = rearBottomLeft.z;
	ATTACH_CAM_TO_ENTITY(heliCam, heli, 0, frontTopRight.y / 2, rearBottomLeft.z - 2, true);
	POINT_CAM_AT_ENTITY(heliCam, player, 0, 0, 0, true);
	SET_CAM_ACTIVE(heliCam, true);
	RENDER_SCRIPT_CAMS(true, true, 1000, true, true, true);
	scaleForm = REQUEST_SCALEFORM_MOVIE("breaking_news");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
	{
		WAIT(0);
	}
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SET_TEXT");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("Chaos Mod Player trying to survive");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING("\"He won't survive\", mod contributors say");
	END_SCALEFORM_MOVIE_METHOD();
}

static void OnStop()
{
	RENDER_SCRIPT_CAMS(false, true, 1000, true, true, true);
	SET_CAM_ACTIVE(heliCam, false);
	SET_ENTITY_AS_MISSION_ENTITY(pilot, true, true);
	DELETE_ENTITY(&pilot);
	SET_ENTITY_AS_MISSION_ENTITY(heli, true, true);
	DELETE_ENTITY(&heli);
	pilot = 0;
	heli = 0;
	scaleForm = 0;
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	int current_time = GET_GAME_TIMER();
	if (current_time - lastPositionGoal > 1000)
	{
		lastPositionGoal = current_time;
		targetCoords = GetCoordsAround(Vector3(), g_random.GetRandomFloat(20, 30));
		TASK_HELI_CHASE(pilot, player, targetCoords.x, targetCoords.y, targetCoords.z + g_random.GetRandomFloat(5, 10));
		SET_PED_KEEP_TASK(pilot, true);
	}
	if (scaleForm > 0)
	{
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_NEWS_TEAM, OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "News Team",
        .Id = "misc_news_team",
		.IsTimed = true,
		.IsShortDuration = true
    }
);
