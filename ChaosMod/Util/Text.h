#pragma once

#include "../nativesNoNamespaces.h"

struct ScreenTextVector
{
public:
	ScreenTextVector(float x, float y) : X(x), Y(y)
	{

	}

	const float X;
	const float Y;
};

struct ScreenTextColor
{
public:
	ScreenTextColor(int r, int g, int b)
		: R(static_cast<unsigned char>(r)), G(static_cast<unsigned char>(g)), B(static_cast<unsigned char>(b))
	{

	}

	const unsigned char R;
	const unsigned char G;
	const unsigned char B;
};

enum class ScreenTextAdjust
{
	CENTER,
	LEFT,
	RIGHT
};

inline float GetScreenTextWidth(const std::string& text, float scale)
{
	_BEGIN_TEXT_COMMAND_GET_WIDTH("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
	SET_TEXT_SCALE(scale, scale);
	return _END_TEXT_COMMAND_GET_WIDTH(true);
}

inline void DrawScreenText(const std::string& text, const ScreenTextVector& textPos, float scale, ScreenTextColor textColor,
	bool outline = false, ScreenTextAdjust textAdjust = ScreenTextAdjust::CENTER, const ScreenTextVector& textWrap = { 0.f, 1.f }, bool addBackground = false)
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
		float width = GetScreenTextWidth(text, scale);
		float height = scale / 10;
		float additionalWidth = 0.02;
		DRAW_RECT(textPos.X - (width / 2), textPos.Y + 0.015, width + additionalWidth, height, 0, 0, 0, 127, true);
	}
}