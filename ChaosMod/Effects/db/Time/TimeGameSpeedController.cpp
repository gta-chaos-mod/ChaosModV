#include <stdafx.h>

static void OnStop()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", false);

	SET_TIME_SCALE(1.f);
}

static void OnTickX02()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_TIME_SCALE(.2f);
}

static RegisterEffect registerEffect1(EFFECT_GAMESPEED_X02, nullptr, OnStop, OnTickX02);

static void OnTickX05()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_TIME_SCALE(.5f);
}

static RegisterEffect registerEffect2(EFFECT_GAMESPEED_X05, nullptr, OnStop, OnTickX05);

static void OnTickLag()
{
	static int state = 0;

	static DWORD64 lastTick = 0;
	auto curTick = GetTickCount64();

	if (curTick > lastTick + 250)
	{
		lastTick = curTick;
		if (++state == 6)
		{
			state = 0;
		}
	}

	SET_TIME_SCALE(state < 5 ? 1.f : 0.f);

	if (state == 5)
	{
		DISABLE_ALL_CONTROL_ACTIONS(1);
	}
}

static RegisterEffect registerEffect3(EFFECT_GAMESPEED_LAG, nullptr, OnStop, OnTickLag);