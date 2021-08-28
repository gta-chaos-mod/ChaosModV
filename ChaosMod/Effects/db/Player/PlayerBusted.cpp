#include <stdafx.h>

static bool isActive = false;

static void BarFrame()
{
	DRAW_RECT(.0f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.1f, .5f, .08f, 1.f, 150, 150, 150, 255, false);
	DRAW_RECT(.2f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.3f, .5f, .08f, 1.f, 150, 150, 150, 255, false);
	DRAW_RECT(.4f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.5f, .5f, .08f, 1.f, 150, 150, 150, 255, false);
	DRAW_RECT(.6f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.7f, .5f, .08f, 1.f, 150, 150, 150, 255, false);
	DRAW_RECT(.8f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.9f, .5f, .08f, 1.f, 150, 150, 150, 255, false);
	DRAW_RECT(1.f, .5f, .12f, 1.f, 0, 0, 0, 255, false);
}

static void OnStart()
{
	isActive = false;
}

static void OnTick()
{
	Player playerID = PLAYER_ID();
	Ped player = PLAYER_PED_ID();
	int wantedLevel = GET_PLAYER_WANTED_LEVEL(playerID);
	static bool isStarted = false;
	DWORD64 curTick = GET_GAME_TIMER();
	static DWORD64 lastTick = 0;

	if (wantedLevel > 0)
	{
		isActive = true;
		isStarted = false;
		lastTick = curTick;
	}

	if (isActive)
	{
		REMOVE_ALL_PED_WEAPONS(player, false);
		if (IS_PED_IN_ANY_VEHICLE(player, true))
		{
			SET_PED_TO_RAGDOLL(player, 100, 100, 0, true, true, false);
		}
		SET_ENTITY_INVINCIBLE(player, true);
		HIDE_HUD_AND_RADAR_THIS_FRAME();
		TASK_HANDS_UP(player, 1000, 0, -1, true);
		UPDATE_TASK_HANDS_UP_DURATION(player, 1000);
		SET_PLAYER_WANTED_LEVEL(playerID, 0, false);
		SET_PLAYER_WANTED_LEVEL_NOW(playerID, false);
		if (curTick > lastTick + 2000)
		{
			lastTick = curTick;
			isStarted = true;
		}
		if (isStarted)
		{
			BarFrame();
			TeleportPlayer(436.491f, -982.172f, 30.699f);
		}
	}
}

static void OnStop()
{
	if (isActive)
	{
		Ped player = PLAYER_PED_ID();
		CLEAR_PED_TASKS_IMMEDIATELY(player);
		SET_ENTITY_INVINCIBLE(player, false);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BUSTED, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Jailtime",
		.Id = "player_busted",
		.IsTimed = true
	}
);