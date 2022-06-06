/*
    Effect by Moxi
*/

#include <stdafx.h>

static int rand_key;

static void OnStart()
{
	rand_key = g_Random.GetRandomInt(0, 3);
}

static void OnTick()
{
	if (rand_key == 0) // Disable Up
	{
		if (IS_CONTROL_PRESSED(0, 32))
		{
			DISABLE_CONTROL_ACTION(0, 31, true);
		}
		if (IS_CONTROL_PRESSED(0, 61))
		{
			DISABLE_CONTROL_ACTION(0, 60, true);
		}
		if (IS_CONTROL_PRESSED(0, 127))
		{
			DISABLE_CONTROL_ACTION(0, 126, true);
		}
		if (IS_CONTROL_PRESSED(0, 150))
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
	else if (rand_key == 1) // Disable Down
	{
		if (IS_CONTROL_PRESSED(0, 33))
		{
			DISABLE_CONTROL_ACTION(0, 31, true);
		}
		if (IS_CONTROL_PRESSED(0, 62))
		{
			DISABLE_CONTROL_ACTION(0, 60, true);
		}
		if (IS_CONTROL_PRESSED(0, 128))
		{
			DISABLE_CONTROL_ACTION(0, 126, true);
		}
		if (IS_CONTROL_PRESSED(0, 151))
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
	else if (rand_key == 2) // Disable Left
	{
		if (IS_CONTROL_PRESSED(0, 34))
		{
			DISABLE_CONTROL_ACTION(0, 30, true);
		}
		if (IS_CONTROL_PRESSED(0, 63))
		{
			DISABLE_CONTROL_ACTION(0, 59, true);
		}
		if (IS_CONTROL_PRESSED(0, 124))
		{
			DISABLE_CONTROL_ACTION(0, 123, true);
		}
		if (IS_CONTROL_PRESSED(0, 147))
		{
			DISABLE_CONTROL_ACTION(0, 146, true);
		}
		DISABLE_CONTROL_ACTION(0, 89, true);
		DISABLE_CONTROL_ACTION(0, 133, true);
		DISABLE_CONTROL_ACTION(0, 152, true);
		DISABLE_CONTROL_ACTION(0, 278, true);
	}
	else // Disable Right
	{
		if (IS_CONTROL_PRESSED(0, 35))
		{
			DISABLE_CONTROL_ACTION(0, 30, true);
		}
		if (IS_CONTROL_PRESSED(0, 64))
		{
			DISABLE_CONTROL_ACTION(0, 59, true);
		}
		if (IS_CONTROL_PRESSED(0, 125))
		{
			DISABLE_CONTROL_ACTION(0, 123, true);
		}
		if (IS_CONTROL_PRESSED(0, 148))
		{
			DISABLE_CONTROL_ACTION(0, 146, true);
		}
		DISABLE_CONTROL_ACTION(0, 90, true);
		DISABLE_CONTROL_ACTION(0, 134, true);
		DISABLE_CONTROL_ACTION(0, 153, true);
		DISABLE_CONTROL_ACTION(0, 279, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Disable Random Direction",
		.Id = "player_no_random_movement",
		.IsTimed = true
	}
);