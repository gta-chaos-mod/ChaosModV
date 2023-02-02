/*
    Effect by Last0xygen
*/

#include <stdafx.h>

static const double SPEED            = 0.003;
static const double BOX_HEIGHT_RATIO = 0.45;
static double boxWidth, boxHeight, offsetX, offsetY;
static bool goingDown = true, goingRight = true;

static void OnStart()
{
	// Get Screenresolution to calculate the square ratio
	int screenresx, screenresy;
	_GET_ACTIVE_SCREEN_RESOLUTION(&screenresx, &screenresy);
	boxHeight  = BOX_HEIGHT_RATIO;
	boxWidth   = boxHeight * (double(screenresy) / double(screenresx));
	offsetX    = 0;
	offsetY    = 0;
	goingDown  = true;
	goingRight = true;
}

static void OnTick()
{
	if (goingRight)
	{
		offsetX += SPEED;
		if (offsetX + boxWidth >= 1)
		{
			goingRight = false;
		}
	}
	else
	{
		offsetX -= SPEED;
		if (offsetX <= 0)
		{
			goingRight = true;
		}
	}
	if (goingDown)
	{
		offsetY += SPEED;
		if (offsetY + boxHeight >= 1)
		{
			goingDown = false;
		}
	}
	else
	{
		offsetY -= SPEED;
		if (offsetY <= 0)
		{
			goingDown = true;
		}
	}
	DRAW_RECT(0.5, offsetY / 2, 1, offsetY, 0, 0, 0, 255, false); // Top bar
	double lowerHeight = (1 - offsetY - boxHeight);
	DRAW_RECT(0.5, 1 - (lowerHeight / 2), 1, lowerHeight, 0, 0, 0, 255, false); // Bottom bar
	DRAW_RECT(offsetX / 2, 0.5, offsetX, 1, 0, 0, 0, 255, false);               // Left bar
	double rightWidth = (1 - offsetX - boxWidth);
	DRAW_RECT(1 - (rightWidth / 2), 0.5, rightWidth, 1, 0, 0, 0, 255, false); // Right bar
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "DVD Screensaver",
		.Id = "misc_dvdscreensaver",
		.IsTimed = true,
		.IsShortDuration = true
	}
);