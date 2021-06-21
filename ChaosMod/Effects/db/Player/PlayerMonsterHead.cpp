/*
	Effect by Moxi
*/

#include <stdafx.h>

static const Hash monsterHash = GET_HASH_KEY("sum_prop_ac_monstermask_01a");

static void OnStart()
{
	LoadModel(monsterHash);

	Ped player = PLAYER_PED_ID();
	int bone = GET_PED_BONE_INDEX(player, 0x60F2);
	Vector3 playerPos = GET_ENTITY_COORDS(player, false);

	Object monsterHead = CREATE_OBJECT(monsterHash, playerPos.x, playerPos.y + 2, playerPos.z, true, false, false);
	ATTACH_ENTITY_TO_ENTITY(monsterHead, player, bone, 0.36, 0.1, -0.01, 180, -90, 0, true, false, false, false, 0, true);
	SET_ENTITY_AS_NO_LONGER_NEEDED(&monsterHead);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_MONSTER_HEAD, OnStart, EffectInfo
	{
		.Name = "Monster Head",
		.Id = "player_monster_head"
	}
);