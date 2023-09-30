/*
	Effect By Rylxnd
*/
#include <stdafx.h>

static std::vector<Hash> models;

static void OnStart()
{
	static int count          = 0;
	static const int maxCount = 25;
	models.clear();

	for (Object prop : GetAllProps())
	{
		Hash model = GET_ENTITY_MODEL(prop);
		Vector3 min, max;
		GET_MODEL_DIMENSIONS(model, &min, &max);
		float modelSize = (max - min).Length();
		if (modelSize > 0.3f && modelSize <= 6.f)
		{
			models.push_back(model);
		}
	}

	for (Ped ped : GetAllPeds())
	{
		if (count >= maxCount)
		{
			break;
		}
		Hash selectedModel = models.at(g_Random.GetRandomInt(0, models.size() - 1));
		Object obj = CreatePoolPropAttachedToPed(selectedModel, ped, GET_PED_BONE_INDEX(ped, 0xDEAD), 0, 0, 0, 0, 0, 0,
		                                         false, false, true);
		SET_ENTITY_COMPLETELY_DISABLE_COLLISION(obj, false, true);
		count++;
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo 
	{ 
		.Name = "Give Everyone A Random Prop",
		.Id = "peds_give_props" 
	}
);
