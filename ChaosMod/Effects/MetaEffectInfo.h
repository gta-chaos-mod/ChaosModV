#pragma once

using BYTE = unsigned char;

struct MetaEffectInfo
{
	float m_fEffectDurationModifier = 1;
	float m_fTimerSpeedModifier = 1;
	BYTE m_ucAdditionalEffectsToDispatch = 0;
	bool m_bShouldHideChaosUI = false;
	bool m_bDisableChaos = false;
	bool m_bFlipChaosUI = false;
};

inline MetaEffectInfo g_MetaInfo;