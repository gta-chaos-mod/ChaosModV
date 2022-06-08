#include <stdafx.h>

static void OnStart()
{
	SET_RADIO_TO_STATION_INDEX(g_Random.GetRandomInt(0, 18));
}

static void OnStop()
{
	ENABLE_CONTROL_ACTION(2, 85, true);

	SET_MOBILE_PHONE_RADIO_STATE(false);
	SET_AUDIO_FLAG("MobileRadioInGame", false);
	SET_AUDIO_FLAG("AllowRadioDuringSwitch", false);
}

static void OnTick()
{
	DISABLE_CONTROL_ACTION(2, 85, true);

	SET_MOBILE_PHONE_RADIO_STATE(true);
	SET_AUDIO_FLAG("MobileRadioInGame", true);
	SET_AUDIO_FLAG("AllowRadioDuringSwitch", true);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Satellite Radio",
		.Id = "mobile_radio",
		.IsTimed = true
	}
);