/*
	Effect by Vladimir Gotsin
*/

#include <stdafx.h>

static void OnStart()
{
	//vehicle
	auto playerPed = PLAYER_PED_ID();
	float playerHeading = GET_ENTITY_HEADING(playerPed);
	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	static const Hash caddyModel = GET_HASH_KEY("CADDY");

	Vehicle veh = CREATE_VEHICLE(caddyModel, playerPos.x, playerPos.y, playerPos.z, heading, true, true, true);

	//removing Vehicle
	Vehicle toRemove = GET_VEHICLE_PED_IS_IN(playerPed, false);
	SET_ENTITY_AS_MISSION_ENTITY(toRemove, true, true);
	DELETE_VEHICLE(&toRemove);

	//setting player into new vehicle
	SET_PED_INTO_VEHICLE(playerPed, veh, -1);

	//clothes
	int pedType = GET_PED_TYPE(playerPed);

	if (pedType == 0)
	{
		//Michael
		SET_PED_COMPONENT_VARIATION(playerPed, 0, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 2, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 3, 16, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 4, 15, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 5, 3, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 6, 1, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 8, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 11, 0, 0, 0);

		SET_PED_PROP_INDEX(playerPed, 0, 8, 0, false);
	}
	else if (pedType == 1) 
	{
		//Franklin
		SET_PED_COMPONENT_VARIATION(playerPed, 0, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 2, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 3, 6, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 4, 6, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 5, 2, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 6, 5, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 8, 14, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 11, 8, 0, 0);
	}
	else if (pedType == 3)
	{
		//Trevor
		SET_PED_COMPONENT_VARIATION(playerPed, 0, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 1, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 2, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 3, 11, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 4, 11, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 5, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 6, 1, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 7, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 8, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 9, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 10, 0, 0, 0);
		SET_PED_COMPONENT_VARIATION(playerPed, 11, 0, 0, 0);
	}

	//weapon
	REMOVE_ALL_PED_WEAPONS(playerPed, false);
	Hash weaponHash = GET_HASH_KEY("WEAPON_GOLFCLUB");
	GIVE_WEAPON_TO_PED(playerPed, weaponHash, 1, false, true);
}

static void OnStop()
{

}

static void OnTick()
{

}

static RegisterEffect registerEffect(EFFECT_GOLF_PERCENT, OnStart, OnStop, OnTick);