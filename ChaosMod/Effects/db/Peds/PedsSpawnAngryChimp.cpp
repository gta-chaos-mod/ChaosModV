/*
	Effect by Marvis
*/

#include <stdafx.h>

static void OnStart()
{

	static const Hash chimpHash = 2825402133;
	static const Hash weaponHash = GET_HASH_KEY("WEAPON_STONE_HATCHET");
	CreateHostilePed(chimpHash, weaponHash);
}


static RegisterEffect registerEffect(EFFECT_ANGRY_CHIMP, OnStart, EffectInfo
	{
		.Name = "Spawn Angry Chimp",
		.Id = "spawn_angry_chimp",
		.EEffectGroupType = EEffectGroupType::SpawnEnemy,
	}
);