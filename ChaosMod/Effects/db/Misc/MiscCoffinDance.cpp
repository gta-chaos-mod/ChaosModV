/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const Hash coffinH = GET_HASH_KEY("prop_coffin_01");
static const Hash dancerH = GET_HASH_KEY("ig_fbisuit_01");

static const char* animDict = "anim@amb@nightclub@mini@dance@dance_solo@shuffle@";
static const char* animName = "med_center";

static void LoadAssets()
{
	std::vector<Hash> models = { coffinH, dancerH };
	for (Hash model : models)
	{
		LoadModel(model);
	}

	REQUEST_ANIM_DICT(animDict);
	while (!HAS_ANIM_DICT_LOADED(animDict))
	{
		WAIT(0);
	}
}

static void OnStart()
{
	LoadAssets();
	Ped plr = PLAYER_PED_ID();

	Vector3 pos = GET_ENTITY_COORDS(plr, true);
	float heading = GET_ENTITY_HEADING(plr);

	std::vector<Ped> dancers;

	for (int i = 0; i < 4; i++)
	{
		Vector3 off;
		Ped p;
		switch (i)
		{
		case 0:
			off = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(plr, 0.f, -2.5f, -0.f);
			p = CreatePoolPed(4, dancerH, off.x, off.y, off.z, heading);
			break;
		case 1:
			off = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(dancers[0], 0.4, 0, 0);
			p = CreatePoolPed(4, dancerH, off.x, off.y, off.z, heading);
			break;
		case 2:
			off = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(dancers[0], 0.4, -1.2, 0);
			p = CreatePoolPed(4, dancerH, off.x, off.y, off.z, heading);
			break;
		case 3:
			off = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(dancers[0], 0, -1.2, 0);
			p = CreatePoolPed(4, dancerH, off.x, off.y, off.z, heading);
			break;
		}
		dancers.push_back(p);
	}

	
	Vector3 cPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(dancers[0], 0.15, -0.5, -0.5f);
	Object coffin = CreatePoolProp(coffinH, cPos.x, cPos.y, cPos.z, true);
	SET_ENTITY_COLLISION(coffin, false, false);
	FREEZE_ENTITY_POSITION(coffin, true);
	SET_ENTITY_ROTATION(coffin, 0.f, 0.f, heading, 2, true);

	for (Ped& ped : dancers)
	{
		TASK_PLAY_ANIM(ped, animDict, animName, 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
		SET_PED_KEEP_TASK(ped, true);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
		SET_PED_CAN_RAGDOLL(ped, false);
	}
}
// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Coffin Dance",
		.Id = "misc_coffin_dance"
	}
);
