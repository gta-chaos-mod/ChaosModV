/*
    Effect by Juhana 
	based on PedsHeadless.cpp and PedsPropHunt.cpp
*/

#include <stdafx.h>

static std::map<Ped, Object> pedPropsMap;
static const int TOAST_MODEL_COUNT = 6;
static Hash availablePropModels[TOAST_MODEL_COUNT];
static const char *toastModelKeys[TOAST_MODEL_COUNT] = { "v_res_fa_bread01", "v_res_fa_bread02", "v_res_fa_bread03", "v_ret_247_bread1", "prop_toaster_01", "prop_toaster_02" };

// removes peds' heads; from PedsHeadless.cpp
static void RemoveHead()
{
	for (Ped ped : GetAllPeds())
	{
		SET_PED_RESET_FLAG(ped, 166, true);
	}
}

static void OnStart()
{
	for (int i = 0; i < TOAST_MODEL_COUNT; ++i)
	{
		Hash toastModel = GET_HASH_KEY(toastModelKeys[i]);
		availablePropModels[i] = toastModel;
	}
}

static void OnTick()
{
	RemoveHead();

	int count = 10;
	for (Ped ped : GetAllPeds())
	{
		if (DOES_ENTITY_EXIST(ped) && pedPropsMap.count(ped) == 0)
		{
			int propIndex  = g_Random.GetRandomInt(0, TOAST_MODEL_COUNT - 1);
			Hash propModel = availablePropModels[propIndex];
			LoadModel(propModel);
			Object prop = CREATE_OBJECT(propModel, 0.f, 0.f, 0.f, true, false, false);

			SET_ENTITY_COLLISION(prop, false, false);
			SET_ENTITY_COMPLETELY_DISABLE_COLLISION(prop, false, false);

			ATTACH_ENTITY_TO_ENTITY(prop, ped, GET_PED_BONE_INDEX(ped, 0x796e), 0, 0, 0, 
			                        -90.f, 0, 0, false, false, false, false, 0, true);

			pedPropsMap[ped] = prop;

			if (--count == 0)
			{
				count = 10;
				WAIT(0);
				RemoveHead();
			}
		}
	}

	// set a new head; adapted from PedsPropHunt.cpp
	static int lastPropPedsCheckTick = 0;
	int currentTick = GET_GAME_TIMER();
	if (currentTick - lastPropPedsCheckTick > 500)
	{
		lastPropPedsCheckTick = currentTick;

		int count = 20;
		for (auto it = pedPropsMap.cbegin(); it != pedPropsMap.cend();)
		{
			Ped ped = it->first;
			Object prop = it->second;
			if (!DOES_ENTITY_EXIST(ped))
			{
				if (DOES_ENTITY_EXIST(prop))
				{
					DELETE_OBJECT(&prop);
				}

				it = pedPropsMap.erase(it);
			}
			else if (!DOES_ENTITY_EXIST(prop))
			{
				it = pedPropsMap.erase(it);
			}
			else 
			{
				if (!IS_ENTITY_ATTACHED_TO_ENTITY(prop, ped))
				{
					ATTACH_ENTITY_TO_ENTITY(prop, ped, GET_PED_BONE_INDEX(ped, 0x796e), 0, 0, 0,
					                         -90.f, 0, 0, false, false, false, false, 0, true);  // 0x796E = SKEL_Head
				}

				it++;
			}

			if (--count == 0)
			{
				count = 20;
				WAIT(0);
				RemoveHead();
			}
		}
	}
}

static void OnStop()
{
	for (auto &it : pedPropsMap)
	{
		Object prop = it.second;

		if (DOES_ENTITY_EXIST(prop))
		{
			DELETE_OBJECT(&prop);
		}
	}

	pedPropsMap.clear();

	for (int i = 0; i < TOAST_MODEL_COUNT; ++i)
	{
		availablePropModels[i] = NULL;
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "You're Toast",
		.Id = "peds_toast",
		.IsTimed = true,
		.IncompatibleWith = { "peds_minions" }
	}
);
