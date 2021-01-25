#include <stdafx.h>

static int m_state;
static std::map<Ped, Vector3> m_toTpPeds;
static std::map<Vehicle, Vector3> m_toTpVehs;

static void OnStart()
{
	m_state = 0;
	m_toTpPeds.clear();
	m_toTpVehs.clear();
}

static void OnTickLag()
{

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (curTick > lastTick + 500)
	{
		lastTick = curTick;

		if (++m_state == 4)
		{
			m_state = 0;
		}

		if (m_state == 2)
		{
			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_IN_ANY_VEHICLE(ped, true) && !GET_VEHICLE_PED_IS_ENTERING(ped))
				{
					Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

					m_toTpPeds.emplace(ped, pedPos);
				}
			}

			for (Vehicle veh : GetAllVehs())
			{
				Vector3 vehPos = GET_ENTITY_COORDS(veh, false);

				m_toTpVehs.emplace(veh, vehPos);
			}
		}
		else if (m_state == 3)
		{
			for (const auto& pair : m_toTpPeds)
			{
				const Ped& ped = pair.first;

				Vector3 vel = GET_ENTITY_VELOCITY(ped);
				float heading = GET_ENTITY_HEADING(ped);

				const Vector3& tpPos = pair.second;

				SET_ENTITY_COORDS_NO_OFFSET(ped, tpPos.x, tpPos.y, tpPos.z, false, false, false);

				SET_ENTITY_HEADING(ped, heading);
				SET_ENTITY_VELOCITY(ped, vel.x, vel.y, vel.z);
			}

			m_toTpPeds.clear();

			for (const auto& pair : m_toTpVehs)
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

			m_toTpVehs.clear();
		}
	}
}

static RegisterEffect registerEffect3(EFFECT_GAMESPEED_LAG, OnStart, nullptr, OnTickLag);