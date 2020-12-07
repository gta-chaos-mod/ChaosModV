#include <stdafx.h>

//Effect by ProfessorBiddle


//future plans: make jesus spawn above and parachute to player, possibly jesus sniper in helicopter
static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);
	static constexpr Hash modelHash = -835930287;

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_DIVINE_JESUS", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);
	for (int i = 0; i < 4; i++)
	{
		//I am shit at programming if someone wants to do this in a more efficient way please do
		//spawn 5 jesus
		Ped ped0 = CreatePoolPed(4, modelHash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
		Ped ped1 = CreatePoolPed(4, modelHash, playerPos.x + 5.f, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
		Ped ped2 = CreatePoolPed(4, modelHash, playerPos.x - 5.f, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
		Ped ped3 = CreatePoolPed(4, modelHash, playerPos.x, playerPos.y, playerPos.z + 5.f, GET_ENTITY_HEADING(playerPed));
		Ped ped4 = CreatePoolPed(4, modelHash, playerPos.x, playerPos.y, playerPos.z - 5.f, GET_ENTITY_HEADING(playerPed));

		auto playerGroup = GET_PLAYER_GROUP(PLAYER_ID());
		
		SET_PED_SUFFERS_CRITICAL_HITS(ped0, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped0, relationshipGroup);
		SET_PED_HEARING_RANGE(ped0, 9999.f);
		SET_PED_AS_GROUP_MEMBER(ped0, playerGroup);
		SET_ENTITY_PROOFS(ped0, true, true, true, true, true, true, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped0, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped0, 46, true);
		SET_PED_ACCURACY(ped0, 100);
		SET_PED_FIRING_PATTERN(ped0, 0xC6EE6B4C);

		SET_PED_SUFFERS_CRITICAL_HITS(ped1, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped1, relationshipGroup);
		SET_PED_HEARING_RANGE(ped1, 9999.f);
		SET_PED_AS_GROUP_MEMBER(ped1, playerGroup);
		SET_ENTITY_PROOFS(ped0, true, true, true, true, true, true, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped1, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped1, 46, true);
		SET_PED_ACCURACY(ped1, 100);
		SET_PED_FIRING_PATTERN(ped1, 0xC6EE6B4C);

		SET_PED_SUFFERS_CRITICAL_HITS(ped2, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped2, relationshipGroup);
		SET_PED_HEARING_RANGE(ped2, 9999.f);
		SET_PED_AS_GROUP_MEMBER(ped2, playerGroup);
		SET_ENTITY_PROOFS(ped0, true, true, true, true, true, true, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped2, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped2, 46, true);
		SET_PED_ACCURACY(ped2, 100);
		SET_PED_FIRING_PATTERN(ped2, 0xC6EE6B4C);

		SET_PED_SUFFERS_CRITICAL_HITS(ped3, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped3, relationshipGroup);
		SET_PED_HEARING_RANGE(ped3, 9999.f);
		SET_PED_AS_GROUP_MEMBER(ped3, playerGroup);
		SET_ENTITY_PROOFS(ped0, true, true, true, true, true, true, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped3, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped3, 46, true);
		SET_PED_ACCURACY(ped3, 100);
		SET_PED_FIRING_PATTERN(ped3, 0x9C74B406);

		SET_PED_SUFFERS_CRITICAL_HITS(ped4, false);
		SET_PED_RELATIONSHIP_GROUP_HASH(ped4, relationshipGroup);
		SET_PED_HEARING_RANGE(ped4, 9999.f);
		SET_PED_AS_GROUP_MEMBER(ped4, playerGroup);
		SET_ENTITY_PROOFS(ped0, true, true, true, true, true, true, 1, true);
		SET_PED_COMBAT_ATTRIBUTES(ped4, 5, true);
		SET_PED_COMBAT_ATTRIBUTES(ped4, 46, true);
		SET_PED_ACCURACY(ped4, 100);
		SET_PED_FIRING_PATTERN(ped4, 0xC6EE6B4C);

		//1. Up'n'atmomizer 2. Unholy Hellbringer 3. Railgun 4. Battleaxe 5. Widowmaker
		GIVE_WEAPON_TO_PED(ped0, GET_HASH_KEY("WEAPON_RAYPISTOL"), 9999, true, true);
		GIVE_WEAPON_TO_PED(ped1, GET_HASH_KEY("WEAPON_RAYCARBINE"), 9999, true, true);
		GIVE_WEAPON_TO_PED(ped2, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
		GIVE_WEAPON_TO_PED(ped3, GET_HASH_KEY("WEAPON_RPG"), 9999, true, true);
		GIVE_WEAPON_TO_PED(ped4, GET_HASH_KEY("WEAPON_RAYMINIGUN"), 9999, true, true);
	}
}
static RegisterEffect registerEffect(EFFECT_PEDS_INTERVENTION, OnStart);