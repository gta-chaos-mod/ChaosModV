#include <stdafx.h>

static void OnStart()
{
	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_ATTACK_PLAYER", &groupHash);
}

static void OnTick()
{
	static const Hash enemyGroupHash  = GET_HASH_KEY("_ATTACK_PLAYER");
	static const Hash playerGroupHash = GET_HASH_KEY("PLAYER");

	SET_RELATIONSHIP_BETWEEN_GROUPS(5, enemyGroupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, enemyGroupHash);

	Player player   = PLAYER_ID();
	Ped playerPed   = PLAYER_PED_ID();
	int playerGroup = GET_PLAYER_GROUP(player);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			if (IS_PED_IN_GROUP(ped) && GET_PED_GROUP_INDEX(ped) == playerGroup)
			{
				REMOVE_PED_FROM_GROUP(ped);
			}

			SET_PED_RELATIONSHIP_GROUP_HASH(ped, enemyGroupHash);

			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

			SET_PED_FLEE_ATTRIBUTES(ped, 2, true);

			TASK_COMBAT_PED(ped, playerPed, 0, 16);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "All Peds Attack Player",
		.Id = "peds_attackplayer",
		.IsTimed = true
	}
);