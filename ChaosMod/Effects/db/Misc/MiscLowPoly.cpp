#include <stdafx.h>

static void OnStop()
{
	_0xA76359FC80B2438E(1.f);
}

static void OnTick()
{
	_0xA76359FC80B2438E(.04f);
}

static RegisterEffect registerEffect(EFFECT_LOW_POLY, nullptr, OnStop, OnTick);