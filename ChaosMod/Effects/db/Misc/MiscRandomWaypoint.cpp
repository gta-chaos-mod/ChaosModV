#include <stdafx.h>

//Effect by Netux

static void OnStart()
{
	// Taken from PlayerTpController - I assume its in the entire map.
	float x = g_random.GetRandomInt(-3747.f, 4500.f);
	float y = g_random.GetRandomInt(-4400.f, 8022.f);

	SET_NEW_WAYPOINT(x, y);
	REFRESH_WAYPOINT();
}

static RegisterEffect registerEffect(EFFECT_RANDOM_WAYPOINT, OnStart, EffectInfo
	{
		.Name = "Random Waypoint",
		.Id = "misc_random_waypoint"
	}
);