/*
    Effect by Slothersbee, modified
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
// clang-format off
CHAOS_VAR std::vector<std::string> actions = {
	"hold forwards",
	"hold backwards",
	"hold left",
	"hold right",
	"hold any movement key",
	"hold look behind (c)"
};

CHAOS_VAR std::map<std::string, std::vector<int>> actionKeys = {
	{"hold forwards", {32, 71, 77, 87, 129, 136, 150, 232}},
	{"hold backwards", {33, 72, 78, 88, 130, 139, 151, 233}},
	{"hold left", {34, 63, 89, 133, 147, 234}},
	{"hold right", {35, 64, 90, 134, 148, 235}},
	{"hold any movement key", {
		32, 71, 77, 87, 129, 136, 150, 232,
		33, 72, 78, 88, 130, 139, 151, 233,
		34, 63, 89, 133, 147, 234,
		35, 64, 90, 134, 148, 235,
		44, 23, 102, 55,
	}},
	{"hold look behind (c)", {26, 79}}
};
// clang-format on

CHAOS_VAR int scaleForm = 0;
CHAOS_VAR int lastTime  = 0;
CHAOS_VAR int waitTime  = 2000;
CHAOS_VAR bool dead     = false;

CHAOS_VAR std::string action;

CHAOS_VAR bool opposite;

static void ShowPopup()
{
	std::string message;

	if (opposite)
		if (g_Random.GetRandomInt(0, 1) == 0)
			message = "Simeon Says: Don't ";
		else
			message = "";
	else if (g_Random.GetRandomInt(0, 1) == 0)
		message = "Don't ";
	else
		message = "Simeon Says: ";

	message.append(action);

	scaleForm = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
		WAIT(0);
	lastTime = GetTickCount64();
	waitTime = 2000;
	SET_TIME_SCALE(0.1f);
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleForm, "SHOW_SHARD_RANKUP_MP_MESSAGE");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(message.c_str());
	END_SCALEFORM_MOVIE_METHOD();

	while (GetTickCount64() - lastTime < waitTime)
	{
		WAIT(0);
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 0);
	}
	SET_TIME_SCALE(1);
}

static void OnStart()
{
	action       = actions[g_Random.GetRandomInt(0, actions.size() - 1)];

	int rand_int = g_Random.GetRandomInt(0, 1);

	if (rand_int == 0)
		opposite = false;
	else
		opposite = true;

	lastTime = 0;
	dead     = false;

	ShowPopup();
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	bool kill     = false;
	if (!opposite)
	{
		if (_IS_PLAYER_CAM_CONTROL_DISABLED())
			return;
		kill = true;

		for (int key : actionKeys[action])
			if (IS_CONTROL_PRESSED(0, key) || IS_CONTROL_JUST_PRESSED(0, key))
				kill = false;
	}
	else
	{
		for (int key : actionKeys[action])
			if (IS_CONTROL_PRESSED(0, key) || IS_CONTROL_JUST_RELEASED(0, key))
				kill = true;
	}

	if (IS_PED_DEAD_OR_DYING(playerPed, false) || !IS_SCREEN_FADED_IN())
	{
		dead = true;
	}
	else if (dead)
	{
		ShowPopup();

		dead = false;
	}
	else if (kill)
	{
		Vector3 pos = GET_ENTITY_COORDS(playerPed, false);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
		ADD_EXPLOSION(pos.x, pos.y, pos.z, 9, 100.f, true, false, 3.f, false);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Simeon Says",
		.Id = "player_simeonsays",
		.IsTimed = true,
		.IsShortDuration = true
	}
);