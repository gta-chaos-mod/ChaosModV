#include <stdafx.h>

static std::list<Ped> m_suicider;
static std::list<Ped> m_suiciding;

static void OnStart()
{
	m_suicider.clear();
	m_suiciding.clear();
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_DEAD_OR_DYING(ped, true))
		{
			CLEAR_PED_TASKS_IMMEDIATELY(ped);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, false);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, false);
		}
	}
	Player playerID = PLAYER_ID();
	SET_PLAYER_WANTED_LEVEL(playerID, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(playerID, false);
}

static void OnTick()
{
	Player playerID = PLAYER_ID();
	Ped player = PLAYER_PED_ID();
	SET_PLAYER_WANTED_LEVEL(playerID, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(playerID, false);
	Vector3 playerPos = GET_ENTITY_COORDS(player, false);
	static Hash sniperHash = GET_HASH_KEY("WEAPON_HEAVYSNIPER");
	static Hash shotgunHash = GET_HASH_KEY("WEAPON_ASSAULTSHOTGUN");
	static Hash rifleHash = GET_HASH_KEY("WEAPON_ASSAULTRIFLE");
	static Hash stungunHash = GET_HASH_KEY("WEAPON_STUNGUN");

	for (Ped ped : GetAllPeds())
	{
		Ped pedType = GET_PED_TYPE(ped);
		Hash weaponHash = GET_SELECTED_PED_WEAPON(ped);
		BOOL isDone = (weaponHash == stungunHash || weaponHash == sniperHash || weaponHash == shotgunHash || weaponHash == rifleHash);

		if (!IS_PED_A_PLAYER(ped) && pedType != 0 && pedType != 1 && pedType != 2 && !isDone)
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
			if (IS_PED_SITTING_IN_ANY_VEHICLE(ped) && (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, pedPos.x, pedPos.y, pedPos.z, true) <= 100.f))
			{
				Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, false);
				Hash vehModel = GET_ENTITY_MODEL(pedVeh);
				int maxSeats = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);
				if (!IS_PED_IN_ANY_HELI(ped) && !IS_PED_IN_ANY_PLANE(ped))
				{
					for (int i = -1; i < maxSeats - 1; i++)
					{
						if (!IS_VEHICLE_SEAT_FREE(pedVeh, i, false))
						{
							Ped ped2 = GET_PED_IN_VEHICLE_SEAT(pedVeh, i, false);
							CLEAR_PED_TASKS_IMMEDIATELY(ped2);
							SET_PED_TO_RAGDOLL(ped2, 100, 100, 0, true, true, false);
							REMOVE_ALL_PED_WEAPONS(ped2, false);
						}
					}
				}
				else
				{
					for (int i = -1; i < maxSeats - 1; i++)
					{
						if (!IS_VEHICLE_SEAT_FREE(pedVeh, i, false))
						{
							Ped ped2 = GET_PED_IN_VEHICLE_SEAT(pedVeh, i, false);
							CLEAR_PED_TASKS_IMMEDIATELY(ped2);
							SET_PED_TO_RAGDOLL(ped2, 100, 100, 0, true, true, false);
							REMOVE_ALL_PED_WEAPONS(ped2, false);
							GIVE_WEAPON_TO_PED(ped2, GET_HASH_KEY("WEAPON_PARACHUTE"), 9999, true, true);
						}
					}
				}
				SET_ENTITY_AS_MISSION_ENTITY(pedVeh, true, true);
				DELETE_VEHICLE(&pedVeh);
			}
			if (!IS_PED_SITTING_IN_ANY_VEHICLE(ped) && !IS_PED_GETTING_INTO_A_VEHICLE(ped) && !IS_PED_DEAD_OR_DYING(ped, true))
			{
				int weaponType = g_Random.GetRandomInt(1, 10);
				if (weaponType <= 5)
				{
					GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_ASSAULTRIFLE"), 9999, true, true);
				}
				if (weaponType <= 8 && weaponType >= 6)
				{
					GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_ASSAULTSHOTGUN"), 9999, true, true);
				}
				if (weaponType == 9)
				{
					GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_HEAVYSNIPER"), 9999, true, true);
				}
				if (weaponType == 10)
				{
					GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_STUNGUN"), 9999, true, true);
				}
			}
			SET_PED_AS_COP(ped, true);
			SET_PED_DROPS_WEAPONS_WHEN_DEAD(ped, false);
			SET_PED_COMBAT_ATTRIBUTES(ped, 3, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
			SET_PED_FLEE_ATTRIBUTES(ped, 2, true);
			SET_PED_ACCURACY(ped, 100);
			SET_PED_HEARING_RANGE(ped, 9999.f);
		}
	}

	for (Ped ped : GetAllPeds())
	{
		Ped pedType = GET_PED_TYPE(ped);
		if (!IS_PED_A_PLAYER(ped) && pedType != 0 && pedType != 1 && pedType != 2 && !IS_PED_DEAD_OR_DYING(ped, true))
		{
			Hash weaponHash = GET_SELECTED_PED_WEAPON(ped);
			if (sniperHash == weaponHash)
			{
				m_suicider.push_back(ped);
			}
			BOOL isDone = (weaponHash == stungunHash || weaponHash == sniperHash || weaponHash == shotgunHash || weaponHash == rifleHash);
			if (isDone)
			{
				SET_PED_AS_COP(ped, true);
				SET_PED_DROPS_WEAPONS_WHEN_DEAD(ped, false);
				SET_PED_COMBAT_ATTRIBUTES(ped, 3, true);
				SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
				SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
				SET_PED_FLEE_ATTRIBUTES(ped, 2, true);
				SET_PED_ACCURACY(ped, 100);
				SET_PED_HEARING_RANGE(ped, 9999.f);
			}
		}
	}

	for (std::list<Ped>::iterator it = m_suicider.begin(); it != m_suicider.end(); )
	{
		Ped suicider = *it;

		if (DOES_ENTITY_EXIST(suicider))
		{
			Vector3 suiciderPos = GET_ENTITY_COORDS(suicider, false);
			int maxHealth = GET_ENTITY_MAX_HEALTH(suicider);

			if (maxHealth > 0)
			{
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, suiciderPos.x, suiciderPos.y, suiciderPos.z, true) >= 30.f)
				{
					Hash suiciderweaponHash = GET_SELECTED_PED_WEAPON(suicider);
					if (sniperHash != suiciderweaponHash)
					{
						REMOVE_ALL_PED_WEAPONS(suicider, false);
						GIVE_WEAPON_TO_PED(suicider, GET_HASH_KEY("WEAPON_HEAVYSNIPER"), 9999, true, true);

						it++;
						
						continue;
					}
				}
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, suiciderPos.x, suiciderPos.y, suiciderPos.z, true) <= 15.f)
				{
					if (IS_PED_IN_COVER(suicider, false))
					{
						CLEAR_PED_TASKS_IMMEDIATELY(suicider);
					}
					TASK_COMBAT_PED(suicider, player, 0, 16);
					SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(suicider, true);
					REMOVE_ALL_PED_WEAPONS(suicider, false);
					m_suiciding.push_back(suicider);
				}
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, suiciderPos.x, suiciderPos.y, suiciderPos.z, true) <= 3.f)
				{
					ADD_EXPLOSION(suiciderPos.x, suiciderPos.y, suiciderPos.z, 4, 9999.f, true, false, 1.f, false);
					SET_ENTITY_HEALTH(suicider, 0, false);
					SET_ENTITY_MAX_HEALTH(suicider, 0);
				}
				else
				{
					it++;

					continue;
				}
			}
		}
		it = m_suicider.erase(it);
	}

    for (std::list<Ped>::iterator it2 = m_suiciding.begin(); it2 != m_suiciding.end(); )
    {
	    Ped suicider = *it2;

		if (DOES_ENTITY_EXIST(suicider))
		{
			int maxHealth = GET_ENTITY_MAX_HEALTH(suicider);

			if (maxHealth > 0)
			{
				Vector3 suiciderPos1 = GET_ENTITY_COORDS(suicider, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, suiciderPos1.x, suiciderPos1.y, suiciderPos1.z, true) >= 30.f)
				{
					CLEAR_PED_TASKS_IMMEDIATELY(suicider);
					SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(suicider, false);
				}
				else
				{
					it2++;

					continue;
				}
			}
		}
		it2 = m_suiciding.erase(it2);
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_FULLASSAULT, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Full Police Assault",
		.Id = "misc_fullassault",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_PEDS_COPS, EFFECT_PEDS_FOLLOW_PLAYER, EFFECT_NEVER_WANTED }
	}
);