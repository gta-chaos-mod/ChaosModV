/*
	Effect by Slothersbee, modified
*/

#include <stdafx.h>

static std::vector<std::string> actions = {
	"move forwards",
	"move backwards",
	"move left",
	"move right",
	"move",
	"look behind (c)"
};

static std::map<std::string, std::vector<int>> actionKeys = {
	{"move forwards", {32, 71, 77, 87, 129, 136, 150, 232}},
	{"move backwards", {33, 72, 78, 88, 130, 139, 151, 233}},
	{"move left", {34, 63, 89, 133, 147, 234}},
	{"move right", {35, 64, 90, 134, 148, 235}},
	{"move", {
		32, 71, 77, 87, 129, 136, 150, 232,
		33, 72, 78, 88, 130, 139, 151, 233,
		34, 63, 89, 133, 147, 234,
		35, 64, 90, 134, 148, 235,
		44, 23, 102, 55,
	}},
	{"look behind (c)", {26, 79}}
};

static std::map<std::string, std::vector<int>> blacklistedKeys = {
	{"move forwards", {33, 72, 78, 88, 130, 139, 151, 233}},
	{"move backwards", {32, 71, 77, 87, 129, 136, 150, 232}},
	{"move left", {35, 64, 90, 134, 148, 235}},
	{"move right", {34, 63, 89, 133, 147, 234}},
	{"move", {}},
	{"look behind (c)", {}}
};

static int scaleForm = 0;
static int lastTime = 0;
static int waitTime = 2000;

static std::string action;

static bool opposite;

static void OnStart()
{
	action = actions[g_random.GetRandomInt(0, actions.size() - 1)];
	
	int rand_int = g_random.GetRandomInt(0, 1);

	std::string message = "Simeon Says: ";
	
	if (rand_int == 0)
	{
		opposite = false;
	}
	else
	{
		opposite = true;
		message.append("Don't ");
	}

	message.append(action);

	scaleForm = 0;
	lastTime = 0;

	scaleForm = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleForm))
	{
		WAIT(0);
	}
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
	scaleForm = 0;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();

	bool kill = false;
	if (!opposite)
	{
		if (_IS_PLAYER_CAM_CONTROL_DISABLED())
		{
			return;
		}
		kill = true;
		
		for (int key : actionKeys[action])
		{
			if (IS_CONTROL_PRESSED(0, key) || IS_CONTROL_JUST_PRESSED(0, key))
			{
				kill = false;
			}
		}
		for (int key : blacklistedKeys[action])
		{
			if (IS_CONTROL_PRESSED(0, key) || IS_CONTROL_JUST_PRESSED(0, key))
			{
				kill = true;
			}
		}
	}
	else
	{
		for (int key : actionKeys[action])
		{
			if (IS_CONTROL_PRESSED(0, key) || IS_CONTROL_JUST_RELEASED(0, key))
			{
				kill = true;
			}
		}
	}

	if (kill && !IS_PED_DEAD_OR_DYING(playerPed, false))
	{
		Vector3 pos = GET_ENTITY_COORDS(playerPed, false);
		SET_ENTITY_HEALTH(playerPed, 0, 0);
		ADD_EXPLOSION(pos.x, pos.y, pos.z, 9, 100.f, true, false, 3.f, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SIMEONSAYS, OnStart, nullptr, OnTick);
