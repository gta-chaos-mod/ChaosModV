#include <stdafx.h>

#include "EffectDispatcher.h"
#include "Mp3Manager.h"

#include "Components/TwitchVoting.h"

#include "Effects/EEffectCategory.h"
#include "Effects/MetaModifiers.h"

#include "Util/OptionsManager.h"
#include "Util/Random.h"
#include "Util/Text.h"

static void _DispatchEffect(EffectDispatcher *effectDispatcher, const EffectDispatcher::EffectDispatchEntry &entry)
{
	EffectData &effectData = g_dictEnabledEffects.at(entry.Identifier);
	if (effectData.TimedType == EEffectTimedType::Permanent)
	{
		return;
	}

	if (!effectDispatcher->OnPreDispatchEffect.Fire(entry.Identifier))
	{
		return;
	}

	LOG("Dispatching effect \"" << effectData.Name << "\"");

	// Increase weight for all effects first
	for (auto &[effectId, effectData] : g_dictEnabledEffects)
	{
		if (!effectData.IsMeta())
		{
			effectData.Weight += effectData.WeightMult;
		}
	}

	// Reset weight of this effect (or every effect in group) to reduce chance of same effect (group) happening multiple
	// times in a row
	if (effectData.GroupType.empty())
	{
		effectData.Weight = effectData.WeightMult;
	}
	else
	{
		for (auto &[effectId, effectData] : g_dictEnabledEffects)
		{
			if (effectData.GroupType == effectData.GroupType)
			{
				effectData.Weight = effectData.WeightMult;
			}
		}
	}

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool bAlreadyExists           = false;

	const auto &rgIncompatibleIds = effectData.IncompatibleIds;

	for (auto it = effectDispatcher->m_rgActiveEffects.begin(); it != effectDispatcher->m_rgActiveEffects.end();)
	{
		auto &activeEffect     = *it;
		auto &activeEffectData = g_dictEnabledEffects.at(activeEffect.m_EffectIdentifier);

		if (activeEffect.m_EffectIdentifier == entry.Identifier)
		{
			if (effectData.TimedType != EEffectTimedType::Unk && effectData.TimedType != EEffectTimedType::NotTimed)
			{
				bAlreadyExists       = true;
				activeEffect.m_Timer = activeEffect.m_MaxTime;
			}
			else
			{
				EffectThreads::StopThreadImmediately(activeEffect.m_ThreadId);
				it = effectDispatcher->m_rgActiveEffects.erase(it);
			}

			break;
		}

		bool bFound = false;
		if (std::find(rgIncompatibleIds.begin(), rgIncompatibleIds.end(), activeEffectData.Id)
		    != rgIncompatibleIds.end())
		{
			bFound = true;
		}

		// Check if current effect is either the same effect category or marked as incompatible in active effect
		if (!bFound)
		{
			const auto &rgActiveIncompatibleIds = activeEffectData.IncompatibleIds;
			if ((effectData.EffectCategory != EEffectCategory::None
			     && effectData.EffectCategory == activeEffectData.EffectCategory)
			    || std::find(rgActiveIncompatibleIds.begin(), rgActiveIncompatibleIds.end(), effectData.Id)
			           != rgActiveIncompatibleIds.end())
			{
				bFound = true;
			}
		}

		if (bFound)
		{
			EffectThreads::StopThread(activeEffect.m_ThreadId);
			activeEffect.m_IsStopping = true;
		}

		it++;
	}

	if (!bAlreadyExists)
	{
		RegisteredEffect *registeredEffect = GetRegisteredEffect(entry.Identifier);

		if (registeredEffect)
		{
			std::ostringstream ossEffectName;
			ossEffectName << (effectData.HasCustomName() ? effectData.CustomName : effectData.Name);

			if (entry.Suffix && strlen(entry.Suffix) > 0)
			{
				ossEffectName << " " << entry.Suffix;
			}

			ossEffectName << std::endl;

			if (!MetaModifiers::m_bHideChaosUI)
			{
				// Play global sound (if one exists)
				// HACK: Force no global sound for "Fake Crash"
				if (entry.Identifier.GetEffectId() != "misc_fakecrash")
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
				effectTime = effectData.IsMeta() ? effectDispatcher->m_usMetaEffectTimedDur
				                                 : effectDispatcher->m_usEffectTimedDur;
				break;
			case EEffectTimedType::Short:
				effectTime = effectData.IsMeta() ? effectDispatcher->m_usMetaEffectShortDur
				                                 : effectDispatcher->m_usEffectTimedShortDur;
				break;
			case EEffectTimedType::Custom:
				effectTime = effectData.CustomTime;
				break;
			}

			effectDispatcher->m_rgActiveEffects.emplace_back(entry.Identifier, registeredEffect, ossEffectName.str(),
			                                                 effectData.FakeName, effectTime);

			// There might be a reason to include meta effects in the future, for now we will just exclude them
			if (entry.AddToLog && !effectData.IsMeta() && !effectData.IsTemporary() && !effectData.IsUtility())
			{
				if (effectDispatcher->m_rgDispatchedEffectsLog.size() >= 100)
				{
					effectDispatcher->m_rgDispatchedEffectsLog.erase(
					    effectDispatcher->m_rgDispatchedEffectsLog.begin());
				}

				effectDispatcher->m_rgDispatchedEffectsLog.emplace_back(registeredEffect);
			}
		}
	}

	effectDispatcher->OnPostDispatchEffect.Fire(entry.Identifier);
}

