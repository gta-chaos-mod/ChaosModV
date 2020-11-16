#include <stdafx.h>

//Effect by ProfessorBiddle

static void OnTick()
{
	static const Hash parachute = GET_HASH_KEY("GADGET_PARACHUTE");
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");
	static const std::vector<Hash>& weps = Memory::GetAllWeapons();
	static constexpr int maxJesus = 40;
	static constexpr Hash jesusHash = -835930287;
	static Ped jesuss[maxJesus] = {};
	static int jesusDespawnTime[maxJesus];
	static int jesusAmount = 0;
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GetTickCount64();

	if (jesusAmount <= maxJesus && curTick > lastTick + 800)
	{
		lastTick = curTick;
		Vector3 spawnPos;
		spawnPos.x = playerPos.x + g_random.GetRandomInt(-20, 20);
		spawnPos.y = playerPos.y + g_random.GetRandomInt(-20, 20);
		spawnPos.z = playerPos.z + g_random.GetRandomInt(50, 75);
		LoadModel(jesusHash);
		Ped jesus = CREATE_PED(28, jesusHash, spawnPos.x, spawnPos.y, spawnPos.z, g_random.GetRandomInt(0, 359), true, false);
		jesusAmount++; 
		for (int i = 0; i < maxJesus; i++)
		{
			Ped& ped = jesuss[i];
			if (!ped)
			{
				ped = jesus;
				jesusDespawnTime[i] = 5;
				break;
			}
		}	
		SET_MODEL_AS_NO_LONGER_NEEDED(maxJesus);
		//Give em' a shove
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(jesus, 0, 35.f, 0, -2500.f, true, false, true, true);
		//give parachute, for some reason only works with both
		SET_PED_GADGET(jesus, parachute, true);
		GIVE_WEAPON_TO_PED(jesus, parachute, 9999, true, true);
		//Jesus hates criminals
		Hash relationshipGroup;
		ADD_RELATIONSHIP_GROUP("_HOSTILE_JESUS", &relationshipGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, femCivGroup);
		SET_PED_RELATIONSHIP_GROUP_HASH(jesus, relationshipGroup);
		//random weapons
		Hash wep = weps[g_random.GetRandomInt(0, weps.size() - 1)];
		GIVE_WEAPON_TO_PED(jesus, wep, 9999, true, false);
		SET_CURRENT_PED_WEAPON(jesus, wep, true);
		//Make jesus badass, but not as badass as Griefer Jesus		
		SET_ENTITY_PROOFS(jesus, false, true, true, true, false, false, false, false);		
		SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(jesus, true);
		SET_RAGDOLL_BLOCKING_FLAGS(jesus, 5);
		SET_PED_SUFFERS_CRITICAL_HITS(jesus, true);
		//Safely parachute to player (sometimes)
		TASK_PARACHUTE_TO_TARGET(jesus, playerPos.x, playerPos.y, playerPos.z);
	}
	static DWORD64 lastTick2 = 0;
	for (int i = 0; i < maxJesus; i++)
	{
		Ped& jesus = jesuss[i];
		if (jesus)
		{
			if (DOES_ENTITY_EXIST(jesus) && jesusDespawnTime[i] > 0)
			{
				//Update target position to always be the player
				SET_PARACHUTE_TASK_TARGET(jesus, playerPos.x, playerPos.y, playerPos.z);
				Vector3 propPos = GET_ENTITY_COORDS(jesus, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, propPos.x, propPos.y, propPos.z, true) < 800.f)
				{
					if (!IS_ENTITY_IN_AIR(jesus) && !GET_IS_TASK_ACTIVE(jesus, 282))
					{
						if (lastTick2 < curTick - 1000)
						{
							jesusDespawnTime[i]--;
							//Attack time
							SET_PED_COMBAT_ATTRIBUTES(jesus, 1, true);
							SET_PED_COMBAT_ATTRIBUTES(jesus, 5, true);
							SET_PED_COMBAT_ATTRIBUTES(jesus, 46, true);
							SET_PED_HEARING_RANGE(jesus, 9999.f);
							SET_PED_CONFIG_FLAG(jesus, 281, true);
							TASK_COMBAT_PED(jesus, playerPed, 0, 16);
							SET_PED_FIRING_PATTERN(jesus, 0xD6FF6D61);
						}
					}
					continue;
				}
			}
			jesusAmount--;

			SET_OBJECT_AS_NO_LONGER_NEEDED(&jesus);
			jesus = 0;
		}
	}
	if (lastTick2 < curTick - 1000)
	{
		lastTick2 = curTick;
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_JESUS_RAIN, nullptr, nullptr, OnTick);