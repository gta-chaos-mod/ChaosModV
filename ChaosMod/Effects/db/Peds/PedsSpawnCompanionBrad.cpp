#include <stdafx.h>

#include "Util/Peds.h"

#include "Effects/Register/RegisterEffect.h"

static void OnStart()
{
	static const Hash model = "ig_brad"_hash;

	Ped playerPed           = PLAYER_PED_ID();
	Vector3 playerPos       = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CreatePoolPed(4, model, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	CurrentEffect::SetEffectSoundPlayOptions(
	    { .PlayType = EffectSoundPlayType::FollowEntity, .PlayFlags = EffectSoundPlayFlags_Looping, .Entity = ped });
	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);

	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	SetCompanionRelationship(ped, "_COMPANION_BRAD");

	SET_PED_HEARING_RANGE(ped, 9999.f);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));

	GIVE_WEAPON_TO_PED(ped, "WEAPON_MICROSMG"_hash, 9999, true, true);
	GIVE_WEAPON_TO_PED(ped, "WEAPON_RPG"_hash, 9999, true, true);

	SET_PED_ACCURACY(ped, 100);
	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Spawn Companion Brad",
		.Id = "spawn_compbrad",
		.EffectGroupType = EffectGroupType::SpawnCompanion
	}
);