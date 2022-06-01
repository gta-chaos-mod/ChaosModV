/*
    Effect by Gorakh, code mostly repurposed from "Prop Hunt"
*/

#include <stdafx.h>

static std::map<Vehicle, Object> vehPropsMap;
static std::vector<Hash> availablePropModels;

static Vector3 GetPropOffset(Hash propModel)
{
	Vector3 propMin, propMax;
	GET_MODEL_DIMENSIONS(propModel, &propMin, &propMax);

	// Offset from model such that its non-offset position is the center of the prop
	return (((propMax - propMin) / 2.f) + propMin) * -1.f;
}

static void RefreshVehiclePropLink(Vehicle veh, Object prop)
{
	SET_ENTITY_COLLISION(prop, false, false);
	SET_ENTITY_COMPLETELY_DISABLE_COLLISION(prop, false, false);

	SET_ENTITY_ALPHA(veh, 0, false);

	Vector3 offset = GetPropOffset(GET_ENTITY_MODEL(prop));
	ATTACH_ENTITY_TO_ENTITY(prop, veh, GET_ENTITY_BONE_INDEX_BY_NAME(veh, "chassis"), offset.x, offset.y, offset.z, 0.f,
	                        0.f, 0.f, false, false, false, false, 0, true);
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
			if (modelSize > 0.75f && modelSize < 6.f)
			{
				availablePropModels.push_back(model);
			}
		}
	}

	if (!availablePropModels.empty())
	{
		int count = 10;
		for (Vehicle veh : GetAllVehs())
		{
			if (DOES_ENTITY_EXIST(veh) && vehPropsMap.count(veh) == 0)
			{
				// Rather than using a predefined list of prop hashes, get a random existing prop and use that as the
				// model
				Hash propModel = availablePropModels[g_Random.GetRandomInt(0, availablePropModels.size() - 1)];
				LoadModel(propModel);

				Object prop = CREATE_OBJECT(propModel, 0.f, 0.f, 0.f, true, false, false);

				RefreshVehiclePropLink(veh, prop);

				vehPropsMap[veh] = prop;

				if (--count == 0)
				{
					count = 10;
					WAIT(0);
				}
			}
		}
	}

	static int lastPropVehsCheckTick = 0;
	currentTick                      = GET_GAME_TIMER();
	if (currentTick - lastPropVehsCheckTick > 500)
	{
		lastPropVehsCheckTick = currentTick;

		int count             = 20;
		for (auto it = vehPropsMap.cbegin(); it != vehPropsMap.cend();)
		{
			Vehicle veh = it->first;
			Object prop = it->second;
			if (!DOES_ENTITY_EXIST(veh))
			{
				if (DOES_ENTITY_EXIST(prop))
				{
					DELETE_OBJECT(&prop);
				}

				it = vehPropsMap.erase(it);
			}
			else if (!DOES_ENTITY_EXIST(prop)) // The vehicle exists, but there is no prop
			{
				SET_ENTITY_VISIBLE(veh, true, 0);
				it = vehPropsMap.erase(it);
			}
			else // Both the vehicle and prop exists
			{
				RefreshVehiclePropLink(veh, prop);
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
	for (auto &it : vehPropsMap)
	{
		Vehicle veh = it.first;
		Object prop = it.second;

		if (DOES_ENTITY_EXIST(veh))
		{
			RESET_ENTITY_ALPHA(veh);
		}

		if (DOES_ENTITY_EXIST(prop))
		{
			DELETE_OBJECT(&prop);
		}
	}

	vehPropsMap.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Prop Cars",
		.Id = "vehs_prop_models",
		.IsTimed = true,
		.IncompatibleWith = { "vehs_ghost" }
	}
);