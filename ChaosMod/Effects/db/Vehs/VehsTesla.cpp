/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
#include <algorithm>

struct TeslaPair
{
	Ped ped;
	Vehicle vehicle;
};

enum PlayerState : int
{
	NONE,
	WANDER, 
	WAYPOINT
};

static PlayerState playerState = PlayerState::NONE;

std::vector<TeslaPair> pairs;

static Vector3 waypointCoords;
static bool m_foundWaypoint;
static bool m_customWaypoint;

bool pairsContains(const TeslaPair& src)
{
	auto iter = std::find_if(pairs.begin(), pairs.end(),
		[&](const TeslaPair& ts) {return ts.ped == src.ped && ts.vehicle == src.vehicle;});
	return iter != pairs.end();
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	//AI Auto-Pilot
	for (Vehicle v : GetAllVehs())
	{
		Ped p = GET_PED_IN_VEHICLE_SEAT(v, -1, false);
		if (pairsContains(TeslaPair(p,v))) continue;
		if (p == playerPed) continue;
		if (DOES_ENTITY_EXIST(p))
		{
			pairs.push_back(TeslaPair(p, v));
		}
	}

	for (int i = 0; i < pairs.size(); i++)
	{
		TeslaPair pr = pairs.at(i);
		Ped p = pr.ped;
		Vehicle v = pr.vehicle;
		if (GET_PED_IN_VEHICLE_SEAT(v, -1, false) == p)
		{
			SET_ENTITY_ALPHA(p, 0, false);
		}
		else
		{
			RESET_ENTITY_ALPHA(p);
			pairs.erase(pairs.begin() + i);
		}
	}

	//Player Auto-Pilot, 
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle vehicle = GET_VEHICLE_PED_IS_IN(playerPed, false);
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);
		if (playerState == PlayerState::NONE)
		{
			if (IS_WAYPOINT_ACTIVE())
			{
				waypointCoords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));

				// Player waypoint doesn't have proper z coord, use player z coord instead
				waypointCoords.z = playerPos.z;

				playerState = PlayerState::WAYPOINT;
				TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, vehicle, waypointCoords.x, waypointCoords.y, waypointCoords.z, 9999.f, 262668, .0f);
			}
			else
			{
				playerState = PlayerState::WANDER;
				TASK_VEHICLE_DRIVE_WANDER(playerPed, vehicle, 9999.f, 262668);
			}
		}
		else if (playerState == PlayerState::WANDER)
		{
			if (IS_WAYPOINT_ACTIVE())
			{
				waypointCoords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));

				// Player waypoint doesn't have proper z coord, use player z coord instead
				waypointCoords.z = playerPos.z;

				playerState = PlayerState::WAYPOINT;
				TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, vehicle, waypointCoords.x, waypointCoords.y, waypointCoords.z, 9999.f, 262668, .0f);
			}
		}
		else if (playerState == PlayerState::WAYPOINT)
		{
			if (IS_WAYPOINT_ACTIVE())
			{
				Vector3 newCoords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));

				// Player waypoint doesn't have proper z coord, use player z coord instead
				newCoords.z = playerPos.z;
				
				if (newCoords.x != waypointCoords.x || newCoords.y != waypointCoords.y)
				{
					waypointCoords = newCoords;
					TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, vehicle, waypointCoords.x, waypointCoords.y, waypointCoords.z, 9999.f, 262668, .0f);
				}
			}
			else
			{
				playerState = PlayerState::WANDER;
				TASK_VEHICLE_DRIVE_WANDER(playerPed, vehicle, 9999.f, 262668);
			}
		}
	}
	else
	{
		if (playerState != PlayerState::NONE)
		{
			CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
		}
	}
}

static void OnStop()
{
	Ped playerPed = PLAYER_PED_ID();
	for (int i = 0; i < pairs.size(); i++)
	{
		TeslaPair pr = pairs.at(i);
		Ped p = pr.ped;
		Vehicle v = pr.vehicle;
		RESET_ENTITY_ALPHA(p);
	}
	pairs.clear();
	CLEAR_PED_TASKS(playerPed);

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
	}
	playerState = PlayerState::NONE;
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Self Drving Cars",
		.Id = "vehs_tesla",
		.IsTimed = true
	}
);
