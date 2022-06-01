/*
    Effect by Gorakh
*/

#include <stdafx.h>

struct PropData
{
	float originalZ;
	int startOffset;
};

static std::map<Object, PropData> propDataMap = {};

static void OnTick()
{
	for (Object prop : GetAllProps())
	{
		Vector3 coords = GET_ENTITY_COORDS(prop, 0);

		if (!propDataMap.contains(prop))
		{
			propDataMap[prop] = PropData { coords.z, GET_GAME_TIMER() + prop };
		}

		PropData data = propDataMap[prop];

		float Z = data.originalZ + (std::max(std::sin((GET_GAME_TIMER() - data.startOffset) / 150.f) * 2.5f, 0.f));

		SET_ENTITY_COORDS(prop, coords.x, coords.y, Z, false, false, false, false);
	}
}

static void OnStop()
{
	for (auto propData : propDataMap)
	{
		Object prop   = propData.first;
		PropData data = propData.second;

		if (prop && DOES_ENTITY_EXIST(prop))
		{
			Vector3 coords = GET_ENTITY_COORDS(prop, 0);
			SET_ENTITY_COORDS(prop, coords.x, coords.y, data.originalZ, false, false, false, false);
		}
	}

	propDataMap.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Jumpy Props",
		.Id = "misc_jumpy_props",
		.IsTimed = true
	}
);