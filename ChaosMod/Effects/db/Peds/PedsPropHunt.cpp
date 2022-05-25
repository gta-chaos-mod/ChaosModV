/*
    Effect by Gorakh
*/

#include <stdafx.h>

static std::map<Ped, Object> pedPropsMap;
static std::vector<Hash> availablePropModels;

static Vector3 GetPropOffset(Hash propModel)
{
	Vector3 propMin, propMax;
	GET_MODEL_DIMENSIONS(propModel, &propMin, &propMax);

	// Lines up prop model center with ped model center
	return (((propMax - propMin) / 2.f) + propMin) * -1.f;
}

static void OnTick()
{
	static int lastModelsUpdateTick = 0;
	int currentTick                 = GET_GAME_TIMER();
	if (currentTick - lastModelsUpdateTick > 1000 || availablePropModels.empty())
	{
		lastModelsUpdateTick = currentTick;

		availablePropModels.clear();

		for (Entity prop : GetAllProps())
		{
			Hash model = GET_ENTITY_MODEL(prop);
			Vector3 min, max;
			GET_MODEL_DIMENSIONS(model, &min, &max);
			float modelSize = (max - min).Length();

			// Don't include models that are either very small or very large
			if (modelSize > 0.3f && modelSize <= 6.f)
			{
				availablePropModels.push_back(model);
			}
		}
	}

	if (!availablePropModels.empty())
	{
		int count = 10;
		for (Ped ped : GetAllPeds())
		{
			if (DOES_ENTITY_EXIST(ped) && !IS_PED_A_PLAYER(ped) && pedPropsMap.count(ped) == 0)
			{
				// Rather than using a predefined list of prop hashes, get a random existing prop and use that as the
				// model
				Hash propModel = availablePropModels[g_Random.GetRandomInt(0, availablePropModels.size() - 1)];
				LoadModel(propModel);

				Object prop = CREATE_OBJECT(propModel, 0.f, 0.f, 0.f, true, false, false);

				SET_ENTITY_COLLISION(prop, false, false);
				SET_ENTITY_COMPLETELY_DISABLE_COLLISION(prop, false, false);

				SET_ENTITY_VISIBLE(ped, false, 0);

				Vector3 offset = GetPropOffset(propModel);
				ATTACH_ENTITY_TO_ENTITY(prop, ped, GET_PED_BONE_INDEX(ped, 0x0), offset.x, offset.y, offset.z, 0.f, 0.f,
				                        0.f, false, false, false, false, 0, true);
				SET_ENTITY_VISIBLE(prop, true, 0);

				pedPropsMap[ped] = prop;

				if (--count == 0)
				{
					count = 10;
					WAIT(0);
				}
			}
		}
	}

	static int lastPropPedsCheckTick = 0;
	currentTick                      = GET_GAME_TIMER();
	if (currentTick - lastPropPedsCheckTick > 500)
	{
		lastPropPedsCheckTick = currentTick;

		int count             = 20;
		for (auto it = pedPropsMap.cbegin(); it != pedPropsMap.cend();)
		{
			Ped ped     = it->first;
			Object prop = it->second;
			if (!DOES_ENTITY_EXIST(ped))
			{
				if (DOES_ENTITY_EXIST(prop))
				{
					DELETE_OBJECT(&prop);
				}

				it = pedPropsMap.erase(it);
			}
			else if (IS_PED_A_PLAYER(
			             ped)) // If player has a prop attached to them (this can happen after switching characters)
			{
				SET_ENTITY_VISIBLE(ped, true, 0);

				if (DOES_ENTITY_EXIST(prop))
				{
					DELETE_OBJECT(&prop);
				}

				it = pedPropsMap.erase(it);
			}
			else if (!DOES_ENTITY_EXIST(prop)) // The ped exists and is not a player, but there is no prop
			{
				SET_ENTITY_VISIBLE(ped, true, 0);
				it = pedPropsMap.erase(it);
			}
			else // Both the ped and prop exist, and the ped is not the player
			{
				// Ensure ped is invisible, prop is attached to the ped, and the prop is visible
				SET_ENTITY_VISIBLE(ped, false, 0);

				if (!IS_ENTITY_ATTACHED_TO_ENTITY(prop, ped))
				{
					Vector3 offset = GetPropOffset(GET_ENTITY_MODEL(prop));
					ATTACH_ENTITY_TO_ENTITY(prop, ped, GET_PED_BONE_INDEX(ped, 0x0), offset.x, offset.y, offset.z, 0.f,
					                        0.f, 0.f, false, false, false, false, 0, true);
				}

				SET_ENTITY_VISIBLE(prop, true, 0);

				it++;
			}

			if (--count == 0)
			{
				count = 20;
				WAIT(0);
			}
		}
	}
}

static void OnStop()
{
	for (auto &it : pedPropsMap)
	{
		Ped ped     = it.first;
		Object prop = it.second;

		if (DOES_ENTITY_EXIST(ped))
		{
			SET_ENTITY_VISIBLE(ped, true, 0);
		}

		if (DOES_ENTITY_EXIST(prop))
		{
			DELETE_OBJECT(&prop);
		}
	}

	pedPropsMap.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Prop Hunt",
		.Id = "peds_prop_hunt",
		.IsTimed = true,
		.IncompatibleWith = { "peds_ghost" }
	}
);
