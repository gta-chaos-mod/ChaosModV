/*
	Effect by Last0xygen
*/

#include <stdafx.h>

enum TiredMode {
	closingEyes,
	openingEyes,
	waiting,
};
static TiredMode currentMode = TiredMode::closingEyes;
static float alpha;
static int closingIterator;
static int nextTimestamp;

static void BlackOut(float alpha)
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, alpha, false);
}

static void OnStop()
{
	BlackOut(0);
}

static void OnStart()
{
	currentMode = TiredMode::closingEyes;
	alpha = 0;
	closingIterator = 20;
}

static void OnTick()
{
	switch (currentMode)
	{
	case closingEyes:
		alpha += closingIterator;
		if (alpha >= 255)
		{
			currentMode = TiredMode::openingEyes;
			nextTimestamp = GET_GAME_TIMER() + ((20 - closingIterator) * 20);
			if (closingIterator > 1)
			{
				closingIterator = max(1, closingIterator - 2);
			}
		}
		break;
	case TiredMode::openingEyes:
		if (GET_GAME_TIMER() > nextTimestamp)
		{
			alpha -= 30;
			if (alpha <= 0)
			{
				alpha = 0;
				currentMode = TiredMode::waiting;
				nextTimestamp = GET_GAME_TIMER() + g_random.GetRandomInt(250, 3000);
			}
		}
		break;
	case TiredMode::waiting:
		if (GET_GAME_TIMER() > nextTimestamp)
		{
			currentMode = TiredMode::closingEyes;
		}
		break;
	}
	BlackOut(alpha);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_TIRED, OnStart, OnStop, OnTick);
