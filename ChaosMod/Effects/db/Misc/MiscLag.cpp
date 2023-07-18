#include <stdafx.h>

static int ms_State;
static std::map<Ped, Vector3> ms_ToTpPeds;
static std::map<Vehicle, Vector3> ms_ToTpVehs;

static void OnStart()
{
	ms_State = 0;
	ms_ToTpVehs.clear();
	ms_ToTpVehs.clear();
}

static void OnTickLag()
{
	static DWORD64 lastTick = 0;
	auto curTick            = GET_GAME_TIMER();

	if (curTick > lastTick + 500)
	{
		lastTick = curTick;

		if (++ms_State == 4)
		{
			ms_State = 0;
		}

		if (ms_State == 2)
		{
			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_IN_ANY_VEHICLE(ped, true) && !GET_VEHICLE_PED_IS_ENTERING(ped))
				{
					Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

					ms_ToTpPeds.emplace(ped, pedPos);
				}
			}

			for (Vehicle veh : GetAllVehs())
			{
				Vector3 vehPos = GET_ENTITY_COORDS(veh, false);

				ms_ToTpVehs.emplace(veh, vehPos);
			}
		}
		else if (ms_State == 3)
		{
			// save current camera heading to apply after teleporting
			float camHeading = GET_GAMEPLAY_CAM_RELATIVE_HEADING();

			for (const auto &pair : ms_ToTpVehs)
			{
				const Ped &ped       = pair.first;

				Vector3 vel          = GET_ENTITY_VELOCITY(ped);
				float heading        = GET_ENTITY_HEADING(ped);

				const Vector3 &tpPos = pair.second;

				SET_ENTITY_COORDS_NO_OFFSET(ped, tpPos.x, tpPos.y, tpPos.z, false, false, false);

				SET_ENTITY_HEADING(ped, heading);
				SET_ENTITY_VELOCITY(ped, vel.x, vel.y, vel.z);
			}

			ms_ToTpVehs.clear();

			for (const auto &pair : ms_ToTpVehs)
			{
				const Vehicle &veh = pair.first;

				Vector3 vel        = GET_ENTITY_VELOCITY(veh);
				float heading      = GET_ENTITY_HEADING(veh);
				float forwardSpeed = GET_ENTITY_SPEED(veh);

				// if the vehicle is reversing use a negative forward speed
				if (GET_ENTITY_SPEED_VECTOR(veh, true).y < 0)
				{
					forwardSpeed *= -1;
				}

				const Vector3 &tpPos = pair.second;

				SET_ENTITY_COORDS_NO_OFFSET(veh, tpPos.x, tpPos.y, tpPos.z, false, false, false);

				SET_ENTITY_HEADING(veh, heading);
				SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);

				SET_VEHICLE_FORWARD_SPEED(veh, forwardSpeed);
			}

			ms_ToTpVehs.clear();

			SET_GAMEPLAY_CAM_RELATIVE_HEADING(camHeading);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTickLag, EffectInfo
	{
		.Name = "Lag",
		.Id = "time_lag",
		.IsTimed = true,
		.IsShortDuration = true
	}
);