#include <stdafx.h>

//Effect by Netux

static void OnStart()
{
	float x, y, _;
	do
	{
		x = g_Random.GetRandomFloat(-3747.f, 4500.f);
		y = g_Random.GetRandomFloat(-4400.f, 8022.f);

	} while (TEST_VERTICAL_PROBE_AGAINST_ALL_WATER(x, y, 0, 0, &_));

	SET_NEW_WAYPOINT(x, y);
	REFRESH_WAYPOINT();
}

static RegisterEffect registerEffect(EFFECT_RANDOM_WAYPOINT, OnStart, EffectInfo
	{
		.Name = "Random Waypoint",
		.Id = "misc_random_waypoint"
	}
);