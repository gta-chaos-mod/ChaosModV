#include <stdafx.h>

static float screenSize = .0f;
static bool expand = false;

static void OnStart()
{
	expand = false;
	screenSize = .0f;
}

static void OnTick()
{
	DRAW_RECT(.0f, .5f, screenSize, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(1.f, .5f, screenSize, 1.f, 0, 0, 0, 255, false);
	DRAW_RECT(.5f, .0f, 1.f, screenSize, 0, 0, 0, 255, false);
	DRAW_RECT(.5f, 1.f, 1.f, screenSize, 0, 0, 0, 255, false);
	float cycleSeconds = 8.f; //The amount of seconds it takes for a full shrink and expand cycle.//
    static DWORD64 lastTick = 0;
    DWORD64 curTick = GET_GAME_TIMER();

    if (curTick > lastTick + 5)
    {
        lastTick = curTick;
		if (!expand && screenSize >= 1.f)
		{
		    expand = true;
		}
		else if (expand && screenSize <= 0.f)
		{
			expand = false;
		}
		if (!expand)
		{
			screenSize += 0.05f / cycleSeconds;
		}
		else
		{
			screenSize -= 0.05f / cycleSeconds;
		}
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_SHRINKCAM, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Shrinking Screen",
		.Id = "misc_shrinkcam",
		.IsTimed = true,
		.IsShortDuration = true
	}
);