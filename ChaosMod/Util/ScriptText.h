#pragma once

#include "Util/Natives.h"

#include <cstdint>

struct ScreenTextVector
{
	const float X;
	const float Y;

	ScreenTextVector(float x, float y) : X(x), Y(y)
	{
	}
};

struct ScreenTextColor
{
	const std::uint8_t R;
	const std::uint8_t G;
	const std::uint8_t B;

	ScreenTextColor(std::uint8_t r, std::uint8_t g, std::uint8_t b) : R(r), G(g), B(b)
	{
	}
};

enum class ScreenTextAdjust
{
	Center,
	Left,
	Right
};

inline float GetScreenTextWidth(const std::string &text, float scale)
{
	_BEGIN_TEXT_COMMAND_GET_WIDTH("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
	SET_TEXT_SCALE(scale, scale);

	return _END_TEXT_COMMAND_GET_WIDTH(true);
}

inline void DrawScreenText(const std::string &text, const ScreenTextVector &textPos, float scale,
                           ScreenTextColor textColor, bool outline = false,
                           ScreenTextAdjust textAdjust      = ScreenTextAdjust::Center,
                           const ScreenTextVector &textWrap = { 0.f, 1.f }, bool addBackground = false)
{
	BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());

	SET_TEXT_SCALE(scale, scale);
	SET_TEXT_COLOUR(textColor.R, textColor.G, textColor.B, 255);

	if (outline)
	{
		SET_TEXT_OUTLINE();
	}

	SET_TEXT_JUSTIFICATION(static_cast<int>(textAdjust));
	SET_TEXT_WRAP(textWrap.X, textWrap.Y);

	END_TEXT_COMMAND_DISPLAY_TEXT(textPos.X, textPos.Y, 0);
	if (addBackground)
	{
		float width           = GetScreenTextWidth(text, scale);
		float height          = scale / 10;
		float additionalWidth = 0.02;
		DRAW_RECT(textPos.X - (width * 0.5f), textPos.Y + 0.015, width + additionalWidth, height, 0, 0, 0, 127, true);
	}
}