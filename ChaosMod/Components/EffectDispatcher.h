#pragma once

#include "Component.h"

#include "../Effects/EffectThreads.h"
#include "../Effects/EffectData.h"
#include "../Effects/EffectIdentifier.h"
#include "../Effects/EnabledEffectsMap.h"
#include "../Util/Random.h"

#include <vector>
#include <array>
#include <memory>

#define _NODISCARD [[nodiscard]]

using DWORD64 = unsigned long long;
using WORD = unsigned short;
using BYTE = unsigned char;

enum class ETwitchOverlayMode;

class EffectDispatcher : public Component
{
private:
	struct ActiveEffect
	{
		EffectIdentifier m_EffectIdentifier;
		RegisteredEffect* m_pRegisteredEffect;

		DWORD64 m_ullThreadId = 0;

		std::string m_szName;
		std::string m_szFakeName;

		float m_fTimer = 0.f;
		float m_fMaxTime = 0.f;

		bool m_bHideText = true;

		bool m_bIsFake = false;
		// The "real" timer to use for fake effects (remove effect after this timer runs out, but still display the normal timer)
		float m_fHiddenTimer = 0.f;
		float m_fHiddenMaxTime = 0.f;

		ActiveEffect(const EffectIdentifier& effectIdentifier, RegisteredEffect* pRegisteredEffect, const std::string& szName, const std::string& szFakeName, float fTimer, bool isFake = false)
		{
			m_EffectIdentifier = effectIdentifier;
			m_pRegisteredEffect = pRegisteredEffect;
			m_szName = szName;
			m_szFakeName = szFakeName;
			m_bIsFake = isFake;

			m_fTimer = fTimer;
			m_fMaxTime = fTimer;

			if (isFake)
			{
				float fHiddenTimer = g_Random.GetRandomFloat(3.f, 6.f);
				m_fHiddenTimer = fHiddenTimer;
				m_fHiddenMaxTime = fHiddenTimer;
			}
			else
			{
				EEffectTimedType eTimedType = g_dictEnabledEffects.at(effectIdentifier).TimedType;

				m_ullThreadId = EffectThreads::CreateThread(pRegisteredEffect, eTimedType != EEffectTimedType::Unk
					&& eTimedType != EEffectTimedType::NotTimed);
			}
		}
	};

	std::array<BYTE, 3> m_rgTimerColor;
	std::array<BYTE, 3> m_rgTextColor;
	std::array<BYTE, 3> m_rgEffectTimerColor;

	bool m_bDisableDrawTimerBar = false;
	bool m_bDisableDrawEffectTexts = false;

	WORD m_usEffectSpawnTime = 0;
	WORD m_usEffectTimedDur = 0;
	WORD m_usEffectTimedShortDur = 0;

	WORD m_usMetaEffectSpawnTime = 0;
	WORD m_usMetaEffectTimedDur = 0;
	WORD m_usMetaEffectShortDur = 0;

	int m_iMaxRunningEffects = 0;

	float m_fPercentage = 0.f;
	float m_fEffectsInnerSpacingMax = .075f;
	float m_fEffectsInnerSpacingMin = .030f;
	float m_fEffectsTopSpacingDefault = .2f;
	float m_fEffectsTopSpacingWithVoting = .35f;

	std::vector<ActiveEffect> m_rgActiveEffects;
	std::vector<RegisteredEffect*> m_rgPermanentEffects;
	std::list<RegisteredEffect*> m_rgDispatchedEffectsLog;

	bool m_bEnableNormalEffectDispatch = true;

	DWORD64 m_ullTimerTimer = 0;
	WORD m_usTimerTimerRuns = 0;
	DWORD64 m_ullEffectsTimer = 0;

	bool m_bMetaEffectsEnabled = true;
	DWORD64 m_ullMetaTimer = 0;
	int m_iMetaEffectTimer = 0;

	bool m_bEnableTwitchVoting;
	ETwitchOverlayMode m_eTwitchOverlayMode;

public:
	bool m_bPauseTimer = false;

	bool m_bDispatchEffectsOnTimer = true;

	float m_fFakeTimerBarPercentage = 0.f;

protected:
	EffectDispatcher(const std::array<BYTE, 3>& rgTimerColor, const std::array<BYTE, 3>& rgTextColor, const std::array<BYTE, 3>& rgEffectTimerColor);
	virtual ~EffectDispatcher() override;
	
private:
	void UpdateTimer();
	void UpdateEffects();
	void UpdateMetaEffects();
	float GetEffectTopSpace();
	bool ShouldRemoveEffectForTimeOut(int timer, int effectCount, int minAmountAdvancedCleaning);

public:
	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	void DrawTimerBar();
	void DrawEffectTexts();

	bool _NODISCARD ShouldDispatchEffectNow() const;

	int _NODISCARD GetRemainingTimerTime() const;
	int _NODISCARD GetMaxTimerTime() const;

	void DispatchEffect(const EffectIdentifier& effectIdentifier, const char* szSuffix = nullptr, bool isFake = false, bool bAddToLog = true);
	void DispatchRandomEffect(const char* szSuffix = nullptr, bool isFake = false);

	void ClearEffects(bool bIncludePermanent = true);
	void ClearActiveEffects(const EffectIdentifier& exclude = EffectIdentifier());
	void ClearMostRecentEffect();
	void ClearAllFakeEffects();

	std::vector<RegisteredEffect*> GetRecentEffects(int distance, EEffectType ignore = EFFECT_INVALID) const;

	void Reset();
	void ResetTimer();

	void OverrideEffectName(EEffectType eEffectType, const std::string& szOverrideName);
	void OverrideEffectName(EEffectType eEffectType, EEffectType eFakeEffectType);

	template <class T> requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};
