/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void DrawSpotlightForEntity(Entity entity)
{
	Vector3 min, max;
	GET_MODEL_DIMENSIONS(GET_ENTITY_MODEL(entity), &min, &max);
	Vector3 modelSize = max - min;

	Vector3 modelCenterLocalPos = (max + min) / 2.f;
	Vector3 modelCenterWorldPos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(entity, modelCenterLocalPos.x, modelCenterLocalPos.y, modelCenterLocalPos.z);

	Vector3 lightPos = modelCenterWorldPos;
	lightPos.z += modelSize.z * 1.5f;

	// This is essentially the minimum "radius" we want the spotlight to have
	float modelRadius = sqrtf((modelSize.x * modelSize.x) + (modelSize.y * modelSize.y)) / 2.f;

	// tan(spotAngle) = modelRadius / lightLocalZ;
	// Increase modelRadius slightly to make sure the entire model is lit
	float spotAngle = atan2f(modelRadius + 0.4f, GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(entity, lightPos.x, lightPos.y, lightPos.z).z - min.z);

	// Lazy radians to degree conversion, but accuracy isn't really of concern here
	spotAngle *= 57.295f;
	
	DRAW_SPOT_LIGHT(lightPos.x, lightPos.y, lightPos.z, 0.f, 0.f, -1.f, 255, 255, 255, 10.f, 150.f, 1.f, spotAngle, 0.f);
}

static void OnTick()
{
	SET_CLOCK_TIME(0, 0, 0);
	SET_ARTIFICIAL_LIGHTS_STATE(true);

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			DrawSpotlightForEntity(ped);
		}
	}

	for (Vehicle veh : GetAllVehs())
	{
		DrawSpotlightForEntity(veh);
	}

	// Decrease screen "brightness"
	DRAW_RECT(0.5f, 0.5f, 1.f, 1.f, 0, 0, 0, 140, 0);
}

static void OnStop()
{
	SET_ARTIFICIAL_LIGHTS_STATE(false);
}

static RegisterEffect registerEffect(EFFECT_MISC_SPOTLIGHT, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Stage Lighting",
		.Id = "misc_spotlight",
		.IsTimed = true
	}
);