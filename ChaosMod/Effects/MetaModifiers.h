#pragma once

using BYTE = unsigned char;

namespace MetaModifiers
{
	inline float m_fEffectDurationModifier = 1;
	inline float m_fTimerSpeedModifier = 1;
	inline BYTE m_ucAdditionalEffectsToDispatch = 0;
	inline bool m_bHideChaosUI = false;
	inline bool m_bDisableChaos = false;
	inline bool m_bFlipChaosUI = false;

	inline bool m_bDisplayFakeEffectBars = false;
	inline BYTE m_ucNumFakeEffectBars = 0;
	inline BYTE m_ucRealEffectBarIndex = 0;
	inline struct FakeEffectBarInfo
	{
		WORD m_usTimerTimerRuns = 0;
		float m_fPercentage = 0.f;
	} *m_pFakeEffectBars = nullptr;
};