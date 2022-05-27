#include <stdafx.h>

#include "Memory/Vehicle.h"

static void OnTickRed()
{
	for (Vehicle veh : GetAllVehs())
	{
		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 255, 0, 0);

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 0);
		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickRed, EffectInfo
	{
		.Name = "Red Traffic",
		.Id = "vehs_red",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
// clang-format on

static void OnTickBlue()
{
	for (Vehicle veh : GetAllVehs())
	{
		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 0, 0, 255);

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 255);
		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickBlue, EffectInfo
	{
		.Name = "Blue Traffic",
		.Id = "vehs_blue",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
// clang-format on

static void OnTickGreen()
{
	for (Vehicle veh : GetAllVehs())
	{
		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 0, 255, 0);

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 255, 0);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 255, 0);
		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickGreen, EffectInfo
	{
		.Name = "Green Traffic",
		.Id = "vehs_green",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
// clang-format on

static void OnTickChrome()
{
	for (Vehicle veh : GetAllVehs())
	{
		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 219, 226, 233);

		// If the vehicle has a custom color, the effect won't work
		CLEAR_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh);
		CLEAR_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh);

		SET_VEHICLE_COLOURS(veh, 120, 120);
		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickChrome, EffectInfo
	{
		.Name = "Chrome Traffic",
		.Id = "vehs_chrome",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
// clang-format on

static std::map<Vehicle, int> flameByCar;

static void OnStopPink()
{
	for (auto it = flameByCar.cbegin(); it != flameByCar.cend();)
	{
		int animHandle = it->second;
		STOP_PARTICLE_FX_LOOPED(animHandle, false);
		it++;
	}
	flameByCar.clear();
	REMOVE_NAMED_PTFX_ASSET("des_trailerpark");
}

static void OnTickPink()
{
	REQUEST_NAMED_PTFX_ASSET("des_trailerpark");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("des_trailerpark"))
	{
		WAIT(0);
	}
	for (auto it = flameByCar.cbegin(); it != flameByCar.cend();)
	{
		Vehicle veh = it->first;
		if (!DOES_ENTITY_EXIST(veh))
		{
			STOP_PARTICLE_FX_LOOPED(it->second, false);
			flameByCar.erase(it++);
		}
		else
		{
			it++;
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (flameByCar.find(veh) == flameByCar.end())
		{
			USE_PARTICLE_FX_ASSET("des_trailerpark");
			int boneId = GET_ENTITY_BONE_INDEX_BY_NAME(veh, "chassis");
			int handle = START_PARTICLE_FX_LOOPED_ON_ENTITY_BONE("ent_ray_trailerpark_fires", veh, 0, 0, 0, 0, 0, 0,
			                                                     boneId, 1, false, false, false);
			flameByCar[veh] = handle;
		}

		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, 255, 0, 255);

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 255);
		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStopPink, OnTickPink, EffectInfo
	{
		.Name = "Hot Traffic",
		.Id = "vehs_pink",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
// clang-format on

static void OnStopRainbow()
{
	for (int i = 0; i < 13; i++)
	{
		Memory::OverrideVehicleHeadlightColor(i, false, 0, 0, 0);
	}
}

static void OnTickRainbow()
{
	static int headlightColor = 0;

	static ULONG cnt          = 0;
	static const float freq   = .1f;

	if (++cnt >= (ULONG)-1)
	{
		cnt = 0;
	}

	for (Vehicle veh : GetAllVehs())
	{
		int r = std::sin(veh + freq * cnt) * 127 + 128;
		int g = std::sin(veh + freq * cnt + 2) * 127 + 128;
		int b = std::sin(veh + freq * cnt + 4) * 127 + 128;

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, g, b, r);

		// Neon lights

		_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, r, g, b);
		for (int i = 0; i < 4; i++)
		{
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
		}

		// Headlights too

		TOGGLE_VEHICLE_MOD(veh, 22, true);
		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, headlightColor);

		TOGGLE_VEHICLE_MOD(veh, 20, true); // Enable custom tyre smoke
		SET_VEHICLE_TYRE_SMOKE_COLOR(veh, r, g, b);

		SET_VEHICLE_ENVEFF_SCALE(veh, 0.f);
		SET_VEHICLE_DIRT_LEVEL(veh, 0.f);
	}

	// Headlight color switcher

	int r = std::sin(freq * cnt) * 127 + 128;
	int g = std::sin(freq * cnt + 2) * 127 + 128;
	int b = std::sin(freq * cnt + 4) * 127 + 128;

	for (int i = 0; i < 13; i++)
	{
		Memory::OverrideVehicleHeadlightColor(i, true, r, g, b);
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStopRainbow, OnTickRainbow, EffectInfo
	{
		.Name = "Rainbow Traffic",
		.Id = "vehs_rainbow",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::TrafficColor,
		.EffectGroupType = EEffectGroupType::TrafficColor
	}
);
