/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	for (Entity prop : GetAllProps())
		SET_DISABLE_BREAKING(prop, true);
}

static void OnStop()
{
	for (Entity prop : GetAllProps())
		SET_DISABLE_BREAKING(prop, false);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Solid Props",
		.Id = "misc_solid_props",
		.IsTimed = true
	}
);
