#include <stdafx.h>

static void OnStartMaxUpgrades()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_MOD_KIT(veh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			if (max > 0)
			{
				SET_VEHICLE_MOD(veh, i, max - 1, true);
			}

			TOGGLE_VEHICLE_MOD(veh, i, true);
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, false);
		SET_VEHICLE_WINDOW_TINT(veh, 1);

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));

		_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));
		for (int i = 0; i < 4; i++)
		{
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
		}

		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, g_Random.GetRandomInt(0, 12));

		WAIT(0);
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_MAX_UPGRADES, OnStartMaxUpgrades, EffectInfo
	{
		.Name = "Add Max Upgrades To Every Vehicle",
		.Id = "playerveh_maxupgrades"
	}
);
static void OnStartRandomUpgrades()
{
	for (Vehicle veh : GetAllVehs())
	{
		SET_VEHICLE_MOD_KIT(veh, 0);

		SET_VEHICLE_WHEEL_TYPE(veh, g_Random.GetRandomInt(0, 7));

		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(veh, i);
			if (max > 0)
			{
				SET_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, max - 1), g_Random.GetRandomInt(0, 1));
			}

			TOGGLE_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(veh, g_Random.GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(veh, g_Random.GetRandomInt(0, 6));

		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));

		_SET_VEHICLE_NEON_LIGHTS_COLOUR(veh, g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255), g_Random.GetRandomInt(0, 255));
		for (int i = 0; i < 4; i++)
		{
			_SET_VEHICLE_NEON_LIGHT_ENABLED(veh, i, true);
		}

		_SET_VEHICLE_XENON_LIGHTS_COLOR(veh, g_Random.GetRandomInt(0, 12));

		WAIT(0);
	}
}

static RegisterEffect registerEffect2(EFFECT_VEH_RANDOM_UPGRADES, OnStartRandomUpgrades, EffectInfo
	{
		.Name = "Add Random Upgrades To Every Vehicle",
		.Id = "playerveh_randupgrades"
	}
);