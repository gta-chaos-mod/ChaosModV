#include "stdafx.h"

EffectDispatcher::EffectDispatcher(const std::array<BYTE, 3>& rgTimerColor, const std::array<BYTE, 3>& rgTextColor, const std::array<BYTE, 3>& rgEffectTimerColor)
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

	m_iMetaEffectTimer = m_usMetaEffectSpawnTime;

	m_bEnableTwitchVoting = g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	m_eTwitchOverlayMode = static_cast<ETwitchOverlayMode>(g_OptionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	ClearEffects();
}

void EffectDispatcher::Run()
{
	UpdateEffects();

	if (!m_bPauseTimer)
	{
		if (!g_MetaInfo.m_bDisableChaos)
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
		fDelta = 0;
	}

	if ((m_fPercentage = (fDelta + (m_usTimerTimerRuns * 1000)) / (m_usEffectSpawnTime / g_MetaInfo.m_fTimerSpeedModifier * 1000)) > 1.f
		&& m_bDispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		if (g_MetaInfo.m_ucAdditionalEffectsToDispatch > 0)
		{
			for (BYTE ucIdx = 0; ucIdx < g_MetaInfo.m_ucAdditionalEffectsToDispatch; ucIdx++)
			{
				g_pEffectDispatcher->DispatchRandomEffect();
			}
		}

		m_usTimerTimerRuns = 0;
	}
}

