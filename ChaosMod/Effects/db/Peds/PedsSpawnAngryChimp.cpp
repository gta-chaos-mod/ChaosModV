/*
	Effect by Marvis
*/

#include <stdafx.h>

#include "Util/Peds.h"

static void OnStart()
{
	static const Hash chimpHash = 2825402133;
	static const Hash weaponHash = GET_HASH_KEY("WEAPON_STONE_HATCHET");
	CreateHostilePed(chimpHash, weaponHash);
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Spawn Angry Chimp",
		.Id = "spawn_angry_chimp",
		.EffectGroupType = EEffectGroupType::SpawnEnemy,
	}
);