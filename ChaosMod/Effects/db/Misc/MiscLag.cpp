#include <stdafx.h>

static int m_state;

static void OnStart()
{
	m_state = 0;
}

static void OnTickLag()
{
	static std::map<Ped, Vector3> toTpPeds;
	static std::map<Vehicle, Vector3> toTpVehs;

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GetTickCount64();

	if (curTick > lastTick + 500)
	{
		lastTick = curTick;

		if (++m_state == 3)
		{
			m_state = 0;
		}

		if (m_state == 1)
		{
			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_IN_ANY_VEHICLE(ped, true) && !GET_VEHICLE_PED_IS_ENTERING(ped))
				{
					Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

					toTpPeds.emplace(ped, pedPos);
				}
			}

			for (Vehicle veh : GetAllVehs())
			{
				Vector3 vehPos = GET_ENTITY_COORDS(veh, false);

				toTpVehs.emplace(veh, vehPos);
			}
		}
		else if (m_state == 2)
		{
			for (const auto& pair : toTpPeds)
			{
				const Ped& ped = pair.first;

				Vector3 vel = GET_ENTITY_VELOCITY(ped);
				float heading = GET_ENTITY_HEADING(ped);

				const Vector3& tpPos = pair.second;

				SET_ENTITY_COORDS_NO_OFFSET(ped, tpPos.x, tpPos.y, tpPos.z, false, false, false);

				SET_ENTITY_HEADING(ped, heading);
				SET_ENTITY_VELOCITY(ped, vel.x, vel.y, vel.z);
			}

			toTpPeds.clear();

			for (const auto& pair : toTpVehs)
			{
				const Vehicle& veh = pair.first;

				Vector3 vel = GET_ENTITY_VELOCITY(veh);
				float heading = GET_ENTITY_HEADING(veh);
				float forwardSpeed = GET_ENTITY_SPEED(veh);

				const Vector3& tpPos = pair.second;

				SET_ENTITY_COORDS_NO_OFFSET(veh, tpPos.x, tpPos.y, tpPos.z, false, false, false);

				SET_ENTITY_HEADING(veh, heading);
				SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);

				SET_VEHICLE_FORWARD_SPEED(veh, forwardSpeed);
			}

			toTpVehs.clear();
		}
	}
}

static RegisterEffect registerEffect3(EFFECT_GAMESPEED_LAG, OnStart, nullptr, OnTickLag);