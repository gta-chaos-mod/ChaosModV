/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

Ped policePed;
Vehicle veh;

static void OnStart()
{
	Hash policeCar = 0x79FBB0C5;
	Ped playerPed = PLAYER_PED_ID();

	Vector3 policeStation = {495, -1020, 27};

	//Possible peds, (male or female)
	std::vector<Hash> policePeds = { 0x15F8700D,  0x5E3DA4A4 };

	float heading = GET_ENTITY_HEADING(playerPed);
	Vector3 pos = GET_ENTITY_COORDS(playerPed, false);

	//Get a random police ped
	Hash randomPed = policePeds[g_Random.GetRandomInt(0, 1)];

	//Create the police car
	veh = CREATE_VEHICLE(policeCar, pos.x, pos.y, pos.z, heading, true, true, true);
	//Create the police ped
	policePed = CREATE_PED_INSIDE_VEHICLE(veh, 4, randomPed, -1, true, true);
	//Put the player in the police car
	SET_PED_INTO_VEHICLE(playerPed, veh, 1);
	//Turn on the police siren
	SET_VEHICLE_SIREN(veh, 1);

	//Have the police car drive to the police station
	TASK_VEHICLE_DRIVE_TO_COORD(policePed, veh, policeStation.x, policeStation.y, policeStation.z, 40, 1, policeCar, 6, 1, true);
	SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(policePed, true);
	SET_PED_KEEP_TASK(policePed, 1);
}



// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EFFECT_MISC_GO_TO_JAIL, OnStart, nullptr, nullptr, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Go Straight To Jail",
		.Id = "misc_go_to_jail"
	}
);