static void _OnRunEffects(LPVOID data)
{
	auto effectDispatcher = reinterpret_cast<EffectDispatcher *>(data);
	while (true)
	{
		DWORD64 ullCurrentUpdateTime = GetTickCount64();
		int iDeltaTime               = ullCurrentUpdateTime - effectDispatcher->m_ullTimer;

		// the game was paused
		if (iDeltaTime > 1000)
		{
			iDeltaTime = 0;
		}

		while (!effectDispatcher->m_EffectDispatchQueue.empty())
		{
			_DispatchEffect(effectDispatcher, effectDispatcher->m_EffectDispatchQueue.front());
			effectDispatcher->m_EffectDispatchQueue.pop();
		}

		effectDispatcher->UpdateEffects(iDeltaTime);

		if (!effectDispatcher->m_bPauseTimer)
		{
			effectDispatcher->UpdateMetaEffects(iDeltaTime);
		}

		SwitchToFiber(g_MainThread);
	}
}

EffectDispatcher::EffectDispatcher(const std::array<BYTE, 3> &rgTimerColor, const std::array<BYTE, 3> &rgTextColor,
                                   const std::array<BYTE, 3> &rgEffectTimerColor)
    : Component()
{
	m_rgTimerColor         = rgTimerColor;
	m_rgTextColor          = rgTextColor;
	m_rgEffectTimerColor   = rgEffectTimerColor;

	m_bDisableDrawTimerBar = g_OptionsManager.GetConfigValue<bool>("DisableTimerBarDraw", OPTION_DEFAULT_NO_EFFECT_BAR);
	m_bDisableDrawEffectTexts =
	    g_OptionsManager.GetConfigValue<bool>("DisableEffectTextDraw", OPTION_DEFAULT_NO_TEXT_DRAW);

	m_usEffectSpawnTime = g_OptionsManager.GetConfigValue<int>("NewEffectSpawnTime", OPTION_DEFAULT_EFFECT_SPAWN_TIME);
	m_usEffectTimedDur  = g_OptionsManager.GetConfigValue<int>("EffectTimedDur", OPTION_DEFAULT_EFFECT_TIMED_DUR);
	m_usEffectTimedShortDur =
	    g_OptionsManager.GetConfigValue<int>("EffectTimedShortDur", OPTION_DEFAULT_EFFECT_SHORT_TIMED_DUR);

	m_usMetaEffectSpawnTime =
	    g_OptionsManager.GetConfigValue<int>("NewMetaEffectSpawnTime", OPTION_DEFAULT_EFFECT_META_SPAWN_TIME);
	m_usMetaEffectTimedDur =
	    g_OptionsManager.GetConfigValue<int>("MetaEffectDur", OPTION_DEFAULT_EFFECT_META_TIMED_DUR);
	m_usMetaEffectShortDur =
	    g_OptionsManager.GetConfigValue<int>("MetaShortEffectDur", OPTION_DEFAULT_EFFECT_META_SHORT_TIMED_DUR);

	m_iMaxRunningEffects =
	    g_OptionsManager.GetConfigValue<int>("MaxParallelRunningEffects", OPTION_DEFAULT_MAX_RUNNING_PARALLEL_EFFECTS);

	m_bEnableTwitchVoting =
	    g_OptionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	m_eTwitchOverlayMode = static_cast<ETwitchOverlayMode>(
	    g_OptionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	Reset();

	if (g_EffectDispatcherThread)
	{
		DeleteFiber(g_EffectDispatcherThread);
	}
	g_EffectDispatcherThread = CreateFiber(0, _OnRunEffects, this);
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
	DWORD64 ullCurrentUpdateTime = GetTickCount64();
	int iDeltaTime               = ullCurrentUpdateTime - m_ullTimer;

	// the game was paused
	if (iDeltaTime > 1000)
	{
		iDeltaTime = 0;
	}

	if (!m_bPauseTimer)
	{
		UpdateTimer(iDeltaTime);
	}

	DrawTimerBar();

	if (g_EffectDispatcherThread)
	{
		SwitchToFiber(g_EffectDispatcherThread);
	}

	DrawEffectTexts();

	m_ullTimer = ullCurrentUpdateTime;
}

void EffectDispatcher::UpdateTimer(int iDeltaTime)
{
	if (!m_bEnableNormalEffectDispatch || MetaModifiers::m_bDisableChaos)
	{
		return;
	}

	m_fTimerPercentage += iDeltaTime * MetaModifiers::m_fTimerSpeedModifier / m_usEffectSpawnTime / 1000;

	if (m_fTimerPercentage >= 1.f && m_bDispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		for (BYTE ucIdx = 0; ucIdx < MetaModifiers::m_ucAdditionalEffectsToDispatch; ucIdx++)
		{
			DispatchRandomEffect();
		}

		m_fTimerPercentage = 0.f;
	}
}

void EffectDispatcher::UpdateEffects(int iDeltaTime)
{
	if (m_ClearEffectsState != ClearEffectsState::None)
	{
		m_rgActiveEffects.clear();
		m_PermanentEffects.clear();
		m_rgDispatchedEffectsLog.clear();

		EffectThreads::StopThreadsImmediately();

		// Ensure player control isn't stuck in disabled state
		SET_PLAYER_CONTROL(PLAYER_ID(), true, 0);
		// Clear screen effects as well
		ANIMPOSTFX_STOP_ALL();
		POP_TIMECYCLE_MODIFIER();

		if (m_ClearEffectsState == ClearEffectsState::AllRestartPermanent)
		{
			RegisterPermanentEffects();
		}

		m_ClearEffectsState = ClearEffectsState::None;
	}

	for (auto threadId : m_PermanentEffects)
	{
		EffectThreads::RunThread(threadId);
	}

	float fDeltaTime = (float)iDeltaTime / 1000;

	int maxEffects =
	    std::min((int)(floor((1.0f - GetEffectTopSpace()) / m_fEffectsInnerSpacingMin) - 1), m_iMaxRunningEffects);
	int effectCountToCheckCleaning = 3;
	for (auto it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end();)
	{
		auto &effect = *it;

		if (EffectThreads::DoesThreadExist(effect.m_ThreadId) && !EffectThreads::IsThreadPaused(effect.m_ThreadId))
		{
			OnPreRunEffect.Fire(effect.m_EffectIdentifier);
			EffectThreads::RunThread(effect.m_ThreadId);
			OnPostRunEffect.Fire(effect.m_EffectIdentifier);
		}

		if (effect.m_HideText && EffectThreads::HasThreadOnStartExecuted(effect.m_ThreadId))
		{
			effect.m_HideText = false;
		}

		bool isTimed = false;
		bool isMeta  = false;
		// Temporary non-timed effects will have their entries removed already since their OnStop is called immediately
		if (g_dictEnabledEffects.contains(effect.m_EffectIdentifier))
		{
			EffectData &effectData = g_dictEnabledEffects.at(effect.m_EffectIdentifier);
			isTimed =
			    effectData.TimedType != EEffectTimedType::NotTimed && effectData.TimedType != EEffectTimedType::Unk;
			isMeta = effectData.IsMeta();
		}

		if (effect.m_MaxTime > 0)
		{
			if (isMeta)
			{
				effect.m_Timer -= fDeltaTime;
			}
			else
			{
				effect.m_Timer -= fDeltaTime / MetaModifiers::m_fEffectDurationModifier;
			}
		}
		else if (!isTimed)
		{
			float t = m_usEffectTimedDur, m = maxEffects, n = effectCountToCheckCleaning;
			// ensure effects stay on screen for at least 5 seconds
			effect.m_Timer +=
			    fDeltaTime / t * (1.f + (t / 5 - 1) * std::max(0.f, m_rgActiveEffects.size() - n) / (m - n));
		}

		if (effect.m_IsStopping)
		{
			EffectThreads::StopThreadImmediately(effect.m_ThreadId);
			it = m_rgActiveEffects.erase(it);
		}
		else
		{
			if (effect.m_MaxTime > 0 && effect.m_Timer <= 0
			    || !isTimed && (m_rgActiveEffects.size() > maxEffects || effect.m_Timer >= 0.f) && !effect.m_IsStopping)
			{
				EffectThreads::StopThread(effect.m_ThreadId);
				effect.m_IsStopping = true;
			}

			it++;
		}
	}
}

void EffectDispatcher::UpdateMetaEffects(int iDeltaTime)
{
	if (!m_bMetaEffectsEnabled)
	{
		return;
	}

	m_fMetaEffectTimerPercentage += (float)iDeltaTime / m_usMetaEffectSpawnTime / 1000;

	if (m_fMetaEffectTimerPercentage >= 1.f)
	{
		m_fMetaEffectTimerPercentage = 0.f;

		std::vector<std::tuple<EffectIdentifier, EffectData *>> availableMetaEffects;

		float totalWeight = 0.f;
		for (auto &[effectId, effectData] : g_dictEnabledEffects)
		{
			if (effectData.IsMeta() && !effectData.IsUtility() && !effectData.IsHidden())
			{
				totalWeight += GetEffectWeight(effectData);

				availableMetaEffects.push_back(std::make_tuple(effectId, &effectData));
			}
		}

		if (!availableMetaEffects.empty())
		{
			// TODO: Stop duplicating effect weight logic everywhere
			float chosen                                   = g_Random.GetRandomFloat(0.f, totalWeight);

			totalWeight                                    = 0.f;

			const EffectIdentifier *targetEffectIdentifier = nullptr;
			for (const auto &[effectIdentifier, effectData] : availableMetaEffects)
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
				_DispatchEffect(this, { .Identifier = *targetEffectIdentifier, .Suffix = "(Meta)", .AddToLog = false });
			}
		}
		else
		{
			m_bMetaEffectsEnabled        = false;
			m_fMetaEffectTimerPercentage = 0.f;
		}
	}
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_bEnableNormalEffectDispatch || m_bDisableDrawTimerBar || MetaModifiers::m_bHideChaosUI
	    || MetaModifiers::m_bDisableChaos)
	{
		return;
	}

	float fPercentage = m_fFakeTimerBarPercentage > 0.f && m_fFakeTimerBarPercentage <= 1.f ? m_fFakeTimerBarPercentage
	                                                                                        : m_fTimerPercentage;

	// New Effect Bar
	DRAW_RECT(.5f, .01f, 1.f, .021f, 0, 0, 0, 127, false);

	if (MetaModifiers::m_bFlipChaosUI)
	{
		DRAW_RECT(1.f - fPercentage * .5f, .01f, fPercentage, .018f, m_rgTimerColor[0], m_rgTimerColor[1],
		          m_rgTimerColor[2], 255, false);
	}
	else
	{
		DRAW_RECT(fPercentage * .5f, .01f, fPercentage, .018f, m_rgTimerColor[0], m_rgTimerColor[1], m_rgTimerColor[2],
		          255, false);
	}
}

