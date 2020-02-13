#include <stdafx.h>

static void OnStop()
{
	Memory::SetSnow(false);
}

static void OnTick()
{
	Memory::SetSnow(true);
}

static RegisterEffect registerEffect(EFFECT_SNOW, nullptr, OnStop, OnTick);