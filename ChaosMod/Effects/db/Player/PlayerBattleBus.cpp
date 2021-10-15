// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
/*
	Effect by <Your Name>
*/

#include <stdafx.h>


static Vehicle veh;
static int busMaxSeats;

static void OnStart()
{
	Hash bus = GET_HASH_KEY("bus");
	LoadModel(bus);
	veh = CREATE_VEHICLE(bus, -418.045f, -1227.783f, 484.594f, 341.00f, true, false, false);
	busMaxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(bus);
	SET_VEHICLE_GRAVITY(veh, false);
	Ped player = PLAYER_PED_ID();
	SET_PED_INTO_VEHICLE(player, veh, 0);
	SET_VEHICLE_FORWARD_SPEED(veh, 40);
	static const Hash parachuteHash = GET_HASH_KEY("GADGET_PARACHUTE");
	GIVE_WEAPON_TO_PED(player, parachuteHash, 1, true, false);
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			if(ARE_ANY_VEHICLE_SEATS_FREE(veh))
			{
				for (int i = -1; i < busMaxSeats - 1; i++)
				{
					if (IS_VEHICLE_SEAT_FREE(veh, i, false))
					{
						SET_PED_INTO_VEHICLE(ped, veh, i);
						break;
					}
				}
				
			}
		}
	}
}

static void OnStop()
{
	SET_VEHICLE_GRAVITY(veh, true);
}

static void OnTick()
{
	SET_VEHICLE_FORWARD_SPEED(veh, 55);
	std::vector<Ped> vehPeds;
	for (int i = -1; i < busMaxSeats - 1; i++)
	{
		if (IS_VEHICLE_SEAT_FREE(veh, i, false))
		{
			continue;
		}
		Ped ped = GET_PED_IN_VEHICLE_SEAT(veh, i, false);
		vehPeds.push_back(ped);
	}

	if (vehPeds.size() > 0) {
		if (g_Random.GetRandomInt(0, 130) == 1) {
			Ped randomped = vehPeds[g_Random.GetRandomInt(0, vehPeds.size() - 1)];
			if(!IS_PED_A_PLAYER(randomped)) {
				TASK_LEAVE_VEHICLE(randomped, veh, 4160);
			}
		}
	}
	
}

static RegisterEffect registerEffect(EFFECT_BATTLE_BUS, OnStart, OnStop, OnTick, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Where we droppin?",
		.Id = "player_battle_bus",
		.IsTimed = true
	}
);