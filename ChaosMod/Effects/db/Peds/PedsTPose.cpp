/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static std::string dict = "nm@hands";
static std::string anim = "hands_up";

static void LoadAnimDict()
{
	while (!HAS_ANIM_DICT_LOADED(dict.c_str()))
	{
		REQUEST_ANIM_DICT(dict.c_str());
		WAIT(0);
	}
}

static void OnStop()
{
	for (Ped p : GetAllPeds())
	{
		if (IS_ENTITY_PLAYING_ANIM(p, dict.c_str(), anim.c_str(), 3))
		{
			CLEAR_PED_TASKS_IMMEDIATELY(p);
		}
	}
}

static void OnTick()
{
	LoadAnimDict();
	for (Ped p : GetAllPeds())
	{
		if (!IS_ENTITY_PLAYING_ANIM(p, dict.c_str(), anim.c_str(), 3))
		{
			TASK_PLAY_ANIM(p, dict.c_str(), anim.c_str(), 5.f, -1.f, -1, 50, 0.f, false, false, false);
		}
	}
}

static RegisterEffect reg(EFFECT_PEDS_TPOSE, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "T-Pose",
		.Id = "peds_tpose",
		.IsTimed = true
	}
);