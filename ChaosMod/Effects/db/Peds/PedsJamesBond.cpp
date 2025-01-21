/*
    Effect by Slothersbee
*/

#include <math.h>
#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#define PI 3.14159265

static void OnStart()
{
	Ped playerPed                 = PLAYER_PED_ID();

	static const Hash model       = "cs_milton"_hash;
	static const Hash playerGroup = "PLAYER"_hash;

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_BOND", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);

	auto playerPos = GET_ENTITY_COORDS(playerPed, false);

	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false)
	                                                                           : playerPed);

	float xPos    = sin((360 - heading) * PI / 180) * 10;
	float yPos    = cos((360 - heading) * PI / 180) * 10;

	auto veh      = CreatePoolVehicle("JB700"_hash, playerPos.x - xPos, playerPos.y - yPos, playerPos.z, heading);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);

	auto vel = GET_ENTITY_VELOCITY(playerPed);
	SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);

	Ped bond = CreatePoolPedInsideVehicle(veh, 4, model, -1);
	CurrentEffect::SetEffectSoundPlayOptions({ .PlayType = EffectSoundPlayType::FollowEntity, .Entity = bond });

	SET_PED_RELATIONSHIP_GROUP_HASH(bond, relationshipGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(bond, true);

	SET_PED_HEARING_RANGE(bond, 9999.f);
	SET_PED_CONFIG_FLAG(bond, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(bond, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(bond, 46, true);

	SET_PED_SUFFERS_CRITICAL_HITS(bond, false);

	GIVE_WEAPON_TO_PED(bond, "WEAPON_SWITCHBLADE"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(bond, "WEAPON_VINTAGEPISTOL"_hash, 9999, true, true);
	GIVE_WEAPON_COMPONENT_TO_PED(bond, "WEAPON_VINTAGEPISTOL"_hash, "COMPONENT_AT_PI_SUPP"_hash);
	SET_PED_ACCURACY(bond, 100);
	TASK_COMBAT_PED(bond, playerPed, 0, 16);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Spawn Deadly Agent",
		.Id = "peds_jamesbond",
		.EffectGroupType = EffectGroupType::SpawnEnemySpecial
	}
);