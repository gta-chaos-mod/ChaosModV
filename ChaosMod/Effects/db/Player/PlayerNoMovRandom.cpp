/*
	Effect by Moxi
*/

#include <stdafx.h>

static int int_rand_key;

static void OnStart()
{
	int_rand_key = g_random.GetRandomInt(0, 3);
}

static void OnTick()
{
	if (int_rand_key == 0) // Disable Up
	{
		if (IS_CONTROL_PRESSED(0, 32) == true)
		{
			DISABLE_CONTROL_ACTION(0, 31, true);
		}
		if (IS_CONTROL_PRESSED(0, 61) == true)
		{
			DISABLE_CONTROL_ACTION(0, 60, true);
		}
		if (IS_CONTROL_PRESSED(0, 127) == true)
		{
			DISABLE_CONTROL_ACTION(0, 126, true);
		}
		if (IS_CONTROL_PRESSED(0, 150) == true)
		{
			DISABLE_CONTROL_ACTION(0, 149, true);
		}
		DISABLE_CONTROL_ACTION(0, 71, true);
		DISABLE_CONTROL_ACTION(0, 87, true);
		DISABLE_CONTROL_ACTION(0, 129, true);
		DISABLE_CONTROL_ACTION(0, 136, true);
		DISABLE_CONTROL_ACTION(0, 137, true);
		DISABLE_CONTROL_ACTION(0, 280, true);
	}
	if (int_rand_key == 1) // Disable Down
	{
		if (IS_CONTROL_PRESSED(0, 33) == true)
		{
			DISABLE_CONTROL_ACTION(0, 31, true);
		}
		if (IS_CONTROL_PRESSED(0, 62) == true)
		{
			DISABLE_CONTROL_ACTION(0, 60, true);
		}
		if (IS_CONTROL_PRESSED(0, 128) == true)
		{
			DISABLE_CONTROL_ACTION(0, 126, true);
		}
		if (IS_CONTROL_PRESSED(0, 151) == true)
		{
			DISABLE_CONTROL_ACTION(0, 149, true);
		}
		DISABLE_CONTROL_ACTION(0, 72, true);
		DISABLE_CONTROL_ACTION(0, 88, true);
		DISABLE_CONTROL_ACTION(0, 130, true);
		DISABLE_CONTROL_ACTION(0, 138, true);
		DISABLE_CONTROL_ACTION(0, 139, true);
		DISABLE_CONTROL_ACTION(0, 281, true);
	}
	if (int_rand_key == 2) // Disable Left
	{
		if (IS_CONTROL_PRESSED(0, 34) == true)
		{
			DISABLE_CONTROL_ACTION(0, 30, true);
		}
		if (IS_CONTROL_PRESSED(0, 63) == true)
		{
			DISABLE_CONTROL_ACTION(0, 59, true);
		}
		if (IS_CONTROL_PRESSED(0, 124) == true)
		{
			DISABLE_CONTROL_ACTION(0, 123, true);
		}
		if (IS_CONTROL_PRESSED(0, 147) == true)
		{
			DISABLE_CONTROL_ACTION(0, 146, true);
		}
		DISABLE_CONTROL_ACTION(0, 89, true);
		DISABLE_CONTROL_ACTION(0, 133, true);
		DISABLE_CONTROL_ACTION(0, 152, true);
		DISABLE_CONTROL_ACTION(0, 278, true);
	}
	if (int_rand_key == 3) // Disable Right
	{
		if (IS_CONTROL_PRESSED(0, 35) == true)
		{
			DISABLE_CONTROL_ACTION(0, 30, true);
		}
		if (IS_CONTROL_PRESSED(0, 64) == true)
		{
			DISABLE_CONTROL_ACTION(0, 59, true);
		}
		if (IS_CONTROL_PRESSED(0, 125) == true)
		{
			DISABLE_CONTROL_ACTION(0, 123, true);
		}
		if (IS_CONTROL_PRESSED(0, 148) == true)
		{
			DISABLE_CONTROL_ACTION(0, 146, true);
		}
		DISABLE_CONTROL_ACTION(0, 90, true);
		DISABLE_CONTROL_ACTION(0, 134, true);
		DISABLE_CONTROL_ACTION(0, 153, true);
		DISABLE_CONTROL_ACTION(0, 279, true);
	}
}

static RegisterEffect registerEffect(EFFECT_MOV_NO_RANDOM, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Random Movement",
		.Id = "player_no_random_movement",
		.IsTimed = true,
		.IsShortDuration = true
	}
);