void EffectDispatcher::DrawEffectTexts()
{
	if (m_bDisableDrawEffectTexts)
	{
		return;
	}

	float fPosY         = GetEffectTopSpace();
	float effectSpacing = m_fEffectsInnerSpacingMax;

	if (m_rgActiveEffects.size() > 0)
	{
		effectSpacing = std::min(m_fEffectsInnerSpacingMax,
		                         std::max(m_fEffectsInnerSpacingMin, (1.0f - fPosY) / m_rgActiveEffects.size()));
	}

	for (const ActiveEffect &effect : m_rgActiveEffects)
	{
		const bool bHasFake = !effect.m_FakeName.empty();

		// Temporary non-timed effects will have their entries removed already since their OnStop is called immediately
		if (g_dictEnabledEffects.contains(effect.m_EffectIdentifier))
		{
			auto &effectData = g_dictEnabledEffects.at(effect.m_EffectIdentifier);
			if ((effect.m_HideText && !bHasFake)
			    || ((MetaModifiers::m_bHideChaosUI || MetaModifiers::m_bDisableChaos) && !effectData.IsMeta()
			        && !effectData.IsUtility() && !effectData.IsTemporary()))
			{
				continue;
			}
		}

		std::string name = effect.m_Name;
		if (effect.m_HideText && bHasFake)
		{
			name = effect.m_FakeName;
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

		if (effect.m_MaxTime > 0)
		{
			if (MetaModifiers::m_bFlipChaosUI)
			{
				DRAW_RECT(.04f, fPosY + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.04f, fPosY + .0185f, .048f * (1.f - (effect.m_Timer / effect.m_MaxTime)), .017f,
				          m_rgEffectTimerColor[0], m_rgEffectTimerColor[1], m_rgEffectTimerColor[2], 255, false);
			}
			else
			{
				DRAW_RECT(.96f, fPosY + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.96f, fPosY + .0185f, .048f * effect.m_Timer / effect.m_MaxTime, .017f,
				          m_rgEffectTimerColor[0], m_rgEffectTimerColor[1], m_rgEffectTimerColor[2], 255, false);
			}
		}

		fPosY += effectSpacing;
	}
}

bool EffectDispatcher::ShouldDispatchEffectNow() const
{
	return GetRemainingTimerTime() <= 0;
}

int EffectDispatcher::GetRemainingTimerTime() const
{
	return std::ceil(m_usEffectSpawnTime / MetaModifiers::m_fTimerSpeedModifier * (1 - m_fTimerPercentage));
}

void EffectDispatcher::DispatchEffect(const EffectIdentifier &effectIdentifier, const char *szSuffix, bool bAddToLog)
{
	m_EffectDispatchQueue.push({ .Identifier = effectIdentifier, .Suffix = szSuffix, .AddToLog = bAddToLog });
}

void EffectDispatcher::DispatchRandomEffect(const char *szSuffix)
{
	if (!m_bEnableNormalEffectDispatch)
	{
		return;
	}

	std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
	for (const auto &[effectIdentifier, effectData] : g_dictEnabledEffects)
	{
		if (!effectData.IsMeta() && !effectData.IsUtility() && !effectData.IsHidden())
		{
			choosableEffects.emplace(effectIdentifier, effectData);
		}
	}

	float fTotalWeight = 0.f;
	for (const auto &[effectIdentifier, effectData] : choosableEffects)
	{
		fTotalWeight += GetEffectWeight(effectData);
	}

	float fChosen                                   = g_Random.GetRandomFloat(0.f, fTotalWeight);

	fTotalWeight                                    = 0.f;

	const EffectIdentifier *pTargetEffectIdentifier = nullptr;
	for (const auto &[effectIdentifier, effectData] : choosableEffects)
	{
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

void EffectDispatcher::ClearEffect(const EffectIdentifier &effectId)
{
	auto result = std::find_if(m_rgActiveEffects.begin(), m_rgActiveEffects.end(),
	                           [effectId](auto &activeEffect) { return activeEffect.m_EffectIdentifier == effectId; });
	if (result == m_rgActiveEffects.end())
	{
		return;
	}

	EffectThreads::StopThread(result->m_ThreadId);
	result->m_IsStopping = true;
}

void EffectDispatcher::ClearEffects(bool bRestartPermanentEffects)
{
	m_ClearEffectsState = bRestartPermanentEffects ? ClearEffectsState::AllRestartPermanent : ClearEffectsState::All;
}

void EffectDispatcher::ClearActiveEffects(const EffectIdentifier &exclude)
{
	for (auto it = m_rgActiveEffects.begin(); it != m_rgActiveEffects.end();)
	{
		ActiveEffect &effect = *it;

		if (effect.m_EffectIdentifier != exclude)
		{
			EffectThreads::StopThread(effect.m_ThreadId);
			effect.m_IsStopping = true;
		}

		it++;
	}
}

void EffectDispatcher::ClearMostRecentEffect()
{
	if (!m_rgActiveEffects.empty())
	{
		ActiveEffect &mostRecentEffect = m_rgActiveEffects[m_rgActiveEffects.size() - 1];

		if (mostRecentEffect.m_Timer > 0)
		{
			EffectThreads::StopThread(mostRecentEffect.m_ThreadId);
			mostRecentEffect.m_IsStopping = true;
		}
	}
}

std::vector<RegisteredEffect *> EffectDispatcher::GetRecentEffects(int distance, std::string_view ignoreEffect) const
{
	std::vector<RegisteredEffect *> effects;

	for (int i = m_rgDispatchedEffectsLog.size() - 1; distance > 0 && i >= 0; i--)
	{
		auto effect = *std::next(m_rgDispatchedEffectsLog.begin(), i);
		if ((!ignoreEffect.empty() && effect->GetIndentifier().GetEffectId() == ignoreEffect)
		    || std::find(effects.begin(), effects.end(), effect) != effects.end())
		{
			continue;
		}

		effects.emplace_back(effect);
		distance--;
	}

	return effects;
}

void EffectDispatcher::Reset(bool bRestartPermanentEffects)
{
	ClearEffects(bRestartPermanentEffects);
	ResetTimer();

	m_bEnableNormalEffectDispatch = false;
	m_bMetaEffectsEnabled         = true;
	m_fMetaEffectTimerPercentage  = 0.f;
}

void EffectDispatcher::ResetTimer()
{
	m_fTimerPercentage = 0.f;
	m_ullTimer         = GetTickCount64();
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

void EffectDispatcher::RegisterPermanentEffects()
{
	for (const auto &[effectIdentifier, effectData] : g_dictEnabledEffects)
	{
		if (effectData.TimedType == EEffectTimedType::Permanent)
		{
			// Always run permanent timed effects in background
			RegisteredEffect *pRegisteredEffect = GetRegisteredEffect(effectIdentifier);
			if (pRegisteredEffect)
			{
				auto threadId = EffectThreads::CreateThread(pRegisteredEffect, true);
				m_PermanentEffects.push_back(threadId);
			}
		}
		else if (!effectData.IsMeta() && !effectData.IsUtility())
		{
			// There's at least 1 enabled non-permanent effect, enable timer
			m_bEnableNormalEffectDispatch = true;
		}
	}
}

// (kolyaventuri): Forces the name of the provided effect to change, using any given string
void EffectDispatcher::OverrideEffectName(std::string_view effectId, const std::string &szOverrideName)
{
	for (auto &effect : m_rgActiveEffects)
	{
		if (effect.m_EffectIdentifier.GetEffectId() == effectId)
		{
			effect.m_FakeName = szOverrideName;
		}
	}
}

// (kolyaventuri): Forces the name of the provided effect to change, using the defined name of another effect
void EffectDispatcher::OverrideEffectNameId(std::string_view effectId, std::string_view fakeEffectId)
{
	for (auto &effect : m_rgActiveEffects)
	{
		if (effect.m_EffectIdentifier.GetEffectId() == effectId)
		{
			auto effectIdentifier = EffectIdentifier(std::string(fakeEffectId));

			if (g_dictEnabledEffects.contains(effectIdentifier))
			{
				auto &fakeEffect  = g_dictEnabledEffects.at(effectIdentifier);
				effect.m_FakeName = fakeEffect.HasCustomName() ? fakeEffect.CustomName : fakeEffect.Name;
			}
			else
			{
				auto result = g_dictEffectsMap.find(fakeEffectId);
				if (result != g_dictEffectsMap.end())
				{
					effect.m_FakeName = result->second.Name;
				}
			}
		}
	}
}

bool EffectDispatcher::IsClearingEffects() const
{
	return m_ClearEffectsState != ClearEffectsState::None;
}