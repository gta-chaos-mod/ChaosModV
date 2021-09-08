#include <stdafx.h>

static void OnStart()
{
	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_ATTACK_PLAYER", &groupHash);
}

static void OnTick()
{
	static const Hash enemyGroupHash = GET_HASH_KEY("_ATTACK_PLAYER");
	static const Hash playerGroupHash = GET_HASH_KEY("PLAYER");

	SET_RELATIONSHIP_BETWEEN_GROUPS(5, enemyGroupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, enemyGroupHash);

	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	int playerGroup = GET_PLAYER_GROUP(player);

	// Just so the effect will do something different with higher multipliers
	if (g_MetaInfo.m_fChaosMultiplier > 1)
	{
		SET_AI_WEAPON_DAMAGE_MODIFIER(g_MetaInfo.m_fChaosMultiplier);
		SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(g_MetaInfo.m_fChaosMultiplier);
	}

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

static void OnStop()
{
	SET_AI_WEAPON_DAMAGE_MODIFIER(1.f);
	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(1.f);
}

// Note (Gorakh): OnStart and OnStop were flipped here previously, which I'm pretty sure was causing peds to flee instead of attacking the player, since the relationship group didn't exist
static RegisterEffect registerEffect(EFFECT_PEDS_ATTACK_PLAYER, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "All Peds Attack Player",
		.Id = "peds_attackplayer",
		.IsTimed = true
	}
);