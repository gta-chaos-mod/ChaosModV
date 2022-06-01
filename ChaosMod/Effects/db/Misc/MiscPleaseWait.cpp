/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

// clang-format off
std::vector<std::string> msgs = 
{ 
	"Gnidaol",
	"Loading",
	"Please wait...",
	"Time Remaining: 6 years",
	"Doing Something",
	"Be Patient Bro",
	"How are you still here?",
	"Everyday I'm Buffering",
	"Loading Online",
	"Redownloading Game",
	"Just f**king around",
	"Killing you, 1 second at a time"
};

static void OnStart()
{
	const char* msg = msgs.at(g_Random.GetRandomInt(0, msgs.size()-1)).c_str();
	BEGIN_TEXT_COMMAND_BUSYSPINNER_ON("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
	END_TEXT_COMMAND_BUSYSPINNER_ON(g_Random.GetRandomInt(0, 4));

	Hooks::EnableScriptThreadBlock();
}

static void OnStop()
{
	BUSYSPINNER_OFF();

	Hooks::DisableScriptThreadBlock();
}

REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Please Wait",
		.Id = "misc_pleasewait",
		.IsTimed = true,
		.IsShortDuration = true
	}
);