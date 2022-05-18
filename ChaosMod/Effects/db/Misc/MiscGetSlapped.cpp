/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const Hash slapperHash = GET_HASH_KEY("ig_fbisuit_01");

static std::string dict = "melee@unarmed@streamed_variations";
static std::string slap = "plyr_takedown_front_slap";
static std::string slapped = "victim_takedown_front_slap";

static void LoadAssets()
{
	REQUEST_ANIM_DICT(dict.c_str());
	while (!HAS_ANIM_DICT_LOADED(dict.c_str()))
	{
		WAIT(0);
	}

	REQUEST_MODEL(slapperHash);
	while (!HAS_MODEL_LOADED(slapperHash))
	{
		WAIT(0);
	}
}

//ped2 will slap ped1
static void EmoteTwoPeds(Ped ped1, Ped ped2)
{
	Vector3 coords = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped1, 0.f, 1.f, 0.f);
	float heading = GET_ENTITY_HEADING(ped1);

	SET_ENTITY_COORDS(ped2, coords.x, coords.y, coords.z, false, false, false, false);
	SET_ENTITY_HEADING(ped2, heading - 180.1f);


	
	WAIT(300);
	SET_CURRENT_PED_WEAPON(ped1, GET_HASH_KEY("WEAPON_UNARMED"), true);
	SET_CURRENT_PED_WEAPON(ped2, GET_HASH_KEY("WEAPON_UNARMED"), true);

	TASK_PLAY_ANIM(ped2, dict.c_str(), slap.c_str(), 2.f, 2.f, 2000, 51, 0.f, false, false, false);
	TASK_PLAY_ANIM(ped1, dict.c_str(), slapped.c_str(), 2.f, 2.f, 2000, 0, 0.f, false, false, false);
	WAIT(1800);
}

static void OnStart()
{
	LoadAssets();

	Ped player = PLAYER_PED_ID();

	SET_PLAYER_CONTROL(PLAYER_ID(), false, 1 << 8);
	Ped slapper = CreatePoolPed(4, slapperHash, 0.f, 0.f, 0.f, 0.f);

	Vector3 pos = GET_ENTITY_COORDS(player, true);
	float outHead;
	Vector3 outPos;
	GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, &outPos, &outHead, 0, 3.f, 0.f);
	SET_ENTITY_COORDS(player, outPos.x, outPos.y, outPos.z, false, false, false, true);
	SET_ENTITY_HEADING(player, outHead);

	EmoteTwoPeds(player, slapper);
	TASK_WANDER_STANDARD(slapper, 10.0f, 10);
	SET_PED_TO_RAGDOLL(player, 1200, 1200, 0, false, false, false);
	SET_ENTITY_AS_NO_LONGER_NEEDED(&slapper);
	SET_PLAYER_CONTROL(PLAYER_ID(), true, 1 << 8);
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Get Slapped",
		.Id = "misc_get_slapped"
	}
);
