#include <stdafx.h>

static void BlackOut(int R, int G, int B, float alpha)
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, R, G, B, alpha, false);
}

static void OnTick()
{
	BlackOut(150, 0, 0, 100);
	Ped player = PLAYER_PED_ID();

	if (IS_ENTITY_ON_FIRE(player))
	{
		int playerHealth = GET_ENTITY_HEALTH(player);
		SET_ENTITY_HEALTH(player, playerHealth + 20, playerHealth + 20);
	}

	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
		TASK_LEAVE_VEHICLE(player, playerVeh, 4160);
	}

	if (!IS_ENTITY_ON_FIRE(player) && !IS_PED_IN_ANY_VEHICLE(player, true))
	{
		START_ENTITY_FIRE(player);
	}
}

static void OnStop()
{
	Ped player = PLAYER_PED_ID();
	BlackOut(150, 0, 0, 0);
	if (IS_ENTITY_ON_FIRE(player))
	{
		STOP_ENTITY_FIRE(player);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_SUFFER, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Suffering",
		.Id = "player_suffer",
		.IsTimed = true,
		.IsShortDuration = true
	}
);