/*
	Effect by Slothersbee
*/

#include <stdafx.h>
#include <math.h>  

#define PI 3.14159265

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();

	static const Hash model = GET_HASH_KEY("cs_milton");

	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_HOSTILE_BOND", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);

	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	float heading = GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : playerPed);

	float x_pos = sin((360 - heading) * PI / 180) * 10;
	float y_pos = cos((360 - heading) * PI / 180) * 10;

	Vehicle veh = CreatePoolVehicle(GET_HASH_KEY("JB700"), playerPos.x - x_pos, playerPos.y - y_pos, playerPos.z, heading);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);

	Vector3 vel = GET_ENTITY_VELOCITY(playerPed);
	SET_ENTITY_VELOCITY(veh, vel.x, vel.y, vel.z);

	Ped bond = CreatePoolPedInsideVehicle(veh, 4, model, -1);

	SET_PED_RELATIONSHIP_GROUP_HASH(bond, relationshipGroup);

	TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(bond, true);

	SET_PED_HEARING_RANGE(bond, 9999.f);
	SET_PED_CONFIG_FLAG(bond, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(bond, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(bond, 46, true);

	SET_PED_SUFFERS_CRITICAL_HITS(bond, false);

	GIVE_WEAPON_TO_PED(bond, GET_HASH_KEY("WEAPON_SWITCHBLADE"), 9999, true, true);
	GIVE_WEAPON_TO_PED(bond, GET_HASH_KEY("WEAPON_VINTAGEPISTOL"), 9999, true, true);
	GIVE_WEAPON_COMPONENT_TO_PED(bond, GET_HASH_KEY("WEAPON_VINTAGEPISTOL"), GET_HASH_KEY("COMPONENT_AT_PI_SUPP"));
	SET_PED_ACCURACY(bond, 100);
	TASK_COMBAT_PED(bond, playerPed, 0, 16);
}

static RegisterEffect registerEffect(EFFECT_JAMES_BOND, OnStart, EffectInfo
	{
		.Name = "Spawn Deadly Agent",
		.Id = "peds_jamesbond",
		.EEffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);