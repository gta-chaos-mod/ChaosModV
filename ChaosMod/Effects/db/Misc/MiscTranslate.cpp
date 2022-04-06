#include <stdafx.h>

#include "Memory/Hooks/LabelHook.h"

static void OnStart()
{
	Hooks::EnableLabelHook("chaosmod/data/misc_translated/labels.txt");
}

static void OnStop()
{
	Hooks::DisableLabelHook();
}

static RegisterEffect registerEffect(EFFECT_MISC_TRANSLATE, OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Google Translated",
		.Id = "misc_translated",
		.IsTimed = true
	}
);