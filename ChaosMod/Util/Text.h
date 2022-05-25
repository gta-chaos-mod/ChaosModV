#pragma once

#include "Util/Natives.h"

using BYTE = unsigned char;

struct ScreenTextVector
{
	const float m_fX;
	const float m_fY;

	ScreenTextVector(float fX, float fY) : m_fX(fX), m_fY(fY)
	{
	}
};

struct ScreenTextColor
{
	const BYTE m_ucR;
	const BYTE m_ucG;
	const BYTE m_ucB;

	ScreenTextColor(int r, int g, int b)
	    : m_ucR(static_cast<unsigned char>(r)),
	      m_ucG(static_cast<unsigned char>(g)),
	      m_ucB(static_cast<unsigned char>(b))
	{
	}
};

enum class EScreenTextAdjust
{
	Center,
	Left,
	Right
};

inline float GetScreenTextWidth(const std::string &szText, float fScale)
{
	_BEGIN_TEXT_COMMAND_GET_WIDTH("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(szText.c_str());
	SET_TEXT_SCALE(fScale, fScale);

	return _END_TEXT_COMMAND_GET_WIDTH(true);
}

inline void DrawScreenText(const std::string &szText, const ScreenTextVector &textPos, float fScale,
                           ScreenTextColor textColor, bool bOutline = false,
                           EScreenTextAdjust eTextAdjust    = EScreenTextAdjust::Center,
                           const ScreenTextVector &textWrap = { 0.f, 1.f }, bool bAddBackground = false)
{
	BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(szText.c_str());

	SET_TEXT_SCALE(fScale, fScale);
	SET_TEXT_COLOUR(textColor.m_ucR, textColor.m_ucG, textColor.m_ucB, 255);

	if (bOutline)
	{
		SET_TEXT_OUTLINE();
	}

	SET_TEXT_JUSTIFICATION(static_cast<int>(eTextAdjust));
	SET_TEXT_WRAP(textWrap.m_fX, textWrap.m_fY);

	END_TEXT_COMMAND_DISPLAY_TEXT(textPos.m_fX, textPos.m_fY, 0);
	if (bAddBackground)
	{
		float fWidth           = GetScreenTextWidth(szText, fScale);
		float fHeight          = fScale / 10;
		float fAdditionalWidth = 0.02;
		DRAW_RECT(textPos.m_fX - (fWidth * 0.5f), textPos.m_fY + 0.015, fWidth + fAdditionalWidth, fHeight, 0, 0, 0,
		          127, true);
	}
}