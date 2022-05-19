#include <stdafx.h>

#include "Memory/WeaponPool.h"

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup    = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_REVIVED", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, femCivGroup);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && IS_PED_DEAD_OR_DYING(ped, false))
		{
			int pedType                = GET_PED_TYPE(ped);
			Hash pedModel              = GET_ENTITY_MODEL(ped);

			bool isMissionEntityCorpse = IS_ENTITY_A_MISSION_ENTITY(ped);

			Ped clone;
			if (isMissionEntityCorpse)
			{
				clone = CreatePoolPed(pedType, pedModel, .0f, .0f, .0f, .0f);
			}
			else
			{
				clone = CREATE_PED(pedType, pedModel, .0f, .0f, .0f, .0f, true, false);
			}

			CLONE_PED_TO_TARGET(ped, clone);

			// See if corpse is in any vehicle
			// If yes, set clone into seat corpse is occupying later
			Vehicle targetVeh = 0;
			int targetSeat    = 0;
			if (IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				Vehicle veh   = GET_VEHICLE_PED_IS_IN(ped, false);
				Hash vehModel = GET_ENTITY_MODEL(veh);
				int maxSeats  = GET_VEHICLE_MODEL_NUMBER_OF_SEATS(vehModel);

				for (int i = -1; i < maxSeats; i++)
				{
					if (!IS_VEHICLE_SEAT_FREE(veh, i, false))
					{
						Ped seatPed = GET_PED_IN_VEHICLE_SEAT(veh, i, false);

						if (seatPed == ped)
						{
							targetVeh  = veh;
							targetSeat = i;
							break;
						}
					}
				}
			}

			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

			// Deleting the corpse requires the corpse to be a mission entity
			SET_ENTITY_AS_MISSION_ENTITY(ped, false, false);
			DELETE_PED(&ped);

			if (targetVeh)
			{
				SET_PED_INTO_VEHICLE(clone, targetVeh, targetSeat);
			}
			else
			{
				SET_ENTITY_COORDS(clone, pedPos.x, pedPos.y, pedPos.z, false, false, false, false);
			}

			SET_PED_RELATIONSHIP_GROUP_HASH(clone, relationshipGroup);
			SET_PED_HEARING_RANGE(clone, 9999.f);
			SET_PED_CONFIG_FLAG(clone, 281, true);
			SET_PED_COMBAT_ATTRIBUTES(clone, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(clone, 46, true);
			SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(clone, false);
			SET_RAGDOLL_BLOCKING_FLAGS(clone, 5);
			SET_PED_SUFFERS_CRITICAL_HITS(clone, false);

			static const std::vector<Hash> &weps = Memory::GetAllWeapons();
			GIVE_WEAPON_TO_PED(clone, weps[g_Random.GetRandomInt(0, weps.size() - 1)], 9999, false, true);

			SET_PED_ACCURACY(clone, 100);
			SET_PED_FIRING_PATTERN(clone, 0xC6EE6B4C);

			if (!isMissionEntityCorpse)
			{
				SET_PED_AS_NO_LONGER_NEEDED(&clone);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Revive Dead Peds",
		.Id = "peds_revive"
	}
);