#include <stdafx.h>

static std::list<Ped> m_zombies;

static void OnStart()
{
	m_zombies.clear();

	static const Hash playerGroupHash    = GET_HASH_KEY("PLAYER");
	static const Hash civMaleGroupHash   = GET_HASH_KEY("CIVMALE");
	static const Hash civFemaleGroupHash = GET_HASH_KEY("CIVFEMALE");

	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_ZOMBIES", &groupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, civMaleGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, civFemaleGroupHash);
}

static void OnStop()
{
	for (Ped ped : m_zombies)
	{
		if (DOES_ENTITY_EXIST(ped))
		{
			SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}
	}
}

static void OnTick()
{
	static constexpr int MAX_ZOMBIES = 20;
	static constexpr Hash MODEL_HASH = -1404353274;

	static Hash zombieGroupHash      = GET_HASH_KEY("_ZOMBIES");

	Ped playerPed                    = PLAYER_PED_ID();
	Vector3 playerPos                = GET_ENTITY_COORDS(playerPed, false);

	if (m_zombies.size() <= MAX_ZOMBIES)
	{
		Vector3 spawnPos;
		if (GET_NTH_CLOSEST_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 10 + m_zombies.size(), &spawnPos, 0, 0,
		                                 0)
		    && GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, spawnPos.x, spawnPos.y, spawnPos.z,
		                                   false)
		           < 300.f)
		{
			LoadModel(MODEL_HASH);

			Ped zombie = CREATE_PED(26, MODEL_HASH, spawnPos.x, spawnPos.y, spawnPos.z, .0f, true, false);

			m_zombies.push_back(zombie);

			SET_PED_RELATIONSHIP_GROUP_HASH(zombie, zombieGroupHash);
			SET_PED_COMBAT_ATTRIBUTES(zombie, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(zombie, 46, true);

			// SET_AMBIENT_VOICE_NAME(zombie, "ALIENS");
			DISABLE_PED_PAIN_AUDIO(zombie, true);

			TASK_COMBAT_PED(zombie, playerPed, 0, 16);

			SET_MODEL_AS_NO_LONGER_NEEDED(MODEL_HASH);
		}
	}

	for (std::list<Ped>::iterator it = m_zombies.begin(); it != m_zombies.end();)
	{
		Ped zombie = *it;

		if (DOES_ENTITY_EXIST(zombie))
		{
			Vector3 zombiePos = GET_ENTITY_COORDS(zombie, false);
			if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, zombiePos.x, zombiePos.y,
			                                zombiePos.z, false)
			    < 300.f)
			{
				int maxHealth = GET_ENTITY_MAX_HEALTH(zombie);

				if (maxHealth > 0)
				{
					if (IS_PED_INJURED(zombie) || IS_PED_RAGDOLL(zombie))
					{
						Vector3 zombiePos = GET_ENTITY_COORDS(zombie, false);

						ADD_EXPLOSION(zombiePos.x, zombiePos.y, zombiePos.z, 4, 9999.f, true, false, 1.f, false);

						SET_ENTITY_HEALTH(zombie, 0, false);
						SET_ENTITY_MAX_HEALTH(zombie, 0);
					}

					it++;

					continue;
				}
			}

			SET_PED_AS_NO_LONGER_NEEDED(&zombie);
		}

		it = m_zombies.erase(it);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Explosive Zombies",
		.Id = "zombies",
		.IsTimed = true
	}
);