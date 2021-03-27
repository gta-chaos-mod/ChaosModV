#include <stdafx.h>

static enum
{
	STATE_NONE,
	STATE_COMBAT,
	STATE_TO_COORDS,
	STATE_ROAMING
}
m_state;

static Vector3 m_waypointCoords;
static bool m_foundWaypoint;
static bool m_customWaypoint;

static bool VectorEqualRoughZ(Vector3 vec1, Vector3 vec2)
{
	return vec1.x == vec2.x && vec1.y == vec2.y && std::abs(vec1.z - vec2.z) < 1.f;
}

static void OnStart()
{
	m_state = STATE_NONE;

	m_waypointCoords.x = m_waypointCoords.y = m_waypointCoords.z = .0f;

	m_foundWaypoint = false;
	m_customWaypoint = false;
}

static void OnStop()
{
	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();

	SET_PLAYER_CONTROL(player, true, 7001);

	SET_ENTITY_INVINCIBLE(playerPed, false);

	SET_PED_INFINITE_AMMO_CLIP(playerPed, false);

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

	bool playerDead = IS_PED_DEAD_OR_DYING(playerPed, false);
	if (playerDead || IS_PLAYER_CONTROL_ON(player))
	{
		m_state = STATE_NONE;

		SET_PLAYER_CONTROL(player, false, 7001);

		if (playerDead)
		{
			return;
		}
	}

#ifdef _DEBUG
	// Draw debug go to line
	if (m_state == STATE_TO_COORDS)
	{
		DRAW_LINE(playerPos.x, playerPos.y, playerPos.z, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 0, 255, 0, 255);
	}
#endif

	SET_ENTITY_INVINCIBLE(playerPed, true);

	if (m_state == STATE_COMBAT)
	{
		// Required to make player ped actually shoot
		_0x2208438012482A1A(playerPed, false, false);

		SET_GAMEPLAY_CAM_RELATIVE_HEADING(.0f);

		SET_PED_INFINITE_AMMO_CLIP(playerPed, true);

		SET_PED_ACCURACY(playerPed, 90);
	}

	// Run every 300 ms
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick = GET_GAME_TIMER();
	if (lastTick > curTick - 300)
	{
		return;
	}
	lastTick = curTick;

	// Try to fetch waypoint coords (if waypoint exists)
	m_foundWaypoint = false;
	Blip targetBlip = 0;

	Vector3 lastWaypointCoords = m_waypointCoords;

	if (IS_WAYPOINT_ACTIVE())
	{
		m_waypointCoords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));

		// Player waypoint doesn't have proper z coord, use player z coord instead
		m_waypointCoords.z = playerPos.z;

		m_foundWaypoint = true;
		m_customWaypoint = true;

		if (m_state != STATE_ROAMING && !VectorEqualRoughZ(m_waypointCoords, lastWaypointCoords))
		{
			m_state = STATE_NONE;
		}
	}
	else
	{
		float closestBlipDist = 9999.f;

		for (int i = 0; i < 3; i++)
		{
			Blip blip = GET_FIRST_BLIP_INFO_ID(i);
			if (DOES_BLIP_EXIST(blip) && IS_BLIP_ON_MINIMAP(blip) && GET_BLIP_ALPHA(blip) > 0)
			{
				m_foundWaypoint = true;

				Vector3 blipPos = GET_BLIP_COORDS(blip);
				float blipDist = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, blipPos.x, blipPos.y, blipPos.z, true);

				if (blipDist < closestBlipDist)
				{
					m_waypointCoords = blipPos;
					closestBlipDist = blipDist;
					targetBlip = blip;
				}
			}
		}

		if (m_foundWaypoint)
		{
			m_customWaypoint = true;

			if (m_state != STATE_ROAMING && !VectorEqualRoughZ(m_waypointCoords, lastWaypointCoords))
			{
				m_state = STATE_NONE;
			}
		}
	}

	bool isAtDest = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, false) <= 10.f;

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		bool vehDrivable = IS_VEHICLE_DRIVEABLE(playerVeh, true);
		bool vehStuck = IS_VEHICLE_STUCK_TIMER_UP(playerVeh, 0, 5000);

		if (vehDrivable && !vehStuck && !isAtDest)
		{
			if (m_state != STATE_TO_COORDS)
			{
				if (m_foundWaypoint)
				{
					// Plane
					if (GET_VEHICLE_CLASS(playerVeh) == 16)
					{
						float planeHeading = GET_ENTITY_HEADING(playerVeh);

						TASK_PLANE_MISSION(playerPed, playerVeh, 0, 0, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 4, 100.f, .0f, planeHeading, .0f, 200.f, 0);
					}
					else
					{
						TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(playerPed, playerVeh, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 9999.f, 262668, .0f);
					}

					m_state = STATE_TO_COORDS;
				}
				else if (m_state != STATE_ROAMING)
				{
					TASK_SMART_FLEE_COORD(playerPed, playerPos.x, playerPos.y, playerPos.z, 9999.f, -1, false, false);

					m_state = STATE_ROAMING;
				}
			}
		}
		else if (!vehDrivable || vehStuck || (isAtDest && IS_VEHICLE_STOPPED(playerVeh)))
		{
			bool isTargetThisVehicle = false;

			if (targetBlip)
			{
				if (GET_BLIP_INFO_ID_ENTITY_INDEX(targetBlip) == playerVeh)
				{
					isTargetThisVehicle = true;
				}
			}

			if (!vehDrivable || (!isTargetThisVehicle && GET_IS_VEHICLE_ENGINE_RUNNING(playerVeh)))
			{
				TASK_LEAVE_ANY_VEHICLE(playerPed, 0, 0);
			}

			m_state = STATE_NONE;
		}
	}
	else if (!IS_PED_IN_ANY_VEHICLE(playerPed, true) && !GET_VEHICLE_PED_IS_ENTERING(playerPed))
	{
		// Priority List:
		// Combat
		// Run to coords through any means
		// Roam around

		Ped closestEnemyPed = 0;
		float closestEnemyDist = 1000.f;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_DEAD_OR_DYING(ped, true) && HAS_ENTITY_CLEAR_LOS_TO_ENTITY(playerPed, ped, 17))
			{
				BOOL bulletproof, dummy;
				_GET_ENTITY_PROOFS(ped, &bulletproof, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);

				if (bulletproof)
				{
					continue;
				}

				int rel = GET_RELATIONSHIP_BETWEEN_PEDS(playerPed, ped);
				int pedType = GET_PED_TYPE(ped);
				if (IS_PED_IN_COMBAT(ped, playerPed) || rel == 4 || rel == 5 || ((pedType == 6 || pedType == 27 || pedType == 29) && GET_PLAYER_WANTED_LEVEL(player) > 0))
				{
					Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
					float pedDist = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z, true);

					if (pedDist < closestEnemyDist)
					{
						closestEnemyPed = ped;
						closestEnemyDist = pedDist;
					}
				}
			}
		}

		if (closestEnemyPed)
		{
			if (!IS_PED_IN_COMBAT(playerPed, closestEnemyPed))
			{
				if (closestEnemyDist > 200.f)
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

				TASK_SHOOT_AT_ENTITY(playerPed, closestEnemyPed, -1, -957453492);
			}

			m_state = STATE_COMBAT;
		}
		else if (m_state != STATE_TO_COORDS)
		{
			if (m_foundWaypoint)
			{
				bool isEnteringVehicle = false;

				if (targetBlip)
				{
					Entity blipEntity = GET_BLIP_INFO_ID_ENTITY_INDEX(targetBlip);
					if (blipEntity && DOES_ENTITY_EXIST(blipEntity) && GET_ENTITY_TYPE(blipEntity) == 2 /* Vehicle */
						&& !IS_PED_IN_VEHICLE(playerPed, blipEntity, true) && GET_VEHICLE_PED_IS_ENTERING(playerPed) != blipEntity)
					{
						isEnteringVehicle = true;

						TASK_ENTER_VEHICLE(playerPed, blipEntity, -1, -1, 2.f, 1, 0);
					}
				}
				
				if (!isEnteringVehicle)
				{
					bool isCarNearby = false;

					for (Vehicle veh : GetAllVehs())
					{
						int doorLock = GET_VEHICLE_DOOR_LOCK_STATUS(veh);

						if (IS_VEHICLE_DRIVEABLE(veh, true) && doorLock != 2 && doorLock != 3 && doorLock != 10)
						{
							Vector3 vehPos = GET_ENTITY_COORDS(veh, false);
							float vehDist = GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, vehPos.x, vehPos.y, vehPos.z, true);

							if (vehDist < 20.f)
							{
								isCarNearby = true;

								break;
							}
						}
					}

					if (isCarNearby)
					{
						TASK_GO_TO_COORD_ANY_MEANS(playerPed, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 2.f, 0, false, 262668, .0f);
					}
					else
					{
						TASK_FOLLOW_NAV_MESH_TO_COORD(playerPed, m_waypointCoords.x, m_waypointCoords.y, m_waypointCoords.z, 2.f, -1, .0f, true, .0f);
					}
				}

				m_state = STATE_TO_COORDS;
			}
			else if (m_state != STATE_ROAMING)
			{
				TASK_SMART_FLEE_COORD(playerPed, playerPos.x, playerPos.y, playerPos.z, 9999.f, -1, false, false);

				m_state = STATE_ROAMING;
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_AUTOPILOT, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Autopilot",
		.Id = "player_break",
		.IsTimed = true,
		.IsShortDuration = true
	}
);