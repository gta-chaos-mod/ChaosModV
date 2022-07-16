#include <stdafx.h>

static void OnStop()
{
	REQUEST_SCRIPT("cellphone_controller");

	while (!HAS_SCRIPT_LOADED("cellphone_controller"))
	{
		WAIT(0);
	}

	START_NEW_SCRIPT("cellphone_controller", 1424);

	SET_SCRIPT_AS_NO_LONGER_NEEDED("cellphone_controller");
}

static void OnTick()
{
	TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("cellphone_controller");
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Phone",
		.Id = "player_nophone",
		.IsTimed = true
	}
);