#include <stdafx.h>

//Effect by ProfessorBiddle, suggested by JustTunerYT

static void OnTick()
{
	static auto lastTick = GetTickCount64();
	auto curTick = GetTickCount64();

	if (lastTick < curTick - 5000)
	{
		Ped playerPed = PLAYER_PED_ID();
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);
		Vehicle veh = CreatePoolVehicle(GET_HASH_KEY("BUFFALO"), playerPos.x, playerPos.y + 1.f, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
		SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 255, 255);
		SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 255, 255);
		SET_PED_INTO_VEHICLE(playerPed, veh, -1);
		lastTick = curTick;
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_GTA_RACING, nullptr, nullptr, OnTick);