void EffectDispatcher::UpdateEffects()
{
	EffectThreads::RunThreads();

	// Don't continue if there are no enabled effects
	if (g_EnabledEffects.empty())
	{
		return;
	}

	for (ActiveEffect& effect : m_rgActiveEffects)
	{
		if (effect.m_bHideText
			&& EffectThreads::HasThreadOnStartExecuted(effect.m_ullThreadId))
		{
			effect.m_bHideText = false;
		}
	}

	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - m_ullEffectsTimer) > 1000)
	{
		m_ullEffectsTimer = currentUpdateTime;

		int activeEffectsSize = m_rgActiveEffects.size();
		std::vector<ActiveEffect>::iterator it;
		for (it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end(); )
		{
			ActiveEffect& effect = *it;
			EffectData& effectData = g_EnabledEffects.at(effect.m_EffectIdentifier);
			if (effectData.IsMeta)
			{
				effect.m_fTimer--;
			}
			else
			{
				effect.m_fTimer -= 1 / g_MetaInfo.m_fEffectDurationModifier;
			}

			if ((effect.m_fMaxTime > 0 && effect.m_fTimer <= 0)
				|| ((!effectData.IsMeta)
					&& (effect.m_fTimer < -m_usEffectTimedDur + (activeEffectsSize > 3
						? ((activeEffectsSize - 3) * 20 < 160
							? (activeEffectsSize - 3) * 20
							: 160)
						: 0))))
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
}

void EffectDispatcher::UpdateMetaEffects()
{
	if (m_bMetaEffectsEnabled)
	{
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
			for (auto& pair : g_EnabledEffects)
			{
				if (pair.second.IsMeta && pair.second.TimedType != EEffectTimedType::Permanent)
				{
					auto& [effectIdentifier, effectData] = pair;

					totalWeight += GetEffectWeight(effectData);

					availableMetaEffects.push_back(std::make_tuple(effectIdentifier, &pair.second));
				}
			}

			if (!availableMetaEffects.empty())
			{
				// TODO: Stop duplicating effect weight logic everywhere
				float chosen = g_Random.GetRandomFloat(0.f, totalWeight);

				totalWeight = 0.f;

				const EffectIdentifier* targetEffectIdentifier = nullptr;
				for (auto& pair : availableMetaEffects)
				{
					auto& [effectIdentifier, effectData] = pair;

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
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_bEnableNormalEffectDispatch
		|| m_bDisableDrawTimerBar
		|| g_MetaInfo.m_bShouldHideChaosUI
		|| g_MetaInfo.m_bDisableChaos)
	{
		return;
	}

	float fPercentage = m_fFakeTimerBarPercentage > 0.f
		&& m_fFakeTimerBarPercentage <= 1.f
			? m_fFakeTimerBarPercentage
			: m_fPercentage;

	// New Effect Bar
	DRAW_RECT(.5f, .01f, 1.f, .021f, 0, 0, 0, 127, false);
	DRAW_RECT(fPercentage * .5f, .01f, fPercentage, .018f, m_rgTimerColor[0], m_rgTimerColor[1], m_rgTimerColor[2], 255, false);
}

void EffectDispatcher::DrawEffectTexts()
{
	if (!m_bEnableNormalEffectDispatch
		|| m_bDisableDrawEffectTexts)
	{
		return;
	}

	float fPosY = .2f;
	if (m_bEnableTwitchVoting
		&& (m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayIngame
			|| m_eTwitchOverlayMode == ETwitchOverlayMode::OverlayOBS))
	{
		fPosY = .35f;
	}

	for (const ActiveEffect& effect : m_rgActiveEffects)
	{
		const bool bHasFake = !effect.m_szFakeName.empty();

		if ((effect.m_bHideText && !bHasFake)
			|| (g_MetaInfo.m_bShouldHideChaosUI
				&& effect.m_EffectIdentifier.GetEffectType() != EFFECT_META_HIDE_CHAOS_UI)
			|| (g_MetaInfo.m_bDisableChaos
				&& effect.m_EffectIdentifier.GetEffectType() != EFFECT_META_NO_CHAOS))
		{
			continue;
		}

		std::string name = effect.m_szFakeName;

		if (!effect.m_bHideText || !bHasFake)
		{
			name = effect.m_szName;
		}

		DrawScreenText(name, { .915f, fPosY }, .47f, { m_rgTextColor[0], m_rgTextColor[1], m_rgTextColor[2] }, true,
			EScreenTextAdjust::Right, { .0f, .915f });

		if (effect.m_fTimer > 0)
		{
			DRAW_RECT(.96f, fPosY + .0185f, .05f, .019f, 0, 0, 0, 127, false);
			DRAW_RECT(.96f, fPosY + .0185f, .048f * effect.m_fTimer / effect.m_fMaxTime, .017f, m_rgEffectTimerColor[0], m_rgEffectTimerColor[1],
				m_rgEffectTimerColor[2], 255, false);
		}

		fPosY += .075f;
	}
}

bool _NODISCARD EffectDispatcher::ShouldDispatchEffectNow() const
{
	return GetRemainingTimerTime() <= 0;
}

int _NODISCARD EffectDispatcher::GetRemainingTimerTime() const
{
	return m_usEffectSpawnTime / g_MetaInfo.m_fTimerSpeedModifier - m_usTimerTimerRuns;
}

void EffectDispatcher::DispatchEffect(const EffectIdentifier& effectIdentifier, const char* szSuffix)
{
	EffectData& effectData = g_EnabledEffects.at(effectIdentifier);
	if (effectData.TimedType == EEffectTimedType::Permanent)
	{
		return;
	}

	// Increase weight for all effects first
	for (auto& pair : g_EnabledEffects)
	{
		EffectData& effectData = pair.second;

		if (!effectData.IsMeta)
		{
			effectData.Weight += effectData.WeightMult;
		}
	}

	// Reset weight of this effect (or every effect in group) to reduce chance of same effect (group) happening multiple times in a row
	if (effectData.EEffectGroupType == EEffectGroupType::None)
	{
		effectData.Weight = effectData.WeightMult;
	}
	else
	{
		for (auto& pair : g_EnabledEffects)
		{
			if (pair.second.EEffectGroupType == effectData.EEffectGroupType)
			{
				pair.second.Weight = pair.second.WeightMult;
			}
		}
	}

	LOG("Dispatched effect \"" << effectData.Name << "\"");

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool bAlreadyExists = false;

	const auto& rgIncompatibleIds = effectData.IncompatibleIds;

	for (auto it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end(); )
	{
		ActiveEffect& activeEffect = *it;

		if (activeEffect.m_EffectIdentifier == effectIdentifier
			&& effectData.TimedType != EEffectTimedType::Unk
			&& effectData.TimedType != EEffectTimedType::NotTimed)
		{
			bAlreadyExists = true;
			activeEffect.m_fTimer = activeEffect.m_fMaxTime;

			break;
		}

		bool bFound = false;
		if (std::find(rgIncompatibleIds.begin(), rgIncompatibleIds.end(), g_EnabledEffects.at(activeEffect.m_EffectIdentifier).Id)
			!= rgIncompatibleIds.end())
		{
			bFound = true;
		}

		// Check if current effect is marked as incompatible in active effect
		if (!bFound)
		{
			const auto& rgActiveIncompatibleIds = g_EnabledEffects.at(activeEffect.m_EffectIdentifier).IncompatibleIds;

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

	if (!bAlreadyExists)
	{
		RegisteredEffect* registeredEffect = GetRegisteredEffect(effectIdentifier);

		if (registeredEffect)
		{
			std::ostringstream ossEffectName;
			ossEffectName << (effectData.HasCustomName ? effectData.CustomName : effectData.Name);

			if (szSuffix && strlen(szSuffix) > 0)
			{
				ossEffectName << " " << szSuffix;
			}

			ossEffectName << std::endl;

			if (!g_MetaInfo.m_bShouldHideChaosUI)
			{
				// Play global sound (if one exists)
				// Workaround: Force no global sound for "Fake Crash" and "Fake Death"
				if (effectIdentifier.GetEffectType() != EFFECT_MISC_CRASH
					&& effectIdentifier.GetEffectType() != EFFECT_PLAYER_FAKEDEATH)
				{
					Mp3Manager::PlayChaosSoundFile("global_effectdispatch");
				}

				// Play a sound if corresponding .mp3 file exists
				Mp3Manager::PlayChaosSoundFile(effectData.Id);
			}

			int effectTime = -1;
			switch (effectData.TimedType)
			{
			case EEffectTimedType::Normal:
				effectTime = effectData.IsMeta
					? m_usMetaEffectTimedDur
					: m_usEffectTimedDur;
				break;
			case EEffectTimedType::Short:
				effectTime = effectData.IsMeta
					? m_usMetaEffectShortDur
					: m_usEffectTimedShortDur;
				break;
			case EEffectTimedType::Custom:
				effectTime = effectData.CustomTime;
				break;
			}

			m_rgActiveEffects.emplace_back(effectIdentifier, registeredEffect, ossEffectName.str(), effectData.FakeName, effectTime);
		}
	}

	m_fPercentage = .0f;
}

void EffectDispatcher::DispatchRandomEffect(const char* szSuffix)
{
	if (!m_bEnableNormalEffectDispatch)
	{
		return;
	}

	std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
	for (const auto& pair : g_EnabledEffects)
	{
		const auto& [effectIdentifier, effectData] = pair;

		if (effectData.TimedType != EEffectTimedType::Permanent && !effectData.IsMeta)
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
		DispatchEffect(*pTargetEffectIdentifier, szSuffix);
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
		const ActiveEffect& mostRecentEffect = m_rgActiveEffects[m_rgActiveEffects.size() - 1];

		if (mostRecentEffect.m_fTimer > 0)
		{
			EffectThreads::StopThread(mostRecentEffect.m_ullThreadId);

			m_rgActiveEffects.erase(m_rgActiveEffects.end() - 1);
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

	for (const auto& pair : g_EnabledEffects)
	{
		const auto& [effectIdentifier, effectData] = pair;

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
		else
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
