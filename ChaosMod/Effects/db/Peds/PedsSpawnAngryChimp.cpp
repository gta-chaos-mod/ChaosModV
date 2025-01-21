/*
    Effect by Marvis
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Peds.h"

static void OnStart()
{
	static const Hash chimpHash  = 2825402133;
	static const Hash weaponHash = "WEAPON_STONE_HATCHET"_hash;
	auto ped                     = CreateHostilePed(chimpHash, weaponHash);
	CurrentEffect::SetEffectSoundPlayOptions(
	    { .PlayType = EffectSoundPlayType::FollowEntity, .PlayFlags = EffectSoundPlayFlags_Looping, .Entity = ped });
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Spawn Angry Chimp",
		.Id = "spawn_angry_chimp",
		.EffectGroupType = EffectGroupType::SpawnEnemy,
	}
);