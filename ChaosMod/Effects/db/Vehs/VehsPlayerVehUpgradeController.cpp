#include <stdafx.h>

static void OnStartMaxUpgrades()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		SET_VEHICLE_MOD_KIT(playerVeh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(playerVeh, i);
			SET_VEHICLE_MOD(playerVeh, i, max > 0 ? max - 1 : 0, true);

			TOGGLE_VEHICLE_MOD(playerVeh, i, true);
		}

		SET_VEHICLE_TYRES_CAN_BURST(playerVeh, false);
		SET_VEHICLE_WINDOW_TINT(playerVeh, 1);
	}
}

static RegisterEffect registerEffect1(EFFECT_VEH_MAX_UPGRADES, OnStartMaxUpgrades);

static void OnStartRandomUpgrades()
{
	auto playerPed = PLAYER_PED_ID();

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		auto playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);

		SET_VEHICLE_MOD_KIT(playerVeh, 0);
		for (int i = 0; i < 50; i++)
		{
			int max = GET_NUM_VEHICLE_MODS(playerVeh, i);
			SET_VEHICLE_MOD(playerVeh, i, max > 0 ? Random::GetRandomInt(0, max - 1) : 0, Random::GetRandomInt(0, 1));

			TOGGLE_VEHICLE_MOD(playerVeh, i, Random::GetRandomInt(0, 1));
		}

		SET_VEHICLE_TYRES_CAN_BURST(playerVeh, Random::GetRandomInt(0, 1));
		SET_VEHICLE_WINDOW_TINT(playerVeh, Random::GetRandomInt(0, 6));
	}
}

static RegisterEffect registerEffect2(EFFECT_VEH_RANDOM_UPGRADES, OnStartRandomUpgrades);