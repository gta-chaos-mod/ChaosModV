#include <stdafx.h>

static enum
{
	STATE_WAITING,
	STATE_DRIVING,
	STATE_COMBAT,
	STATE_TO_COORDS_ANY_MEANS,
	STATE_ROAMING
}
m_state;

static Vector3 m_waypointCoords;
static bool m_foundWaypoint;
static bool m_reachedWaypoint;
static Ped m_toCombatPed;

static void OnStart()
{
	m_state = STATE_WAITING;
	m_foundWaypoint = false;
	m_reachedWaypoint = false;
	m_toCombatPed = 0;

	Ped playerPed = PLAYER_PED_ID();

	// Try to fetch waypoint coords (if waypoint exists)
	if (IS_WAYPOINT_ACTIVE())
	{
		m_waypointCoords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
		m_foundWaypoint = true;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			Blip blip = GET_FIRST_BLIP_INFO_ID(i);
			if (DOES_BLIP_EXIST(blip))
			{
				m_waypointCoords = GET_BLIP_COORDS(blip);
				m_foundWaypoint = true;

				break;
			}
		}
	}
}

static void OnStop()
{
	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();

	SET_PLAYER_CONTROL(player, true, 7001);

	SET_ENTITY_INVINCIBLE(playerPed, false);

	CLEAR_PED_TASKS(playerPed);

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
	}
}

static void OnTick()
{
	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	float distToWaypoint = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, false);
	bool isAtDest = m_foundWaypoint && distToWaypoint <= 10.f;

	SET_PLAYER_CONTROL(player, false, 7001);

	SET_ENTITY_INVINCIBLE(playerPed, true);

	/* State stuff */

	if (m_state == STATE_COMBAT)
	{
		// Required to make player ped actually shoot
		_0x2208438012482A1A(playerPed, false, false);

		SET_GAMEPLAY_CAM_RELATIVE_HEADING(.0f);
	}

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false) && !m_reachedWaypoint)
	{
		if (m_state == STATE_WAITING && !isAtDest)
		{
			if (m_foundWaypoint)
			{
				TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, playerVeh, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 9999.f, 4176732, 10.f);
			}
			else
			{
				TASK_SMART_FLEE_COORD(playerPed, playerPos.x, playerPos.y, playerPos.z, 9999.f, -1, false, false);
			}

			m_state = STATE_DRIVING;
		}
	}
	else
	{
		// Priority List:
		// Combat
		// Run to coords through any means
		// Roam around

		if (!m_toCombatPed && m_state != STATE_COMBAT)
		{
			Ped closestPed = 0;
			float closestDist = 9999.f;

			for (Ped ped : GetAllPeds())
			{
				if (!IS_PED_DEAD_OR_DYING(ped, true) && HAS_ENTITY_CLEAR_LOS_TO_ENTITY(playerPed, ped, 17))
				{
					int rel = GET_RELATIONSHIP_BETWEEN_PEDS(playerPed, ped);
					int pedType = GET_PED_TYPE(ped);
					if (rel == 4 || rel == 5 || ((pedType == 6 || pedType == 27 || pedType == 29) && GET_PLAYER_WANTED_LEVEL(player) > 0))
					{
						Vector3 pedPos = GET_ENTITY_COORDS(m_toCombatPed, false);
						float pedDist = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z, true);

						if (pedDist < closestDist)
						{
							closestPed = ped;
							closestDist = pedDist;
						}
					}
				}
			}

			if (closestPed)
			{
				m_toCombatPed = closestPed;

				m_state = STATE_COMBAT;
			}
		}

		if (!m_toCombatPed && m_state == STATE_WAITING)
		{
			if (m_foundWaypoint)
			{
				TASK_GO_TO_COORD_ANY_MEANS(playerPed, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 9999.f, 0, false, 0, .0f);

				m_state = STATE_TO_COORDS_ANY_MEANS;
			}
			else
			{
				TASK_SMART_FLEE_COORD(playerPed, playerPos.x, playerPos.y, playerPos.z, 9999.f, -1, false, false);

				m_state = STATE_ROAMING;
			}
		}
	}

	if (m_state == STATE_DRIVING && (!DOES_ENTITY_EXIST(playerVeh) || !IS_VEHICLE_DRIVEABLE(playerVeh, true) || (isAtDest && IS_VEHICLE_STOPPED(playerVeh))))
	{
		TASK_LEAVE_ANY_VEHICLE(playerPed, 0, 0);

		m_reachedWaypoint = true;

		m_state = STATE_WAITING;
	}
	else if (m_state == STATE_COMBAT)
	{
		if (!DOES_ENTITY_EXIST(m_toCombatPed) || IS_PED_DEAD_OR_DYING(m_toCombatPed, true))
		{
			m_toCombatPed = 0;

			m_state = STATE_WAITING;
		}
		else if (!IS_PED_IN_COMBAT(playerPed, m_toCombatPed))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(m_toCombatPed, false);
			float pedDist = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z, true);

			if (pedDist > 200.f)
			{
				static const Hash sniperHash = GET_HASH_KEY("WEAPON_SNIPERRIFLE");

				// Sniper time
				GIVE_WEAPON_TO_PED(playerPed, sniperHash, 9999, false, true);
				SET_CURRENT_PED_WEAPON(playerPed, sniperHash, true);
			}
			else
			{
				Hash bestWep = GET_BEST_PED_WEAPON(playerPed, false);

				// If player has no gun, just give a pistol
				if (GET_WEAPON_DAMAGE_TYPE(bestWep) == 2)
				{
					static const Hash pistolHash = GET_HASH_KEY("WEAPON_PISTOL");

					GIVE_WEAPON_TO_PED(playerPed, pistolHash, 9999, false, true);
					SET_CURRENT_PED_WEAPON(playerPed, pistolHash, true);
				}
				else
				{
					SET_CURRENT_PED_WEAPON(playerPed, bestWep, true);
				}
			}

			TASK_SHOOT_AT_ENTITY(playerPed, m_toCombatPed, -1, -957453492);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_AUTOPILOT, OnStart, OnStop, OnTick);