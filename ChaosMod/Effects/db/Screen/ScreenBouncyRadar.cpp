#include <stdafx.h>

#include "Memory/UI.h"

static float xOffset, yOffset, xVelocity, yVelocity, xMin, xMax, yMin, yMax;

static void OnStart()
{
	int safezoneSize = GET_PROFILE_SETTING(212);
	int screenWidth, screenHeight;
	_GET_ACTIVE_SCREEN_RESOLUTION(&screenWidth, &screenHeight);

	float multiplier = ((float)screenWidth / (float)screenHeight) * (9.f / 16);

	if (multiplier > 1)
	{
		xMin = -0.005f * safezoneSize * multiplier - (multiplier - 1) / 2;
	}
	else
	{
		xMin = -0.005f * safezoneSize * multiplier;
	}
	xMax      = multiplier - 0.14f + xMin;
	yMax      = 0.005f * safezoneSize;
	yMin      = yMax - 0.825f;

	xOffset   = 0.f;
	yOffset   = 0.f;

	xVelocity = g_Random.GetRandomFloat(0.0025f, 0.01f) * (g_Random.GetRandomInt(0, 1) ? -1 : 1);
	yVelocity = g_Random.GetRandomFloat(0.0025f, 0.01f) * (g_Random.GetRandomInt(0, 1) ? -1 : 1);
}

static void OnTick()
{
	if (xOffset < xMin || xOffset > xMax)
	{
		xVelocity *= -1;
	}

	if (yOffset < yMin || yOffset > yMax)
	{
		yVelocity *= -1;
	}

	xOffset += xVelocity;
	yOffset += yVelocity;

	Memory::SetRadarOffset(xOffset, yOffset);
}

static void OnStop()
{
	Memory::ResetRadar();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Bouncy Radar",
		.Id = "screen_bouncyradar",
		.IsTimed = true,
		.IncompatibleWith = { "no_hud", "screen_maximap", "no_radar" }
	}
);