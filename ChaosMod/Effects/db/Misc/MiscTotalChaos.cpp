#include <stdafx.h>

static void OnStart()
{
	SET_WEATHER_TYPE_OVERTIME_PERSIST("THUNDER", 2.f);
}

static void OnStop()
{
	CLEAR_WEATHER_TYPE_PERSIST();
	SET_WEATHER_TYPE_NOW("EXTRASUNNY");
}

static void OnTick()
{
	static constexpr int EXPLOSIONS_PER_SEC = 5;

	auto playerPed = PLAYER_PED_ID();
	auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

	for (auto veh : GetAllVehs())
	{
		if (veh != playerVeh)
		{
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(veh, 0, 50.f, .0f, .0f, true, true, true, true);
		}
	}

	for (auto prop : GetAllProps())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(prop, 0, 50.f, .0f, .0f, true, true, true, true);
	}

	for (int i = 0; i < EXPLOSIONS_PER_SEC; i++)
	{
		float x = GET_RANDOM_INT_IN_RANGE(0, 1) ? GET_RANDOM_FLOAT_IN_RANGE(20.f, 50.f) : GET_RANDOM_FLOAT_IN_RANGE(-50.f, -20.f);
		float y = GET_RANDOM_FLOAT_IN_RANGE(-50.f, 50.f);
		float z = GET_RANDOM_FLOAT_IN_RANGE(.0f, 50.f);
		auto pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, x, y, z);

		ADD_EXPLOSION(pos.x, pos.y, pos.z, 8, 1.f, false, true, .2f, false);
	}
}

static RegisterEffect registerEffect(EFFECT_TOTAL_CHAOS, OnStart, OnStop, OnTick);