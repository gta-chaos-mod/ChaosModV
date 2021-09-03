/*
	Effect by Gorakh
*/

#include <stdafx.h>

#define NUM_CLONES 8

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	static const Hash maleCivGroup = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash cloneGroup;
	ADD_RELATIONSHIP_GROUP("_PLAYER_CLONES", &cloneGroup);

	SET_RELATIONSHIP_BETWEEN_GROUPS(0, playerGroup, cloneGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, cloneGroup, playerGroup);

	SET_RELATIONSHIP_BETWEEN_GROUPS(5, cloneGroup, maleCivGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, cloneGroup, femCivGroup);

	Ped playerPed = PLAYER_PED_ID();

	Vector3 playerCoords = GET_ENTITY_COORDS(playerPed, true);
	int playerPedType = GET_PED_TYPE(playerPed);
	Hash playerModel = GET_ENTITY_MODEL(playerPed);
	float playerHeading = GET_ENTITY_HEADING(playerPed);
	int playerGroupID = GET_PLAYER_GROUP(PLAYER_ID());

	std::vector<Ped> clones;

	int count = 3;
	for (int i = 0; i < NUM_CLONES; i++)
	{
		Ped clone = CreatePoolPed(playerPedType, playerModel, playerCoords.x + g_Random.GetRandomFloat(-2, 2), playerCoords.y + g_Random.GetRandomFloat(-2, 2), playerCoords.z, playerHeading);
		CLONE_PED_TO_TARGET(playerPed, clone);
		SET_PED_RELATIONSHIP_GROUP_HASH(clone, cloneGroup);

		SET_PED_SUFFERS_CRITICAL_HITS(clone, false);
		SET_PED_AS_GROUP_MEMBER(clone, playerGroupID);

		GIVE_WEAPON_TO_PED(clone, GET_SELECTED_PED_WEAPON(playerPed), 9999, false, true);

		SET_PED_FIRING_PATTERN(clone, 0xC6EE6B4C);

		SET_PED_PATH_CAN_DROP_FROM_HEIGHT(clone, true);
		SET_PED_PATH_CAN_USE_CLIMBOVERS(clone, true);
		SET_PED_PATH_CAN_USE_LADDERS(clone, true);
		SET_PED_PATH_MAY_ENTER_WATER(clone, true);

		SET_PED_MOVE_RATE_OVERRIDE(clone, 1.15f);

		SET_PED_HEARING_RANGE(clone, 9999.f);
		SET_PED_SEEING_RANGE(clone, 9999.f);

		clones.push_back(clone);

		if (--count == 0)
		{
			WAIT(0);
			count = 3;
		}
	}

	// Make clones attack everyone who isn't another clone or the player
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped) && !IS_PED_DEAD_OR_DYING(ped, true) && std::find(clones.begin(), clones.end(), ped) == clones.end())
		{
			Hash pedGroup = GET_PED_RELATIONSHIP_GROUP_HASH(ped);
			if (_DOES_RELATIONSHIP_GROUP_EXIST(pedGroup))
			{
				SET_RELATIONSHIP_BETWEEN_GROUPS(5, cloneGroup, pedGroup);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_CLONE_ARMY, OnStart, EffectInfo
	{
		.Name = "Spawn Clone Army",
		.Id = "player_clone_army"
	}
);