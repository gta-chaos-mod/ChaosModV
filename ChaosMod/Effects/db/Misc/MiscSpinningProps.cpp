/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	for (Entity prop : GetAllProps())
	{
		Vector3 rotation = GET_ENTITY_ROTATION(prop, 2);
		SET_ENTITY_ROTATION(prop, rotation.x, rotation.y, rotation.z + 5.f, 2, true);
	}
}

static RegisterEffect registerEffect(EFFECT_SPINNING_PROPS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Spinning Props",
		.Id = "misc_spinning_props",
		.IsTimed = true
	}
);
