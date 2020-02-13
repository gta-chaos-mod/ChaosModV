#include <stdafx.h>

static void OnStop()
{
	SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 1.f, false);

	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(1.f);
}

static void OnTick()
{
	SET_PLAYER_MELEE_WEAPON_DAMAGE_MODIFIER(PLAYER_ID(), 9999.f, false);

	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(9999.f);
}

static RegisterEffect registerEffect(EFFECT_PEDS_ONEPUNCH, nullptr, OnStop, OnTick);