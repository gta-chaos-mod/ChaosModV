#include <stdafx.h>

static void OnStart()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_MAX_WANTED_LEVEL(0);
	static const auto playerGroupHash = GET_HASH_KEY("PLAYER");
	static const auto civMaleGroupHash = GET_HASH_KEY("CIVMALE");
	static const auto civFemaleGroupHash = GET_HASH_KEY("CIVFEMALE");

	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_ZOMBIES", &groupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, civMaleGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, civFemaleGroupHash);
}

static void OnStop()
{
	SET_MAX_WANTED_LEVEL(5);
	static const auto zombieGroupHash = GET_HASH_KEY("_ZOMBIES");

	for (Ped ped : GetAllPeds())
	{
		if (GET_PED_RELATIONSHIP_GROUP_HASH(ped) == zombieGroupHash)
		{
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
	}
}

static void OnTick()
{
	static constexpr int MAX_ZOMBIES = 20;
	static constexpr Hash MODEL_HASH = -1404353274;

	static Hash zombieGroupHash = GET_HASH_KEY("_ZOMBIES");
	static Ped zombies[MAX_ZOMBIES] = {};
	static int zombiesAmount = 0;
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	if (zombiesAmount <= MAX_ZOMBIES)
	{
		Vector3 spawnPos;
		if (GET_NTH_CLOSEST_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 10 + zombiesAmount, &spawnPos, 0, 0, 0)
			&& GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, spawnPos.x, spawnPos.y, spawnPos.z, false) < 300.f)
		{
			LoadModel(MODEL_HASH);

			Ped zombie = CREATE_PED(26, MODEL_HASH, spawnPos.x, spawnPos.y, spawnPos.z, .0f, true, false);
			zombiesAmount++;

			for (Ped& ped : zombies)
			{
				if (!ped)
				{
					ped = zombie;
					break;
				}
			}

			SET_PED_RELATIONSHIP_GROUP_HASH(zombie, zombieGroupHash);
			SET_PED_COMBAT_ATTRIBUTES(zombie, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(zombie, 46, true);

			SET_AMBIENT_VOICE_NAME(zombie, "ALIENS");
			DISABLE_PED_PAIN_AUDIO(zombie, true);

			TASK_COMBAT_PED(zombie, PLAYER_PED_ID(), 0, 16);

			SET_MODEL_AS_NO_LONGER_NEEDED(MODEL_HASH);
		}
	}

	for (Ped& zombie : zombies)
	{
		if (zombie)
		{
			if (DOES_ENTITY_EXIST(zombie) && !IS_PED_DEAD_OR_DYING(zombie, true))
			{
				Vector3 zombiePos = GET_ENTITY_COORDS(zombie, false);
				if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, zombiePos.x, zombiePos.y, zombiePos.z, false) < 300.f)
				{
					if (IS_PED_RAGDOLL(zombie))
					{
						Vector3 zombiePos = GET_ENTITY_COORDS(zombie, false);

						ADD_EXPLOSION(zombiePos.x, zombiePos.y, zombiePos.z, 4, 9999.f, true, false, 1.f, false);

						SET_ENTITY_HEALTH(zombie, 0, false);
					}

					continue;
				}
			}

			zombiesAmount--;

			SET_PED_AS_NO_LONGER_NEEDED(&zombie);

			zombie = 0;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_ZOMBIES, OnStart, OnStop, OnTick);