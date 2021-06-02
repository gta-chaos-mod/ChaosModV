/*
	Effect by Last0xygen
*/

#include <stdafx.h>

enum TiredMode {
	closingEyes,
	openingEyes,
	waiting,
};
static TiredMode currentMode = TiredMode::closingEyes;
static float alpha;
static int closingIterator;
static int nextTimestamp;
static float steeringDirection;

static void BlackOut(float alpha)
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, alpha, false);
}

static void SteerVehicle()
{
	Ped playerPed = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_VEHICLE_STEER_BIAS(veh, steeringDirection);
	}
}

static void RagdollOnFoot()
{
	Ped playerPed = PLAYER_PED_ID();
	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		int sleepDuration = 3000;
		SET_PED_TO_RAGDOLL(playerPed, sleepDuration, sleepDuration, 0, true, true, false);
	}
}

static void OnStop()
{
	BlackOut(0);
}

static void OnStart()
{
	currentMode = TiredMode::closingEyes;
	alpha = 0;
	closingIterator = 20;
}

static void OnTick()
{
	switch (currentMode)
	{
	case closingEyes:
		alpha += closingIterator;
		// Chance for player who's on foot to ragdoll halfway through blinking
		if (alpha / closingIterator == floor(255.f / closingIterator / 2.f) && g_Random.GetRandomFloat(0.f, 1.f) < .25f)
		{
			RagdollOnFoot();
		}
		// Fall asleep at the wheel near the end of blinking
		if (alpha > 200.f)
		{
			SteerVehicle();
		}
		if (alpha >= 255)
		{
			currentMode = TiredMode::openingEyes;
			nextTimestamp = GET_GAME_TIMER() + ((20 - closingIterator) * 20);
			if (closingIterator > 1)
			{
				closingIterator = max(1, closingIterator - 2);
			}
		}
		break;
	case TiredMode::openingEyes:
		if (GET_GAME_TIMER() > nextTimestamp)
		{
			alpha -= 30;
			if (alpha <= 0)
			{
				alpha = 0;
				currentMode = TiredMode::waiting;
				nextTimestamp = GET_GAME_TIMER() + g_Random.GetRandomInt(250, 3000);
			}
		}
		break;
	case TiredMode::waiting:
		if (GET_GAME_TIMER() > nextTimestamp)
		{
			currentMode = TiredMode::closingEyes;
			steeringDirection = (g_Random.GetRandomFloat(0, 1) < .5f) ? 1.0f : -1.0f;
		}
		break;
	}
	BlackOut(alpha);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_TIRED, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "I'm So Tired",
		.Id = "player_tired",
		.IsTimed = true
	}
);