#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR const std::array options = {
	"Nothing",
	"All Peds Are Peds",
	"Teleport To Current Location",
	"Expanded & Enhanced",
	"Spawn Air",
	"Destroy All Destroyed Vehicles",
	"Kill All Dead Peds",
	"+0 Wanted Stars",
	"Jesus Watches Over You",
	"Set Time To Current Time",
	"Set Player Into Current Vehicle",
	"All Cops Are Cops",
	"Aim to Point Gun",
	"Give Everyone A Nose",
	"Teleport Player A Few Millimeters",
	"Flying Birds",
	"Teleport All Elephants To Player",
	"PC Experience",
	"All Enemies Attack Player",
	"No Crash",
	" ",
	"Delay GTA 6",
	"Run Game",
	"Make Player Ugly",
	"Make Player Beautiful",
	"Teleport To Parallel Universe",
};

static void OnStart()
{
	const auto &effectOverride = options[g_Random.GetRandomInt(0, options.size() - 1)];

	CurrentEffect::OverrideEffectName(effectOverride);

	while (CurrentEffect::GetEffectCompletionPercentage() < 0.5f)
		WAIT(0);
	;
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Nothing",
		.Id = "nothing",
		.HideRealNameOnStart = true
	}
);