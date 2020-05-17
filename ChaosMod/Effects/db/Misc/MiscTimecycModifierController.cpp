#include <stdafx.h>

static void OnStop()
{
	CLEAR_TIMECYCLE_MODIFIER();
}

static void OnTickMexico()
{
	SET_TIMECYCLE_MODIFIER("trevorspliff");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect1(EFFECT_SCREEN_MEXICO, nullptr, OnStop, OnTickMexico);

static void OnTickBright()
{
	SET_TIMECYCLE_MODIFIER("mp_x17dlc_int_02");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect2(EFFECT_SCREEN_BRIGHT, nullptr, OnStop, OnTickBright);

static void OnTickFog()
{
	SET_TIMECYCLE_MODIFIER("prologue_ending_fog");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect3(EFFECT_SCREEN_FOG, nullptr, OnStop, OnTickFog);

static void OnTickRenderdist()
{
	SET_TIMECYCLE_MODIFIER("Mp_apart_mid");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect4(EFFECT_SCREEN_RENDERDIST, nullptr, OnStop, OnTickRenderdist);

static void OnTickBloom()
{
	SET_TIMECYCLE_MODIFIER("Bloom");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect5(EFFECT_SCREEN_BLOOM, nullptr, OnStop, OnTickBloom);

static void OnTickLSD()
{
	SET_TIMECYCLE_MODIFIER("ArenaEMP");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect6(EFFECT_SCREEN_LSD, nullptr, OnStop, OnTickLSD);

static void OnTickFullbright()
{
	SET_CLOCK_TIME(0, 0, 0);

	SET_TIMECYCLE_MODIFIER("int_lesters");
	PUSH_TIMECYCLE_MODIFIER();
}

static RegisterEffect registerEffect7(EFFECT_SCREEN_FULLBRIGHT, nullptr, OnStop, OnTickFullbright);