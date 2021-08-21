#include <stdafx.h>

//Effect by ProfessorBiddle

static void OnTick()
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (curTick > lastTick + 500)
	{
		lastTick = curTick;

		Vector3 spawnPos;
		spawnPos.x = playerPos.x + g_Random.GetRandomInt(-100, 100);
		spawnPos.y = playerPos.y + g_Random.GetRandomInt(-100, 100);
		spawnPos.z = playerPos.z + g_Random.GetRandomInt(25, 50);


		static std::vector<Hash> vehModels = Memory::GetAllVehModels();

		if (!vehModels.empty())
		{
			Vehicle veh = CreateTempVehicle(vehModels[g_Random.GetRandomInt(0, vehModels.size() - 1)], spawnPos.x, spawnPos.y, spawnPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

			// Also apply random upgrades
			SET_VEHICLE_MOD_KIT(veh, 0);
			for (int i = 0; i < 50; i++)
			{
				int max = GET_NUM_VEHICLE_MODS(veh, i);
				SET_VEHICLE_MOD(veh, i, max > 0 ? g_Random.GetRandomInt(0, max - 1) : 0, g_Random.GetRandomInt(0, 1));

				TOGGLE_VEHICLE_MOD(veh, i, g_Random.GetRandomInt(0, 1));
			}

			SET_VEHICLE_TYRES_CAN_BURST(veh, g_Random.GetRandomInt(0, 1));
			SET_VEHICLE_WINDOW_TINT(veh, g_Random.GetRandomInt(0, 6));
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_VEHICLE_RAIN, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Vehicle Rain",
		.Id = "misc_vehicle_rain",
		.IsTimed = true,
		.IsShortDuration = true
	}
);