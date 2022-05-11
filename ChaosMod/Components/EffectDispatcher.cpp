#include <stdafx.h>

#include "EffectDispatcher.h"

EffectDispatcher::EffectDispatcher(const std::array<BYTE, 3>& rgTimerColor, const std::array<BYTE, 3>& rgTextColor, const std::array<BYTE, 3>& rgEffectTimerColor)
	: Component()
{
	m_rgTimerColor = rgTimerColor;
	m_rgTextColor = rgTextColor;
	m_rgEffectTimerColor = rgEffectTimerColor;

	m_bDisableDrawTimerBar = g_OptionsManager.GetConfigValue<bool>("DisableTimerBarDraw", OPTION_DEFAULT_NO_EFFECT_BAR);
	m_bDisableDrawEffectTexts = g_OptionsManager.GetConfigValue<bool>("DisableEffectTextDraw", OPTION_DEFAULT_NO_TEXT_DRAW);

	m_usEffectSpawnTime = g_OptionsManager.GetConfigValue<int>("NewEffectSpawnTime", OPTION_DEFAULT_EFFECT_SPAWN_TIME);
	m_usEffectTimedDur = g_OptionsManager.GetConfigValue<int>("EffectTimedDur", OPTION_DEFAULT_EFFECT_TIMED_DUR);
	m_usEffectTimedShortDur = g_OptionsManager.GetConfigValue<int>("EffectTimedShortDur", OPTION_DEFAULT_EFFECT_SHORT_TIMED_DUR);

	m_usMetaEffectSpawnTime = g_OptionsManager.GetConfigValue<int>("NewMetaEffectSpawnTime", OPTION_DEFAULT_EFFECT_META_SPAWN_TIME);
	m_usMetaEffectTimedDur = g_OptionsManager.GetConfigValue<int>("MetaEffectDur", OPTION_DEFAULT_EFFECT_META_TIMED_DUR);
	m_usMetaEffectShortDur = g_OptionsManager.GetConfigValue<int>("MetaShortEffectDur", OPTION_DEFAULT_EFFECT_META_SHORT_TIMED_DUR);

	m_iMaxRunningEffects = g_OptionsManager.GetConfigValue<int>("MaxParallelRunningEffects", OPTION_DEFAULT_MAX_RUNNING_PARALLEL_EFFECTS);

	m_iMetaEffectTimer = m_usMetaEffectSpawnTime;

	m_bEnableTwitchVoting = g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	m_eTwitchOverlayMode = static_cast<ETwitchOverlayMode>(g_OptionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	OnModPauseCleanup();
}

void EffectDispatcher::OnModPauseCleanup()
{
	ClearEffects();
}

void EffectDispatcher::OnRun()
{
	UpdateEffects();

	if (!m_bPauseTimer)
	{
		if (!MetaModifiers::m_bDisableChaos)
		{
			UpdateTimer();
		}

		UpdateMetaEffects();
	}

	DrawTimerBar();
	DrawEffectTexts();
}

void EffectDispatcher::UpdateTimer()
{
	if (!m_bEnableNormalEffectDispatch)
	{
		return;
	}

	DWORD64 ullCurrentUpdateTime = GetTickCount64();

	float fDelta = ullCurrentUpdateTime - m_ullTimerTimer;
	if (fDelta > 1000.f)
	{
		m_usTimerTimerRuns++;

		m_ullTimerTimer = ullCurrentUpdateTime;

		if (MetaModifiers::m_bDisplayFakeEffectBars)
		{
			for (size_t i = 0; i < MetaModifiers::m_ucNumFakeEffectBars; i++)
			{
				MetaModifiers::m_pFakeEffectBars[i].m_usTimerTimerRuns++;
			}
		}

		fDelta = 0;
	}

	if ((m_fPercentage = (fDelta + (m_usTimerTimerRuns * 1000)) / (m_usEffectSpawnTime / MetaModifiers::m_fTimerSpeedModifier * 1000)) > 1.f
		&& m_bDispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		if (MetaModifiers::m_ucAdditionalEffectsToDispatch > 0)
		{
			for (BYTE ucIdx = 0; ucIdx < MetaModifiers::m_ucAdditionalEffectsToDispatch; ucIdx++)
			{
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			}
		}

		m_usTimerTimerRuns = 0;
	}

	if (MetaModifiers::m_bDisplayFakeEffectBars)
	{
		for (size_t i = 0; i < MetaModifiers::m_ucNumFakeEffectBars; i++)
		{
			if ((MetaModifiers::m_pFakeEffectBars[i].m_fPercentage = (fDelta + (MetaModifiers::m_pFakeEffectBars[i].m_usTimerTimerRuns * 1000)) / (m_usEffectSpawnTime / MetaModifiers::m_fTimerSpeedModifier * 1000)) > 1.f
				&& m_bDispatchEffectsOnTimer)
			{
				DispatchRandomEffect(nullptr, true);

				if (MetaModifiers::m_ucAdditionalEffectsToDispatch > 0)
				{
					for (BYTE ucIdx = 0; ucIdx < MetaModifiers::m_ucAdditionalEffectsToDispatch; ucIdx++)
					{
						GetComponent<EffectDispatcher>()->DispatchRandomEffect(nullptr, true);
					}
				}

				MetaModifiers::m_pFakeEffectBars[i].m_usTimerTimerRuns = 0;
			}
		}
	}
}

void EffectDispatcher::UpdateEffects()
{
	EffectThreads::RunThreads();

	// Don't continue if there are no enabled effects
	if (g_dictEnabledEffects.empty())
	{
		return;
	}

	for (ActiveEffect& effect : m_rgActiveEffects)
	{
		if (effect.m_bHideText
			&& (effect.m_bIsFake || EffectThreads::HasThreadOnStartExecuted(effect.m_ullThreadId)))
		{
			effect.m_bHideText = false;
		}
	}

	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - m_ullEffectsTimer) > 1000)
	{
		m_ullEffectsTimer = currentUpdateTime;

		int activeEffectsSize = m_rgActiveEffects.size();
		int maxEffects = (int)(floor((1.0f - GetEffectTopSpace()) / m_fEffectsInnerSpacingMin) - 1);
		maxEffects = min(maxEffects, m_iMaxRunningEffects);
		int effectCountToCheckCleaning = 3;
		std::vector<ActiveEffect>::iterator it;
		for (it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end(); )
		{
			ActiveEffect& effect = *it;
			EffectData& effectData = g_dictEnabledEffects.at(effect.m_EffectIdentifier);
			if (effectData.IsMeta())
			{
				effect.m_fTimer--;

				if (effect.m_bIsFake)
				{
					effect.m_fHiddenTimer--;
				}
			}
			else
			{
				float diff = 1 / MetaModifiers::m_fEffectDurationModifier;

				effect.m_fTimer -= diff;

				if (effect.m_bIsFake)
				{
					effect.m_fHiddenTimer -= diff;
				}
			}
			bool shouldStopEffect = false;
			if ((effect.m_bIsFake ? effect.m_fHiddenMaxTime > 0 && effect.m_fHiddenTimer <= 0 : effect.m_fMaxTime > 0 && effect.m_fTimer <= 0))
			{
				shouldStopEffect = true;
			} 
			else if (!effectData.IsMeta())
			{
				if (activeEffectsSize > maxEffects || (effect.m_fMaxTime < 0 && ShouldRemoveEffectForTimeOut(effect.m_fTimer, activeEffectsSize, effectCountToCheckCleaning)))
				{
					shouldStopEffect = true;
				}
			}
			if (shouldStopEffect)
			{
				EffectThreads::StopThread(effect.m_ullThreadId);
				activeEffectsSize--;
				it = m_rgActiveEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

void EffectDispatcher::UpdateMetaEffects()
{
	if (!m_bMetaEffectsEnabled)
	{
		return;
	}

	DWORD64 currentUpdateTime = GetTickCount64();
	if (currentUpdateTime - m_ullMetaTimer < 1000)
	{
		return;
	}

	m_ullMetaTimer = currentUpdateTime;

	if (--m_iMetaEffectTimer <= 0)
	{
		m_iMetaEffectTimer = m_usMetaEffectSpawnTime;

		std::vector<std::tuple<EffectIdentifier, EffectData*>> availableMetaEffects;

		float totalWeight = 0.f;
		for (auto& [ effectId, effectData ] : g_dictEnabledEffects)
		{
			if (effectData.IsMeta() && effectData.TimedType != EEffectTimedType::Permanent && !effectData.IsUtility())
			{
				totalWeight += GetEffectWeight(effectData);

				availableMetaEffects.push_back(std::make_tuple(effectId, &effectData));
			}
		}

		if (!availableMetaEffects.empty())
		{
			// TODO: Stop duplicating effect weight logic everywhere
			float chosen = g_Random.GetRandomFloat(0.f, totalWeight);

			totalWeight = 0.f;

			const EffectIdentifier* targetEffectIdentifier = nullptr;
			for (const auto& [effectIdentifier, effectData] : availableMetaEffects)
			{
				totalWeight += GetEffectWeight(*effectData);

				effectData->Weight += effectData->WeightMult;

				if (!targetEffectIdentifier && chosen <= totalWeight)
				{
					targetEffectIdentifier = &effectIdentifier;
				}
			}

			if (targetEffectIdentifier)
			{
				DispatchEffect(*targetEffectIdentifier, "(Meta)");
			}
		}
		else
		{
			m_bMetaEffectsEnabled = false;
			m_iMetaEffectTimer = INT_MAX;
		}
	}
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_bEnableNormalEffectDispatch
		|| m_bDisableDrawTimerBar
		|| MetaModifiers::m_bHideChaosUI
		|| MetaModifiers::m_bDisableChaos)
	{
		return;
	}

	float fPercentage = m_fFakeTimerBarPercentage > 0.f
		&& m_fFakeTimerBarPercentage <= 1.f
			? m_fFakeTimerBarPercentage
			: m_fPercentage;

#define DRAW_TIMER(fillAmount, height, yPos) \
	if (MetaModifiers::m_bFlipChaosUI) \
	{ \
		DRAW_RECT(1.f - fillAmount * .5f, yPos, fillAmount, height, m_rgTimerColor[0], m_rgTimerColor[1], m_rgTimerColor[2], 255, false); \
	} \
	else \
	{ \
		DRAW_RECT(fillAmount * .5f, yPos, fillAmount, height, m_rgTimerColor[0], m_rgTimerColor[1], m_rgTimerColor[2], 255, false); \
	}

	// New Effect Bar
	if (MetaModifiers::m_bDisplayFakeEffectBars)
	{
		static constexpr float BAR_HEIGHT = .0125f;
		static constexpr float BACKGROUND_BAR_HEIGHT = BAR_HEIGHT * (.021f / .018f);

		DRAW_RECT(.5f, (BACKGROUND_BAR_HEIGHT / 2.f) * (MetaModifiers::m_ucNumFakeEffectBars + 1), 1.f, BACKGROUND_BAR_HEIGHT * (MetaModifiers::m_ucNumFakeEffectBars + 1), 0, 0, 0, 127, false);

		for (size_t i = 0; i < MetaModifiers::m_ucNumFakeEffectBars + 1; i++)
		{
			float fPercentageForBar;
			if (i == MetaModifiers::m_ucRealEffectBarIndex)
			{
				fPercentageForBar = fPercentage;
			}
			else
			{
				fPercentageForBar = MetaModifiers::m_pFakeEffectBars[(i > MetaModifiers::m_ucRealEffectBarIndex ? i - 1 : i)].m_fPercentage;
			}

			DRAW_TIMER(fPercentageForBar, BAR_HEIGHT, BACKGROUND_BAR_HEIGHT * (i + .5f));
		}
	}
	else
	{
		DRAW_RECT(.5f, .01f, 1.f, .021f, 0, 0, 0, 127, false);
		DRAW_TIMER(fPercentage, .018f, .01f);
	}

#undef DRAW_TIMER
}

void EffectDispatcher::DrawEffectTexts()
{
	if (m_bDisableDrawEffectTexts)
	{
		return;
	}

	float fPosY = GetEffectTopSpace();
	float effectSpacing = m_fEffectsInnerSpacingMax;
	while (round(effectSpacing * 1000) > round(m_fEffectsInnerSpacingMin * 1000) && 1.0 - fPosY < m_rgActiveEffects.size() * effectSpacing)
	{
		effectSpacing -= 0.005f;
	}

	for (const ActiveEffect& effect : m_rgActiveEffects)
	{
		const bool bHasFake = !effect.m_szFakeName.empty();
		auto &effectData = g_dictEnabledEffects.at(effect.m_EffectIdentifier);

		if ((effect.m_bHideText && !bHasFake)
			|| (MetaModifiers::m_bHideChaosUI && !effectData.IsMeta() && !effectData.IsUtility())
			|| (MetaModifiers::m_bDisableChaos && !effectData.IsMeta() && !effectData.IsUtility()))
		{
			continue;
		}

		std::string name = effect.m_szFakeName;

		if (!effect.m_bHideText || !bHasFake)
		{
			name = effect.m_szName;
		}

		if (MetaModifiers::m_bFlipChaosUI)
		{
			DrawScreenText(name, { .085f, fPosY }, .47f, { m_rgTextColor[0], m_rgTextColor[1], m_rgTextColor[2] }, true,
				EScreenTextAdjust::Left, { .0f, .915f });
		}
		else
		{
			DrawScreenText(name, { .915f, fPosY }, .47f, { m_rgTextColor[0], m_rgTextColor[1], m_rgTextColor[2] }, true,
				EScreenTextAdjust::Right, { .0f, .915f });
		}

		if (effect.m_fTimer > 0)
		{
			if (MetaModifiers::m_bFlipChaosUI)
			{
				DRAW_RECT(.04f, fPosY + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.04f, fPosY + .0185f, .048f * (1.f - (effect.m_fTimer / effect.m_fMaxTime)), .017f, m_rgEffectTimerColor[0], m_rgEffectTimerColor[1],
					m_rgEffectTimerColor[2], 255, false);
			}
			else
			{
				DRAW_RECT(.96f, fPosY + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.96f, fPosY + .0185f, .048f * effect.m_fTimer / effect.m_fMaxTime, .017f, m_rgEffectTimerColor[0], m_rgEffectTimerColor[1],
					m_rgEffectTimerColor[2], 255, false);
			}
		}

		fPosY += effectSpacing;
	}
}

bool _NODISCARD EffectDispatcher::ShouldDispatchEffectNow() const
{
	return GetRemainingTimerTime() <= 0;
}

int _NODISCARD EffectDispatcher::GetRemainingTimerTime() const
{
	return GetMaxTimerTime() - m_usTimerTimerRuns;
}

int _NODISCARD EffectDispatcher::GetMaxTimerTime() const
{
	return m_usEffectSpawnTime / MetaModifiers::m_fTimerSpeedModifier;
}

void EffectDispatcher::DispatchEffect(const EffectIdentifier& effectIdentifier, const char* szSuffix, bool isFake)
{
	EffectData& effectData = g_dictEnabledEffects.at(effectIdentifier);
	if (effectData.TimedType == EEffectTimedType::Permanent)
	{
		return;
	}

	// Increase weight for all effects first
	for (auto& [ effectId, effectData ] : g_dictEnabledEffects)
	{
		if (!effectData.IsMeta())
		{
			effectData.Weight += effectData.WeightMult;
		}
	}

	// Reset weight of this effect (or every effect in group) to reduce chance of same effect (group) happening multiple times in a row
	if (effectData.GroupType.empty())
	{
		effectData.Weight = effectData.WeightMult;
	}
	else
	{
		for (auto& [ effectId, effectData ] : g_dictEnabledEffects)
		{
			if (effectData.GroupType == effectData.GroupType)
			{
				effectData.Weight = effectData.WeightMult;
			}
		}
	}

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool bAlreadyExists = false;
	if (!isFake)
	{
		LOG("Dispatched effect \"" << effectData.Name << "\"");

		const auto& rgIncompatibleIds = effectData.IncompatibleIds;

		for (auto it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end(); )
		{
			ActiveEffect& activeEffect = *it;
			if (activeEffect.m_bIsFake)
			{
				it++;
				continue;
			}

			if (activeEffect.m_EffectIdentifier == effectIdentifier
				&& effectData.TimedType != EEffectTimedType::Unk
				&& effectData.TimedType != EEffectTimedType::NotTimed)
			{
				bAlreadyExists = true;
				activeEffect.m_fTimer = activeEffect.m_fMaxTime;

				break;
			}

			bool bFound = false;
			if (std::find(rgIncompatibleIds.begin(), rgIncompatibleIds.end(), g_dictEnabledEffects.at(activeEffect.m_EffectIdentifier).Id)
				!= rgIncompatibleIds.end())
			{
				bFound = true;
			}

			// Check if current effect is marked as incompatible in active effect
			if (!bFound)
			{
				const auto& rgActiveIncompatibleIds = g_dictEnabledEffects.at(activeEffect.m_EffectIdentifier).IncompatibleIds;

				if (std::find(rgActiveIncompatibleIds.begin(), rgActiveIncompatibleIds.end(), effectData.Id) != rgActiveIncompatibleIds.end())
				{
					bFound = true;
				}
			}

			if (bFound)
			{
				EffectThreads::StopThread(activeEffect.m_ullThreadId);

				it = m_rgActiveEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	if (!bAlreadyExists || isFake)
	{
		RegisteredEffect* registeredEffect = GetRegisteredEffect(effectIdentifier);

		if (registeredEffect)
		{
			std::ostringstream ossEffectName;
			ossEffectName << (effectData.HasCustomName() ? effectData.CustomName : effectData.Name);

			if (szSuffix && strlen(szSuffix) > 0)
			{
				ossEffectName << " " << szSuffix;
			}

			ossEffectName << std::endl;

			if (!MetaModifiers::m_bHideChaosUI)
			{
				// Play global sound (if one exists)
				// Workaround: Force no global sound for "Fake Crash" and "Fake Death"
				if (effectIdentifier.GetEffectType() != EFFECT_MISC_CRASH
					&& effectIdentifier.GetEffectType() != EFFECT_PLAYER_FAKEDEATH)
				{
					Mp3Manager::PlayChaosSoundFile("global_effectdispatch");
				}

				if (!isFake)
				{
					// Play a sound if corresponding .mp3 file exists
					Mp3Manager::PlayChaosSoundFile(effectData.Id);
				}
			}

			int effectTime = -1;
			switch (effectData.TimedType)
			{
			case EEffectTimedType::Normal:
				effectTime = effectData.IsMeta()
					? m_usMetaEffectTimedDur
					: m_usEffectTimedDur;
				break;
			case EEffectTimedType::Short:
				effectTime = effectData.IsMeta()
					? m_usMetaEffectShortDur
					: m_usEffectTimedShortDur;
				break;
			case EEffectTimedType::Custom:
				effectTime = effectData.CustomTime;
				break;
			}

			m_rgActiveEffects.emplace_back(effectIdentifier, registeredEffect, ossEffectName.str(), effectData.FakeName, effectTime, isFake);
		}
	}

	if (!isFake)
	{
		m_fPercentage = .0f;
	}
}

void EffectDispatcher::DispatchRandomEffect(const char* szSuffix, bool isFake)
{
	if (!m_bEnableNormalEffectDispatch)
	{
		return;
	}

	std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
	for (const auto& pair : g_dictEnabledEffects)
	{
		const auto& [effectIdentifier, effectData] = pair;

		if (effectData.TimedType != EEffectTimedType::Permanent && !effectData.IsMeta() && !effectData.IsUtility())
		{
			choosableEffects.emplace(effectIdentifier, effectData);
		}
	}

	float fTotalWeight = 0.f;
	for (const auto& pair : choosableEffects)
	{
		const EffectData& effectData = pair.second;

		fTotalWeight += GetEffectWeight(effectData);
	}

	float fChosen = g_Random.GetRandomFloat(0.f, fTotalWeight);

	fTotalWeight = 0.f;

	const EffectIdentifier* pTargetEffectIdentifier = nullptr;
	for (const auto& pair : choosableEffects)
	{
		const auto& [effectIdentifier, effectData] = pair;

		fTotalWeight += GetEffectWeight(effectData);

		if (fChosen <= fTotalWeight)
		{
			pTargetEffectIdentifier = &effectIdentifier;

			break;
		}
	}

	if (pTargetEffectIdentifier)
	{
		DispatchEffect(*pTargetEffectIdentifier, szSuffix, isFake);
	}
}

void EffectDispatcher::ClearEffects(bool bIncludePermanent)
{
	EffectThreads::StopThreads();

	if (bIncludePermanent)
	{
		m_rgPermanentEffects.clear();
	}

	m_rgActiveEffects.clear();
}

void EffectDispatcher::ClearActiveEffects(const EffectIdentifier& exclude)
{
	for (auto it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end(); )
	{
		const ActiveEffect& effect = *it;

		if (effect.m_EffectIdentifier != exclude)
		{
			EffectThreads::StopThread(effect.m_ullThreadId);

			it = m_rgActiveEffects.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void EffectDispatcher::ClearMostRecentEffect()
{
	if (!m_rgActiveEffects.empty())
	{
		for (int i = m_rgActiveEffects.size() - 1; i >= 0; i--)
		{
			const ActiveEffect& mostRecentEffect = m_rgActiveEffects[i];
			if (!mostRecentEffect.m_bIsFake)
			{
				EffectThreads::StopThread(mostRecentEffect.m_ullThreadId);

				if (mostRecentEffect.m_fTimer > 0)
				{
					m_rgActiveEffects.erase(m_rgActiveEffects.end() - 1);
				}

				break;
			}
		}
	}
}

void EffectDispatcher::ClearAllFakeEffects()
{
	if (!m_rgActiveEffects.empty())
	{
		for (int i = m_rgActiveEffects.size() - 1; i >= 0; i--)
		{
			ActiveEffect& activeEffect = m_rgActiveEffects[i];
			if (activeEffect.m_bIsFake)
			{
				EffectThreads::StopThread(activeEffect.m_ullThreadId);

				m_rgActiveEffects.erase(m_rgActiveEffects.end() - 1);
			}
		}
	}
}

void EffectDispatcher::Reset()
{
	ClearEffects();
	ResetTimer();

	m_bEnableNormalEffectDispatch = false;
	m_bMetaEffectsEnabled = true;
	m_iMetaEffectTimer = m_usMetaEffectSpawnTime;
	m_ullMetaTimer = GetTickCount64();

	for (const auto& [ effectIdentifier, effectData ] : g_dictEnabledEffects)
	{
		if (effectData.TimedType == EEffectTimedType::Permanent)
		{
			// Always run permanent timed effects in background
			RegisteredEffect* pRegisteredEffect = GetRegisteredEffect(effectIdentifier);

			if (pRegisteredEffect)
			{
				m_rgPermanentEffects.push_back(pRegisteredEffect);

				EffectThreads::CreateThread(pRegisteredEffect, true);
			}
		}
		else if (!effectData.IsMeta() && !effectData.IsUtility())
		{
			// There's at least 1 enabled non-permanent effect, enable timer
			m_bEnableNormalEffectDispatch = true;
		}
	}
}

void EffectDispatcher::ResetTimer()
{
	m_ullTimerTimer = GetTickCount64();
	m_usTimerTimerRuns = 0;
	m_ullEffectsTimer = GetTickCount64();
}

float EffectDispatcher::GetEffectTopSpace()
{
	if (m_bEnableTwitchVoting
		&& (m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayIngame
			|| m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayOBS))
	{
		return m_fEffectsTopSpacingWithVoting;
	}
	return m_fEffectsTopSpacingDefault;
}

bool EffectDispatcher::ShouldRemoveEffectForTimeOut(int timer, int effectCount, int minAmountAdvancedCleaning)
{
	float additionalTime = 0;
	if (effectCount > minAmountAdvancedCleaning)
	{
		additionalTime = min((min(effectCount, 10) - 3) * 20, 160);
	}
	return timer < -m_usEffectTimedDur + additionalTime;
}

// (kolyaventuri): Forces the name of the provided effect to change, using any given string
void EffectDispatcher::OverrideEffectName(EEffectType eEffectType, const std::string& szOverrideName)
{
	for (ActiveEffect& effect : m_rgActiveEffects)
	{
		if (effect.m_EffectIdentifier.GetEffectType() == eEffectType)
		{
			effect.m_szFakeName = szOverrideName;
		}
	}
}

// (kolyaventuri): Forces the name of the provided effect to change, using the defined name of another effect
void EffectDispatcher::OverrideEffectName(EEffectType eEffectType, EEffectType eFakeEffectType) {
	for (ActiveEffect& effect : m_rgActiveEffects)
	{
		if (effect.m_EffectIdentifier.GetEffectType() == eEffectType)
		{
			EffectInfo fakeEffectInfo = g_dictEffectsMap.find(eFakeEffectType)->second;
			effect.m_szFakeName = fakeEffectInfo.Name;
		}
	}
}
