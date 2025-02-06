/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static Vector3 GetCoordsAround(Vector3 pos, float radius)
{
	int randOffset = g_Random.GetRandomInt(0, 360);
	Vector3 res;
	res.x = pos.x + (std::cos(randOffset) * radius);
	res.y = pos.y + (std::sin(randOffset) * radius);
	res.z = pos.z + 50;
	return res;
}

CHAOS_VAR Vehicle heli         = 0;
CHAOS_VAR Ped pilot            = 0;
CHAOS_VAR Camera heliCam       = 0;
CHAOS_VAR int lastPositionGoal = 0;
CHAOS_VAR int scaleForm        = 0;
CHAOS_VAR Vector3 targetCoords;

CHAOS_VAR const std::vector<std::string_view> ms_Titles = {
	"",
	"Chaos Mod player trying to survive",
	"A speedrunner in action",
	"Meow",
	"This is you in 10 years",
	"Very bleak yes",
	"Just your average sunday in Ohio",
	"Nothing ever happens",
	"We got this before GTA 6",
	"What do you even want me to say to this?",
	"This is Neoliberalism",
	"Topical reference",
	"Something is happening",
	"Look at this occurance",
	"This is gaming",
	"New news",
	"eCola or Sprunk?",
	"\"He won't survive\", Mod Contributors say",
	"Criticizers claim mods might be at play",
	"Very bleak yes",
	"Nothing ever happens",
	"Look at this occurance",
	"eCola or Sprunk?",
	"Unbelievable",
	"Woof",
	"I love cats",
	"New research indicates deers are cows",
};

static void OnStart()
{
	Ped player                    = PLAYER_PED_ID();
	Vector3 pos                   = GET_ENTITY_COORDS(player, false);

	static const Hash playerGroup = "PLAYER"_hash;
	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_NEWS_TEAM", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(2, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(2, playerGroup, relationshipGroup);

	// Spawn Helicopter
	Hash heliHash = "frogger"_hash;
	LoadModel(heliHash);
	Vector3 aroundCoords;
	float groundZ;
	bool isValidCoord = false;
	do
	{
		aroundCoords = GetCoordsAround(pos, 70);
		isValidCoord =
		    GET_GROUND_Z_FOR_3D_COORD(aroundCoords.x, aroundCoords.y, aroundCoords.z, &groundZ, false, false);
	} while (!isValidCoord);
	groundZ = std::max(groundZ, aroundCoords.z);
	heli    = CREATE_VEHICLE(heliHash, aroundCoords.x, aroundCoords.y, groundZ, 0, true, false, false);
	SET_VEHICLE_ENGINE_ON(heli, true, true, true);
	SET_VEHICLE_FORWARD_SPEED(heli, 0); // Needed, so the heli doesn't fall down instantly
	MODIFY_VEHICLE_TOP_SPEED(heli, 100);
	SET_HELI_BLADES_SPEED(heli, 1);
	WAIT(0);
	// Spawn Pilot
	Hash pilotModel = "csb_reporter"_hash;
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
		WAIT(0);

	BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SET_TEXT");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(
	    ms_Titles[g_RandomNoDeterm.GetRandomInt(0, ms_Titles.size() - 1)].data());
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(
	    ms_Titles[g_RandomNoDeterm.GetRandomInt(0, ms_Titles.size() - 1)].data());

	END_SCALEFORM_MOVIE_METHOD();
}

static void OnStop()
{
	RENDER_SCRIPT_CAMS(false, true, 1000, true, true, true);
	SET_CAM_ACTIVE(heliCam, false);
	DESTROY_CAM(heliCam, true);
	SET_ENTITY_AS_MISSION_ENTITY(pilot, true, true);
	DELETE_ENTITY(&pilot);
	SET_ENTITY_AS_MISSION_ENTITY(heli, true, true);
	DELETE_ENTITY(&heli);
	pilot     = 0;
	heli      = 0;
	scaleForm = 0;
}

static void OnTick()
{
	Ped player       = PLAYER_PED_ID();
	int current_time = GET_GAME_TIMER();
	if (current_time - lastPositionGoal > 1000)
	{
		lastPositionGoal = current_time;
		targetCoords     = GetCoordsAround(Vector3(), g_Random.GetRandomFloat(20, 30));
		TASK_HELI_CHASE(pilot, player, targetCoords.x, targetCoords.y, targetCoords.z + g_Random.GetRandomFloat(5, 10));
		SET_PED_KEEP_TASK(pilot, true);
	}
	if (scaleForm > 0)
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
    {
		.Name = "News Team",
		.Id = "misc_news_team",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Camera
    }
);