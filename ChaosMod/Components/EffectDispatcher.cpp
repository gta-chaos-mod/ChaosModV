#include <stdafx.h>

#include "EffectDispatcher.h"
#include "Mp3Manager.h"

#include "Components/EffectDispatchTimer.h"
#include "Components/MetaModifiers.h"

#include "Effects/EffectCategory.h"

#include "Util/OptionsManager.h"
#include "Util/Random.h"
#include "Util/ScriptText.h"

#define EFFECT_TEXT_INNER_SPACING_MIN .030f
#define EFFECT_TEXT_INNER_SPACING_MAX .075f
#define EFFECT_TEXT_TOP_SPACING .2f
#define EFFECT_TEXT_TOP_SPACING_EXTRA .35f

static void _DispatchEffect(EffectDispatcher *effectDispatcher, const EffectDispatcher::EffectDispatchEntry &entry)
{
	auto &effectData = g_EnabledEffects.at(entry.Identifier);

	if (effectData.TimedType == EffectTimedType::Permanent)
	{
		return;
	}

	if (!effectDispatcher->OnPreDispatchEffect.Fire(entry.Identifier))
	{
		return;
	}

	LOG("Dispatching effect \"" << effectData.Name << "\"");

	// Increase weight for all effects first
	for (auto &[effectId, effectData] : g_EnabledEffects)
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
		for (auto &[effectId, effectData] : g_EnabledEffects)
		{
			if (effectData.GroupType == effectData.GroupType)
			{
				effectData.Weight = effectData.WeightMult;
			}
		}
	}

	auto playEffectDispatchSound = [&]()
	{
		if ((!ComponentExists<MetaModifiers>() || GetComponent<MetaModifiers>()->HideChaosUI)
		    && ComponentExists<Mp3Manager>())
		{
			// Play global sound (if one exists)
			// HACK: Force no global sound for "Fake Crash"
			if (entry.Identifier.GetEffectId() != "misc_fakecrash")
			{
				GetComponent<Mp3Manager>()->PlayChaosSoundFile("global_effectdispatch");
			}

			// Play a sound if corresponding .mp3 file exists
			GetComponent<Mp3Manager>()->PlayChaosSoundFile(effectData.Id);
		}
	};

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool alreadyExists = false;

	for (auto &activeEffect : effectDispatcher->SharedState.ActiveEffects)
	{
		if (activeEffect.Identifier == entry.Identifier)
		{
			if (effectData.TimedType != EffectTimedType::NotTimed)
			{
				// Just extend timer of existing instance of timed effect
				alreadyExists      = true;
				activeEffect.Timer = activeEffect.MaxTime;

				playEffectDispatchSound();
			}
			else
			{
				// Replace previous instance of non-timed effect with this new one
				EffectThreads::StopThreadImmediately(activeEffect.ThreadId);
			}

			break;
		}

		const auto &activeEffectData = g_EnabledEffects.at(activeEffect.Identifier);

		bool isIncompatible          = false;
		if (effectData.IncompatibleIds.contains(activeEffectData.Id))
		{
			isIncompatible = true;
		}

		// Check if current effect is either the same effect category or marked as incompatible in active effect
		if (!isIncompatible)
		{
			const auto &activeIncompatibleIds = activeEffectData.IncompatibleIds;
			if ((effectData.EffectCategory != EffectCategory::None
			     && effectData.EffectCategory == activeEffectData.EffectCategory)
			    || activeIncompatibleIds.contains(effectData.Id))
			{
				isIncompatible = true;
			}
		}

		if (isIncompatible)
		{
			// No immediate effect thread stopping required here, do it gracefully
			EffectThreads::StopThread(activeEffect.ThreadId);
			activeEffect.IsStopping = true;
		}
	}

	if (!alreadyExists)
	{
		auto *registeredEffect = GetRegisteredEffect(entry.Identifier);

		if (registeredEffect)
		{
			std::ostringstream effectName;
			effectName << (effectData.HasCustomName() ? effectData.CustomName : effectData.Name);

			if (!entry.Suffix.empty())
			{
				effectName << " " << entry.Suffix;
			}

			playEffectDispatchSound();

			int effectDuration;
			switch (effectData.TimedType)
			{
			case EffectTimedType::Normal:
				effectDuration = effectData.IsMeta() ? effectDispatcher->SharedState.MetaEffectTimedDur
				                                     : effectDispatcher->SharedState.EffectTimedDur;
				break;
			case EffectTimedType::Short:
				effectDuration = effectData.IsMeta() ? effectDispatcher->SharedState.MetaEffectShortDur
				                                     : effectDispatcher->SharedState.EffectTimedShortDur;
				break;
			case EffectTimedType::Custom:
				effectDuration = effectData.CustomTime;
				break;
			default:
				effectDuration = -1;
				break;
			}

			effectDispatcher->SharedState.ActiveEffects.emplace_back(entry.Identifier, registeredEffect,
			                                                         effectName.str(), effectData, effectDuration);

			// There might be a reason to include meta effects in the future, for now we will just exclude them
			if (!(entry.Flags & EffectDispatcher::DispatchEffectFlag_NoAddToLog) && !effectData.IsMeta()
			    && !effectData.IsTemporary() && !effectData.IsUtility())
			{
				if (effectDispatcher->SharedState.DispatchedEffectsLog.size() >= 100)
				{
					effectDispatcher->SharedState.DispatchedEffectsLog.erase(
					    effectDispatcher->SharedState.DispatchedEffectsLog.begin());
				}

				effectDispatcher->SharedState.DispatchedEffectsLog.emplace_back(registeredEffect);
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
		auto currentUpdateTime = GetTickCount64();
		int deltaTime          = currentUpdateTime
		              - (ComponentExists<EffectDispatchTimer>() ? GetComponent<EffectDispatchTimer>()->GetTimer() : 0);

		// the game was paused
		if (deltaTime > 1000)
		{
			deltaTime = 0;
		}

		while (!effectDispatcher->EffectDispatchQueue.empty())
		{
			_DispatchEffect(effectDispatcher, effectDispatcher->EffectDispatchQueue.front());
			effectDispatcher->EffectDispatchQueue.pop();
		}

		effectDispatcher->UpdateEffects(deltaTime);

		if (!ComponentExists<EffectDispatchTimer>() || GetComponent<EffectDispatchTimer>()->IsTimerEnabled())
		{
			effectDispatcher->UpdateMetaEffects(deltaTime);
		}

		SwitchToFiber(g_MainThread);
	}
}

EffectDispatcher::EffectDispatcher(const std::array<BYTE, 3> &textColor, const std::array<BYTE, 3> &effectTimerColor)
    : Component()
{
	m_TextColor        = textColor;
	m_EffectTimerColor = effectTimerColor;

	m_DisableDrawEffectTexts =
	    g_OptionsManager.GetConfigValue({ "DisableEffectTextDraw" }, OPTION_DEFAULT_NO_TEXT_DRAW);

	SharedState.EffectTimedDur = g_OptionsManager.GetConfigValue({ "EffectTimedDur" }, OPTION_DEFAULT_EFFECT_TIMED_DUR);
	SharedState.EffectTimedShortDur =
	    g_OptionsManager.GetConfigValue({ "EffectTimedShortDur" }, OPTION_DEFAULT_EFFECT_SHORT_TIMED_DUR);

	SharedState.MetaEffectSpawnTime =
	    g_OptionsManager.GetConfigValue({ "NewMetaEffectSpawnTime" }, OPTION_DEFAULT_EFFECT_META_SPAWN_TIME);
	SharedState.MetaEffectTimedDur =
	    g_OptionsManager.GetConfigValue({ "MetaEffectDur" }, OPTION_DEFAULT_EFFECT_META_TIMED_DUR);
	SharedState.MetaEffectShortDur =
	    g_OptionsManager.GetConfigValue({ "MetaShortEffectDur" }, OPTION_DEFAULT_EFFECT_META_SHORT_TIMED_DUR);

	m_MaxRunningEffects =
	    g_OptionsManager.GetConfigValue({ "MaxParallelRunningEffects" }, OPTION_DEFAULT_MAX_RUNNING_PARALLEL_EFFECTS);

	Reset();

	for (const auto &[effectIdentifier, effectData] : g_EnabledEffects)
	{
		if (!effectData.IsMeta() && !effectData.IsUtility())
		{
			// There's at least 1 enabled non-permanent effect, enable timer
			m_EnableNormalEffectDispatch = true;
			break;
		}
	}

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
	if (g_EffectDispatcherThread)
	{
		SwitchToFiber(g_EffectDispatcherThread);
	}

	DrawEffectTexts();
}

void EffectDispatcher::UpdateEffects(int deltaTime)
{
	if (m_ClearEffectsState != ClearEffectsState::None)
	{
		SharedState.ActiveEffects.clear();
		m_PermanentEffects.clear();
		SharedState.DispatchedEffectsLog.clear();

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

	float adjustedDeltaTime = (float)deltaTime / 1000;

	int maxEffects =
	    std::min((int)(floor((1.0f - GetEffectTopSpace()) / EFFECT_TEXT_INNER_SPACING_MIN) - 1), m_MaxRunningEffects);
	int activeEffects              = 0;
	int effectCountToCheckCleaning = 3;
	// Reverse order to ensure the effects on top of the list are removed if activeEffects > maxEffects
	for (auto it = SharedState.ActiveEffects.rbegin(); it != SharedState.ActiveEffects.rend();)
	{
		auto &effect        = *it;

		bool isEffectPaused = EffectThreads::IsThreadPaused(effect.ThreadId);

		if (!EffectThreads::DoesThreadExist(effect.ThreadId))
		{
			if (effect.MaxTime > 0.f
			    || (effect.MaxTime < 0.f && effect.Timer >= 0.f /* Timer > 0 for non-timed effects = remove */))
			{
				// Effect thread doesn't exist anymore so just remove the effect from list
				it = static_cast<decltype(it)>(SharedState.ActiveEffects.erase(std::next(it).base()));
				continue;
			}
		}
		else if (!isEffectPaused)
		{
			OnPreRunEffect.Fire(effect.Identifier);
			EffectThreads::RunThread(effect.ThreadId);
			OnPostRunEffect.Fire(effect.Identifier);

			if (!effect.IsMeta)
			{
				activeEffects++;
			}
		}

		if (effect.HideEffectName && EffectThreads::HasThreadOnStartExecuted(effect.ThreadId))
		{
			effect.HideEffectName = false;
		}

		if (effect.MaxTime > 0.f)
		{
			if (isEffectPaused)
			{
				effect.Timer -= adjustedDeltaTime;
			}
			else
			{
				effect.Timer -=
				    adjustedDeltaTime
				    / (ComponentExists<MetaModifiers>() ? GetComponent<MetaModifiers>()->EffectDurationModifier : 1.f);
			}
		}
		else
		{
			float t = SharedState.EffectTimedDur, m = maxEffects, n = effectCountToCheckCleaning;
			// Ensure non-timed effects stay on screen for a certain amount of time
			effect.Timer += adjustedDeltaTime / t
			              * (1.f + (t / 5 - 1) * std::max(0.f, SharedState.ActiveEffects.size() - n) / (m - n));
		}

		if ((effect.MaxTime > 0.f && effect.Timer <= 0.f) || (!effect.IsMeta && activeEffects > maxEffects))
		{
			if (effect.Timer < -60.f)
			{
				// Effect took over 60 seconds to stop, forcibly stop it in a blocking manner
				EffectThreads::StopThreadImmediately(effect.ThreadId);
			}
			else if (!effect.IsStopping)
			{
				EffectThreads::StopThread(effect.ThreadId);
				effect.IsStopping = true;
			}
		}

		it++;
	}
}

void EffectDispatcher::UpdateMetaEffects(int deltaTime)
{
	if (!SharedState.MetaEffectsEnabled)
	{
		return;
	}

	SharedState.MetaEffectTimerPercentage += (float)deltaTime / SharedState.MetaEffectSpawnTime / 1000;

	if (SharedState.MetaEffectTimerPercentage >= 1.f)
	{
		SharedState.MetaEffectTimerPercentage = 0.f;

		std::vector<std::tuple<EffectIdentifier, EffectData *>> availableMetaEffects;

		float totalWeight = 0.f;
		for (auto &[effectId, effectData] : g_EnabledEffects)
		{
			if (effectData.IsMeta() && !effectData.IsUtility() && !effectData.IsHidden())
			{
				totalWeight += effectData.GetEffectWeight();

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
				totalWeight += effectData->GetEffectWeight();

				effectData->Weight += effectData->WeightMult;

				if (!targetEffectIdentifier && chosen <= totalWeight)
				{
					targetEffectIdentifier = &effectIdentifier;
				}
			}

			if (targetEffectIdentifier)
			{
				_DispatchEffect(this, { .Identifier = *targetEffectIdentifier,
				                        .Suffix     = "(Meta)",
				                        .Flags      = DispatchEffectFlag_NoAddToLog });
			}
		}
		else
		{
			SharedState.MetaEffectsEnabled        = false;
			SharedState.MetaEffectTimerPercentage = 0.f;
		}
	}
}

void EffectDispatcher::DrawEffectTexts()
{
	if (m_DisableDrawEffectTexts)
	{
		return;
	}

	float y             = GetEffectTopSpace();
	float effectSpacing = EFFECT_TEXT_INNER_SPACING_MAX;

	if (SharedState.ActiveEffects.size() > 0)
	{
		effectSpacing =
		    std::min(EFFECT_TEXT_INNER_SPACING_MAX,
		             std::max(EFFECT_TEXT_INNER_SPACING_MIN, (1.0f - y) / SharedState.ActiveEffects.size()));
	}

	for (const ActiveEffect &effect : SharedState.ActiveEffects)
	{
		if (effect.IsStopping)
		{
			continue;
		}

		const bool hasFake = !effect.FakeName.empty();

		// Temporary non-timed effects will have their entries removed already since their OnStop is called immediately
		if (g_EnabledEffects.contains(effect.Identifier))
		{
			auto &effectData = g_EnabledEffects.at(effect.Identifier);
			if ((effect.HideEffectName && !hasFake)
			    || ((ComponentExists<MetaModifiers>()
			         && (GetComponent<MetaModifiers>()->HideChaosUI || GetComponent<MetaModifiers>()->DisableChaos))
			        && !effectData.IsMeta() && !effectData.IsUtility() && !effectData.IsTemporary()))
			{
				continue;
			}
		}

		auto effectName = effect.Name;
		if (effect.HideEffectName && hasFake)
		{
			effectName = effect.FakeName;
		}

		if (ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->FlipChaosUI)
		{
			DrawScreenText(effectName, { .085f, y }, .47f, { m_TextColor[0], m_TextColor[1], m_TextColor[2] }, true,
			               ScreenTextAdjust::Left, { .0f, .915f });
		}
		else
		{
			DrawScreenText(effectName, { .915f, y }, .47f, { m_TextColor[0], m_TextColor[1], m_TextColor[2] }, true,
			               ScreenTextAdjust::Right, { .0f, .915f });
		}

		if (effect.MaxTime > 0.f)
		{
			if (ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->FlipChaosUI)
			{
				DRAW_RECT(.04f, y + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.04f, y + .0185f, .048f * (1.f - (effect.Timer / effect.MaxTime)), .017f,
				          m_EffectTimerColor[0], m_EffectTimerColor[1], m_EffectTimerColor[2], 255, false);
			}
			else
			{
				DRAW_RECT(.96f, y + .0185f, .05f, .019f, 0, 0, 0, 127, false);
				DRAW_RECT(.96f, y + .0185f, .048f * effect.Timer / effect.MaxTime, .017f, m_EffectTimerColor[0],
				          m_EffectTimerColor[1], m_EffectTimerColor[2], 255, false);
			}
		}

		y += effectSpacing;
	}
}

void EffectDispatcher::DispatchEffect(const EffectIdentifier &effectIdentifier, DispatchEffectFlags dispatchEffectFlags,
                                      const std::string &suffix)
{
	EffectDispatchQueue.push({ .Identifier = effectIdentifier, .Suffix = suffix, .Flags = dispatchEffectFlags });
}

void EffectDispatcher::DispatchRandomEffect(DispatchEffectFlags dispatchEffectFlags, const std::string &suffix)
{
	if (!m_EnableNormalEffectDispatch)
	{
		return;
	}

	std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
	for (const auto &[effectIdentifier, effectData] : g_EnabledEffects)
	{
		if (!effectData.IsMeta() && !effectData.IsUtility() && !effectData.IsHidden())
		{
			choosableEffects.emplace(effectIdentifier, effectData);
		}
	}

	float totalWeight = 0.f;
	for (const auto &[effectIdentifier, effectData] : choosableEffects)
	{
		totalWeight += effectData.GetEffectWeight();
	}

	float chosen                                   = g_Random.GetRandomFloat(0.f, totalWeight);

	totalWeight                                    = 0.f;

	const EffectIdentifier *targetEffectIdentifier = nullptr;
	for (const auto &[effectIdentifier, effectData] : choosableEffects)
	{
		totalWeight += effectData.GetEffectWeight();

		if (chosen <= totalWeight)
		{
			targetEffectIdentifier = &effectIdentifier;

			break;
		}
	}

	if (targetEffectIdentifier)
	{
		DispatchEffect(*targetEffectIdentifier, dispatchEffectFlags, suffix);
	}
}

void EffectDispatcher::ClearEffect(const EffectIdentifier &effectId)
{
	auto result = std::find_if(SharedState.ActiveEffects.begin(), SharedState.ActiveEffects.end(),
	                           [effectId](auto &activeEffect) { return activeEffect.Identifier == effectId; });
	if (result == SharedState.ActiveEffects.end())
	{
		return;
	}

	EffectThreads::StopThread(result->ThreadId);
	result->IsStopping = true;
}

void EffectDispatcher::ClearEffects(ClearEffectsFlags clearEffectFlags)
{
	m_ClearEffectsState = clearEffectFlags & ClearEffectsFlag_NoRestartPermanentEffects
	                        ? ClearEffectsState::All
	                        : ClearEffectsState::AllRestartPermanent;
}

void EffectDispatcher::ClearActiveEffects()
{
	for (auto it = SharedState.ActiveEffects.begin(); it != SharedState.ActiveEffects.end(); it++)
	{
		auto &effect = *it;

		if (effect.IsMeta || effect.Timer <= 0.f)
		{
			continue;
		}

		EffectThreads::StopThread(effect.ThreadId);
		effect.IsStopping = true;
	}
}

void EffectDispatcher::ClearMostRecentEffect()
{
	for (auto it = SharedState.ActiveEffects.rbegin(); it != SharedState.ActiveEffects.rend(); it++)
	{
		auto &effect = *it;

		if (effect.IsMeta || effect.Timer <= 0.f)
		{
			continue;
		}

		EffectThreads::StopThread(effect.ThreadId);
		effect.IsStopping = true;

		break;
	}
}

std::vector<RegisteredEffect *> EffectDispatcher::GetRecentEffects(int distance, std::string_view ignoreEffect) const
{
	std::vector<RegisteredEffect *> effects;

	for (int i = SharedState.DispatchedEffectsLog.size() - 1; distance > 0 && i >= 0; i--)
	{
		auto effect = *std::next(SharedState.DispatchedEffectsLog.begin(), i);
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

void EffectDispatcher::Reset(ClearEffectsFlags clearEffectFlags)
{
	ClearEffects(clearEffectFlags);

	SharedState.MetaEffectsEnabled        = true;
	SharedState.MetaEffectTimerPercentage = 0.f;
}

float EffectDispatcher::GetEffectTopSpace()
{
	return EnableEffectTextExtraTopSpace ? EFFECT_TEXT_TOP_SPACING_EXTRA : EFFECT_TEXT_TOP_SPACING;
}

void EffectDispatcher::RegisterPermanentEffects()
{
	auto registerEffect = [&](EffectIdentifier effectIdentifier)
	{
		auto *registeredEffect = GetRegisteredEffect(effectIdentifier);
		if (registeredEffect)
		{
			auto threadId = EffectThreads::CreateThread(registeredEffect, true);
			m_PermanentEffects.push_back(threadId);
		}
	};

	if (g_OptionsManager.GetConfigValue({ "Australia" }, false))
	{
		registerEffect({ "player_flip_camera" });
	}

	for (const auto &[effectIdentifier, effectData] : g_EnabledEffects)
	{
		if (effectData.TimedType == EffectTimedType::Permanent)
		{
			// Always run permanent timed effects in background
			registerEffect(effectIdentifier);
		}
	}
}

// (kolyaventuri): Forces the name of the provided effect to change, using any given string
void EffectDispatcher::OverrideEffectName(std::string_view effectId, const std::string &overrideName)
{
	for (auto &effect : SharedState.ActiveEffects)
	{
		if (effect.Identifier.GetEffectId() == effectId)
		{
			effect.FakeName = overrideName;
		}
	}
}

// (kolyaventuri): Forces the name of the provided effect to change, using the defined name of another effect
void EffectDispatcher::OverrideEffectNameId(std::string_view effectId, std::string_view fakeEffectId)
{
	for (auto &effect : SharedState.ActiveEffects)
	{
		if (effect.Identifier.GetEffectId() == effectId)
		{
			auto effectIdentifier = EffectIdentifier(std::string(fakeEffectId));

			if (g_EnabledEffects.contains(effectIdentifier))
			{
				auto &fakeEffect = g_EnabledEffects.at(effectIdentifier);
				effect.FakeName  = fakeEffect.HasCustomName() ? fakeEffect.CustomName : fakeEffect.Name;
			}
			else
			{
				auto result = g_EffectsMap.find(fakeEffectId);
				if (result != g_EffectsMap.end())
				{
					effect.FakeName = result->second.Name;
				}
			}
		}
	}
}

bool EffectDispatcher::IsClearingEffects() const
{
	return m_ClearEffectsState != ClearEffectsState::None;
}