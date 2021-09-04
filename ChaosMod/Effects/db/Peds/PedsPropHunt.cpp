/*
	Effect by Gorakh
*/

#include <stdafx.h>

struct PropPed
{
	Ped ped;
	Object prop;
};
std::vector<PropPed> propPeds;

std::vector<Hash> availablePropModels;
static void OnTick()
{
	static int lastTick = 0;
	int currentTick = GET_GAME_TIMER();

	if (currentTick - lastTick > 1000)
	{
		lastTick = GET_GAME_TIMER();

		availablePropModels.clear();

		std::vector<Entity> props = GetAllProps();
		for (Entity prop : props)
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

	if (availablePropModels.size() > 0)
	{
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_A_PLAYER(ped))
			{
				bool alreadyHasProp = false;
				for (PropPed existingPropPed : propPeds)
				{
					if (existingPropPed.ped == ped)
					{
						alreadyHasProp = DOES_ENTITY_EXIST(existingPropPed.prop);
						break;
					}
				}

				if (!alreadyHasProp)
				{
					// Rather than using a predefined list of prop hashes, get a random existing prop and use that as the model
					Hash propModel = availablePropModels[g_Random.GetRandomInt(0, availablePropModels.size() - 1)];
					LoadModel(propModel);

					Object pedProp = CREATE_OBJECT(propModel, 0.f, 0.f, 0.f, true, false, false);

					SET_ENTITY_COLLISION(pedProp, false, false);
					SET_ENTITY_COMPLETELY_DISABLE_COLLISION(pedProp, false, false);

					SET_ENTITY_VISIBLE(ped, false, 0);

					PropPed propPed = { ped, pedProp };
					propPeds.push_back(propPed);
				}
			}
		}
	}

	for (std::vector<PropPed>::iterator it = propPeds.begin(); it != propPeds.end(); )
	{
		PropPed propPed = *it;
		if (DOES_ENTITY_EXIST(propPed.ped))
		{
			if (IS_PED_A_PLAYER(propPed.ped))
			{
				SET_ENTITY_VISIBLE(propPed.ped, true, 0);

				if (DOES_ENTITY_EXIST(propPed.prop))
				{
					DELETE_OBJECT(&propPed.prop);
				}

				it = propPeds.erase(it);
				continue;
			}

			SET_ENTITY_VISIBLE(propPed.ped, false, 0);

			if (DOES_ENTITY_EXIST(propPed.prop))
			{
				// Get bone coords instead of entity coords to make prop line up with model while ped is in a vehicle
				Vector3 pedCoords = GET_PED_BONE_COORDS(propPed.ped, 0x0, 0.f, 0.f, 0.f);

				// For some reason ATTACH_ENTITY_TO_ENTITY wasn't working, so we set the position and rotation every tick instead
				SET_ENTITY_COORDS(propPed.prop, pedCoords.x, pedCoords.y, pedCoords.z, false, false, false, false);
				SET_ENTITY_ROTATION(propPed.prop, GET_ENTITY_PITCH(propPed.ped), GET_ENTITY_ROLL(propPed.ped), GET_ENTITY_HEADING(propPed.ped), 0, true);
			}

			it++;
		}
		else
		{
			if (DOES_ENTITY_EXIST(propPed.prop))
			{
				DELETE_OBJECT(&propPed.prop);
			}

			it = propPeds.erase(it);
		}
	}
}

static void OnStop()
{
	for (PropPed propPed : propPeds)
	{
		if (DOES_ENTITY_EXIST(propPed.ped))
		{
			SET_ENTITY_VISIBLE(propPed.ped, true, 0);
		}

		if (DOES_ENTITY_EXIST(propPed.prop))
		{
			DELETE_OBJECT(&propPed.prop);
		}
	}

	propPeds.clear();
}

static RegisterEffect registerEffect(EFFECT_PEDS_PROP_HUNT, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Prop Hunt",
		.Id = "peds_prop_hunt",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_PEDS_INVISIBLE }
	}
);
