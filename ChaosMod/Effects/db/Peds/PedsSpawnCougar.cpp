/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

Entity cougar;
Player plr;

static void OnStart()
{
	static constexpr Hash modelHash = 307287994;

	plr = PLAYER::PLAYER_ID();
	Ped plrped = PLAYER_PED_ID();
	//Get spawn location for the cougar
	Vector3 plr_pos = GET_ENTITY_COORDS(plrped, 1);
	//spawn the cougar
	cougar = CreatePoolPed(4, modelHash, plr_pos.x, plr_pos.y-0.5, plr_pos.z-6, 0.f);
	//Set the relationship between the cougar and the player (aww, they hate each other)
	Hash relation;
	Hash playerG = GET_HASH_KEY("PLAYER");
	Hash pedmG = GET_HASH_KEY("CIVMALE");
	Hash pedfG = GET_HASH_KEY("CIVFEMALE");
	ADD_RELATIONSHIP_GROUP("_COUGAR_ATTACK", &relation);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relation, pedmG);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relation, pedfG);	
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relation, playerG);
	SET_PED_RELATIONSHIP_GROUP_HASH(cougar, relation);
	SET_PED_HEARING_RANGE(cougar, 9999.f);
	SET_PED_CONFIG_FLAG(cougar, 281, 1);
	//Make the cougar attack the player and nearby peds
	SET_PED_COMBAT_ATTRIBUTES(cougar, 5, 1);
	SET_PED_COMBAT_ATTRIBUTES(cougar, 46, 1);
	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(cougar, 0);
	SET_ENTITY_PROOFS(cougar, 0, 1, 1, 0, 0, 0, 0, 0);
	GIVE_WEAPON_TO_PED(cougar, -1569615261, 9999, 1,1);
	TASK_COMBAT_HATED_TARGETS_AROUND_PED(cougar, 15, 0);
}

// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EEffectType::EFFECT_PEDS_SPAWN_COUGAR, OnStart, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "There Aren't Cougars In Missions",
		.Id = "peds_spawn_cougar"
	}
);
