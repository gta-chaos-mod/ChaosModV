#include <stdafx.h>

static void OnStart()
{
	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_RIOT", &groupHash);
}

static void OnStop()
{
	SET_MAX_WANTED_LEVEL(5);
}

static void OnTick()
{
	static const Hash riotGroupHash = GET_HASH_KEY("_RIOT");
	static const Hash playerGroupHash = GET_HASH_KEY("PLAYER");

	SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, riotGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, riotGroupHash);

	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_MAX_WANTED_LEVEL(0);

	static std::list<Ped> goneThroughPeds;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_RELATIONSHIP_GROUP_HASH(ped, riotGroupHash);

			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

			SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

			if (std::find(goneThroughPeds.begin(), goneThroughPeds.end(), ped) == goneThroughPeds.end())
			{
				static const std::vector<Hash>& weps = Memory::GetAllWeapons();
				GIVE_WEAPON_TO_PED(ped, weps[g_Random.GetRandomInt(0, weps.size() - 1)], 9999, false, true);

				goneThroughPeds.push_back(ped);
			}
		}
	}

	for (std::list<Ped>::iterator it = goneThroughPeds.begin(); it != goneThroughPeds.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = goneThroughPeds.erase(it);
		}
		else
		{
			it++;
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_RIOT, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Peds Riot",
		.Id = "peds_riot",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_PEDS_COPS }
	}
);