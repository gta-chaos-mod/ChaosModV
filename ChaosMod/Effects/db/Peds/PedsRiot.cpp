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

	static std::vector<Ped> goneThroughPeds;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && std::find(goneThroughPeds.begin(), goneThroughPeds.end(), ped) == goneThroughPeds.end())
		{
			SET_PED_RELATIONSHIP_GROUP_HASH(ped, riotGroupHash);

			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

			static const std::vector<Hash>& weps = Memory::GetAllWeapons();
			GIVE_WEAPON_TO_PED(ped, weps[g_random.GetRandomInt(0, weps.size() - 1)], 9999, false, true);

			SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

			goneThroughPeds.push_back(ped);
		}
	}

	for (std::vector<Ped>::iterator it = goneThroughPeds.begin(); it != goneThroughPeds.end(); )
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

static RegisterEffect registerEffect(EFFECT_PEDS_RIOT, OnStart, nullptr, OnTick);