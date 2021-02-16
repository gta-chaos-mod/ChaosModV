#include "stdafx.h"

EffectDispatcher::EffectDispatcher(const std::array<int, 3>& timerColor, const std::array<int, 3>& textColor, const std::array<int, 3>& effectTimerColor)
	: m_timerColor(timerColor), m_textColor(textColor), m_effectTimerColor(effectTimerColor)
{
	m_effectSpawnTime = g_optionsManager.GetConfigValue<int>("NewEffectSpawnTime", OPTION_DEFAULT_EFFECT_SPAWN_TIME);
	m_effectTimedDur = g_optionsManager.GetConfigValue<int>("EffectTimedDur", OPTION_DEFAULT_EFFECT_TIMED_DUR);
	m_effectTimedShortDur = g_optionsManager.GetConfigValue<int>("EffectTimedShortDur", OPTION_DEFAULT_EFFECT_SHORT_TIMED_DUR);

	m_metaEffectSpawnTime = g_optionsManager.GetConfigValue<int>("NewMetaEffectSpawnTime", OPTION_DEFAULT_EFFECT_META_SPAWN_TIME);
	m_metaEffectTimedDur = g_optionsManager.GetConfigValue<int>("MetaEffectDur", OPTION_DEFAULT_EFFECT_META_TIMED_DUR);
	m_metaEffectShortDur = g_optionsManager.GetConfigValue<int>("MetaShortEffectDur", OPTION_DEFAULT_EFFECT_META_SHORT_TIMED_DUR);

	m_enableTwitchVoting = g_optionsManager.GetTwitchValue<bool>("EnableTwitchVoting", OPTION_DEFAULT_TWITCH_VOTING_ENABLED);

	m_twitchOverlayMode = static_cast<TwitchOverlayMode>(g_optionsManager.GetTwitchValue<int>("TwitchVotingOverlayMode", OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	ClearEffects();
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_enableNormalEffectDispatch)
	{
		return;
	}

	// New Effect Bar
	DRAW_RECT(.5f, .01f, 1.f, .021f, 0, 0, 0, 127, false);
	DRAW_RECT(m_percentage * .5f, .01f, m_percentage, .018f, m_timerColor[0], m_timerColor[1], m_timerColor[2], 255, false);
}

void EffectDispatcher::DrawEffectTexts()
{
	if (!m_enableNormalEffectDispatch)
	{
		return;
	}

	// Effect Texts
	float y = .2f;
	if (m_enableTwitchVoting && (m_twitchOverlayMode == TwitchOverlayMode::OVERLAY_INGAME || m_twitchOverlayMode == TwitchOverlayMode::OVERLAY_OBS))
	{
		y = .35f;
	}

	for (const ActiveEffect& effect : m_activeEffects)
	{
		if (effect.HideText || (g_metaInfo.ShouldHideChaosUI && effect.EffectIdentifier.GetEffectType() != EFFECT_META_HIDE_CHAOS_UI)
			|| (g_metaInfo.DisableChaos && effect.EffectIdentifier.GetEffectType() != EFFECT_META_NO_CHAOS))
		{
			continue;
		}

		DrawScreenText(effect.Name, { .915f, y }, .47f, { m_textColor[0], m_textColor[1], m_textColor[2] }, true,
			ScreenTextAdjust::RIGHT, { .0f, .915f });

		if (effect.Timer > 0)
		{
			DRAW_RECT(.96f, y + .0185f, .05f, .019f, 0, 0, 0, 127, false);
			DRAW_RECT(.96f, y + .0185f, .048f * effect.Timer / effect.MaxTime, .017f, m_effectTimerColor[0], m_effectTimerColor[1],
				m_effectTimerColor[2], 255, false);
		}

		y += .075f;
	}
}

void EffectDispatcher::UpdateTimer()
{
	if (!m_enableNormalEffectDispatch)
	{
		return;
	}

	DWORD64 currentUpdateTime = GetTickCount64();

	float delta = currentUpdateTime - m_timerTimer;

	if (delta > 1000.f)
	{
		m_timerTimerRuns++;

		m_timerTimer = currentUpdateTime;
		delta = 0;
	}

	if ((m_percentage = (delta + (m_timerTimerRuns * 1000)) / (m_effectSpawnTime / g_metaInfo.TimerSpeedModifier * 1000)) > 1.f && m_dispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		if (g_metaInfo.AdditionalEffectsToDispatch > 0)
		{
			for (int i = 0; i < g_metaInfo.AdditionalEffectsToDispatch; i++)
			{
				g_effectDispatcher->DispatchRandomEffect();
			}
		}

		m_timerTimerRuns = 0;
	}
}

void EffectDispatcher::OverrideTimerDontDispatch(bool state)
{
	m_dispatchEffectsOnTimer = !state;
}

void EffectDispatcher::UpdateEffects()
{
	ThreadManager::RunThreads();

	// Don't continue if there are no enabled effects
	if (g_enabledEffects.empty())
	{
		return;
	}

	for (ActiveEffect& effect : m_activeEffects)
	{
		if (effect.HideText && ThreadManager::HasThreadOnStartExecuted(effect.ThreadId))
		{
			effect.HideText = false;
		}
	}

	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - m_effectsTimer) > 1000)
	{
		m_effectsTimer = currentUpdateTime;

		int activeEffectsSize = m_activeEffects.size();
		std::vector<ActiveEffect>::iterator it;
		for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
		{
			ActiveEffect& effect = *it;
			EffectData& effectData = g_enabledEffects.at(effect.EffectIdentifier);
			if (effectData.IsMeta)
			{
				effect.Timer--;
			} 
			else
			{
				effect.Timer -= 1 / g_metaInfo.EffectDurationModifier;
			}

			if ((effect.MaxTime > 0 && effect.Timer <= 0)
				|| ((!effectData.IsMeta)
					&& (effect.Timer < -m_effectTimedDur + (activeEffectsSize > 3 ? ((activeEffectsSize - 3) * 20 < 160 ? (activeEffectsSize - 3) * 20 : 160) : 0))))
			{
				ThreadManager::StopThread(effect.ThreadId);

				it = m_activeEffects.erase(it);
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
	if (m_metaEffectsEnabled)
	{
		DWORD64 currentUpdateTime = GetTickCount64();
		if (currentUpdateTime - m_metaTimer < 1000)
		{
			return;
		}

		m_metaTimer = currentUpdateTime;

		if (--m_metaEffectTimer <= 0)
		{
			m_metaEffectTimer = m_metaEffectSpawnTime;

			std::vector<std::tuple<EffectIdentifier, EffectData*>> availableMetaEffects;

			int totalWeight = 0;
			for (auto& pair : g_enabledEffects)
			{
				if (pair.second.IsMeta && pair.second.TimedType != EffectTimedType::TIMED_PERMANENT)
				{
					totalWeight += pair.second.Weight;

					availableMetaEffects.push_back(std::make_tuple(pair.first, &pair.second));
				}
			}

			if (!availableMetaEffects.empty()) 
			{
				// TODO: Stop duplicating effect weight logic everywhere
				int index = g_random.GetRandomInt(0, totalWeight);

				totalWeight = 0;

				const EffectIdentifier* effectIdentifier = nullptr;
				for (auto& pair : availableMetaEffects)
				{
					auto& [effect, effectData] = pair;

					totalWeight += effectData->Weight;

					effectData->Weight += effectData->WeightMult;

					if (!effectIdentifier && index <= totalWeight)
					{
						effectIdentifier = &effect;
					}
				}

				if (effectIdentifier)
				{
					DispatchEffect(*effectIdentifier, "(Meta)");
				}
			}
			else
			{
				m_metaEffectsEnabled = false;
				m_metaEffectTimer = INT_MAX;
			}
		}
	}
}

void EffectDispatcher::DispatchEffect(const EffectIdentifier& effectIdentifier, const char* suffix)
{
	EffectData& effectData = g_enabledEffects.at(effectIdentifier);
	if (effectData.TimedType == EffectTimedType::TIMED_PERMANENT)
	{
		return;
	}

	// Increase weight for all effects first
	for (auto& pair : g_enabledEffects)
	{
		if (!pair.second.IsMeta)
		{
			pair.second.Weight += pair.second.WeightMult;
		}
	}

	// Reset weight of this effect to reduce chance of same effect happening multiple times in a row
	effectData.Weight = effectData.WeightMult;

	LOG("Dispatched effect \"" << effectData.Name << "\"");

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool alreadyExists = false;

	const std::vector<std::string>& incompatibleIds = effectData.IncompatibleIds;

	std::vector<ActiveEffect>::iterator it;
	for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
	{
		ActiveEffect& activeEffect = *it;

		if (activeEffect.EffectIdentifier == effectIdentifier && effectData.TimedType != EffectTimedType::TIMED_UNK && effectData.TimedType != EffectTimedType::TIMED_NOTTIMED)
		{
			alreadyExists = true;
			activeEffect.Timer = activeEffect.MaxTime;

			break;
		}

		bool found = false;
		if (std::find(incompatibleIds.begin(), incompatibleIds.end(), g_enabledEffects.at(activeEffect.EffectIdentifier).Id) != incompatibleIds.end())
		{
			found = true;
		}

		// Check if current effect is marked as incompatible in active effect
		if (!found)
		{
			const std::vector<std::string>& activeIncompatibleIds = g_enabledEffects.at(activeEffect.EffectIdentifier).IncompatibleIds;

			if (std::find(activeIncompatibleIds.begin(), activeIncompatibleIds.end(), effectData.Id) != activeIncompatibleIds.end())
			{
				found = true;
			}
		}

		if (found)
		{
			ThreadManager::StopThread(activeEffect.ThreadId);

			it = m_activeEffects.erase(it);
		}
		else
		{
			it++;
		}
	}

	if (!alreadyExists)
	{
		RegisteredEffect* registeredEffect = GetRegisteredEffect(effectIdentifier);

		if (registeredEffect)
		{
			std::ostringstream ossEffectName;
			ossEffectName << (effectData.HasCustomName ? effectData.CustomName : effectData.Name);

			if (suffix && strlen(suffix) > 0)
			{
				ossEffectName << " " << suffix;
			}

			ossEffectName << std::endl;

			if (!g_metaInfo.ShouldHideChaosUI)
			{
				// Play global sound (if one exists)
				// Workaround: Force no global sound for "Fake Crash" and "Fake Death"
				if (effectIdentifier.GetEffectType() != EFFECT_MISC_CRASH && effectIdentifier.GetEffectType() != EFFECT_PLAYER_FAKEDEATH)
				{
					Mp3Manager::PlayChaosSoundFile("global_effectdispatch");
				}

				// Play a sound if corresponding .mp3 file exists
				Mp3Manager::PlayChaosSoundFile(effectData.Id);
			}

			int effectTime = -1;
			switch (effectData.TimedType)
			{
			case EffectTimedType::TIMED_NORMAL:
				effectTime = effectData.IsMeta ? m_metaEffectTimedDur : m_effectTimedDur;
				break;
			case EffectTimedType::TIMED_SHORT:
				effectTime = effectData.IsMeta ? m_metaEffectShortDur : m_effectTimedShortDur;
				break;
			case EffectTimedType::TIMED_CUSTOM:
				effectTime = effectData.CustomTime;
				break;
			}

			m_activeEffects.emplace_back(effectIdentifier, registeredEffect, ossEffectName.str(), effectTime);
		}
	}

	m_percentage = .0f;
}

void EffectDispatcher::DispatchRandomEffect(const char* suffix)
{
	if (!m_enableNormalEffectDispatch)
	{
		return;
	}

	std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
	for (const auto& pair : g_enabledEffects)
	{
		const EffectIdentifier& effectIdentifier = pair.first;
		const EffectData& effectData = pair.second;

		if (effectData.TimedType != EffectTimedType::TIMED_PERMANENT && !effectData.IsMeta)
		{
			choosableEffects.emplace(effectIdentifier, effectData);
		}
	}

	int effectsTotalWeight = 0;
	for (const auto& pair : choosableEffects)
	{
		effectsTotalWeight += pair.second.Weight;
	}

	int index = g_random.GetRandomInt(0, effectsTotalWeight);

	int addedUpWeight = 0;
	const EffectIdentifier* targetEffectIdentifier = nullptr;
	for (const auto& pair : choosableEffects)
	{
		if (pair.second.TimedType == EffectTimedType::TIMED_PERMANENT)
		{
			continue;
		}

		addedUpWeight += pair.second.Weight;

		if (index <= addedUpWeight)
		{
			targetEffectIdentifier = &pair.first;
			break;
		}
	}

	if (targetEffectIdentifier)
	{
		DispatchEffect(*targetEffectIdentifier, suffix);
	}
}

void EffectDispatcher::ClearEffects()
{
	ThreadManager::StopThreads();

	m_permanentEffects.clear();

	m_activeEffects.clear();
}

void EffectDispatcher::ClearActiveEffects(EffectIdentifier exclude)
{
	for (std::vector<ActiveEffect>::iterator it = m_activeEffects.begin(); it != m_activeEffects.end(); )
	{
		const ActiveEffect& effect = *it;

		if (effect.EffectIdentifier != exclude)
		{
			ThreadManager::StopThread(effect.ThreadId);

			it = m_activeEffects.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void EffectDispatcher::Reset()
{
	ClearEffects();
	ResetTimer();

	m_enableNormalEffectDispatch = false;
	m_metaEffectsEnabled = true;
	m_metaEffectTimer = m_metaEffectSpawnTime;
	m_metaTimer = GetTickCount64();

	for (const auto& pair : g_enabledEffects)
	{
		if (pair.second.TimedType == EffectTimedType::TIMED_PERMANENT)
		{
			// Always run permanent timed effects in background
			RegisteredEffect* registeredEffect = GetRegisteredEffect(pair.first);

			if (registeredEffect)
			{
				m_permanentEffects.push_back(registeredEffect);

				ThreadManager::CreateThread(registeredEffect, true);
			}
		}
		else
		{
			// There's at least 1 enabled non-permanent effect, enable timer
			m_enableNormalEffectDispatch = true;
		}
	}
}

void EffectDispatcher::ResetTimer()
{
	m_timerTimer = GetTickCount64();
	m_timerTimerRuns = 0;
	m_effectsTimer = GetTickCount64();
}