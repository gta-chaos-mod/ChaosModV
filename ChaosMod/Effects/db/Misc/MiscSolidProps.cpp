/*
	Effect by Reguas
*/

#include <stdafx.h>


static void OnTick()
{
	for (Entity prop : GetAllProps())
	{
		SET_DISABLE_BREAKING(prop, true);
	}
}

static void OnStop()
{
	for (Entity prop : GetAllProps())
	{
		SET_DISABLE_BREAKING(prop, false);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_SOLID_PROPS, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Solid Props",
		.Id = "misc_solid_props",
		.IsTimed = true
	}
);
