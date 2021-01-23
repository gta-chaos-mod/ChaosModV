#include "stdafx.h"

EffectDispatcher::EffectDispatcher(int effectSpawnTime, int effectTimedDur, int effectTimedShortDur, int metaEffectSpawnTime, int metaEffectTimedDur, int metaEffectShortDur,
	std::array<int, 3> timerColor, std::array<int, 3> textColor, std::array<int, 3> effectTimerColor, bool enableTwitchVoting,
	TwitchOverlayMode twitchOverlayMode)
	: m_percentage(.0f), m_effectSpawnTime(effectSpawnTime), m_effectTimedDur(effectTimedDur), m_effectTimedShortDur(effectTimedShortDur), m_metaEffectSpawnTime(metaEffectSpawnTime), 
	m_metaEffectTimedDur(metaEffectTimedDur), m_metaEffectShortDur(metaEffectShortDur), m_timerColor(timerColor), m_textColor(textColor), m_effectTimerColor(effectTimerColor),
	m_enableTwitchVoting(enableTwitchVoting), m_twitchOverlayMode(twitchOverlayMode)
{
	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	ClearEffects();
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_enableNormalEffectDispatch || g_metaInfo.ShouldHideChaosUI)
	{
		return;
	}

	// New Effect Bar
	DRAW_RECT(.5f, .0f, 1.f, .05f, 0, 0, 0, 127, false);
	DRAW_RECT(m_percentage * .5f, .0f, m_percentage, .05f, m_timerColor[0], m_timerColor[1], m_timerColor[2], 255, false);
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
		if (effect.HideText || (g_metaInfo.ShouldHideChaosUI && effect.EffectIdentifier.GetEffectType() != EFFECT_META_HIDE_CHAOS_UI))
		{
			continue;
		}

		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(effect.Name.c_str());
		SET_TEXT_SCALE(.5f, .5f);
		SET_TEXT_COLOUR(m_textColor[0], m_textColor[1], m_textColor[2], 255);
		SET_TEXT_OUTLINE();
		SET_TEXT_WRAP(.0f, .91f);
		SET_TEXT_RIGHT_JUSTIFY(true);
		END_TEXT_COMMAND_DISPLAY_TEXT(.91f, y, 0);

		if (effect.Timer > 0)
		{
			DRAW_RECT(.95f, y + .02f, .05f, .02f, 0, 0, 0, 127, false);
			DRAW_RECT(.95f, y + .02f, .05f * effect.Timer / effect.MaxTime, .02f, m_effectTimerColor[0], m_effectTimerColor[1],
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
				|| effect.Timer < -m_effectTimedDur + (activeEffectsSize > 3 ? ((activeEffectsSize - 3) * 20 < 160 ? (activeEffectsSize - 3) * 20 : 160) : 0))
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

		m_metaEffectTimer -= 1;
		if (m_metaEffectTimer <= 0)
		{
			m_metaEffectTimer = m_metaEffectSpawnTime;
			std::vector<EffectIdentifier> availableMetaEffects;
			for (const auto& pair : g_enabledEffects)
			{
				if (pair.second.IsMeta && pair.second.TimedType != EffectTimedType::TIMED_PERMANENT)
				{
					availableMetaEffects.push_back(pair.first);
				}
			}
			if (!availableMetaEffects.empty()) 
			{
				const EffectIdentifier& randomMetaEffect = availableMetaEffects[g_random.GetRandomInt(0, availableMetaEffects.size() - 1)];
				DispatchEffect(randomMetaEffect, "(Meta)");
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
		pair.second.Weight += pair.second.WeightMult;
	}

	// Reset weight of this effect to reduce / stop chance of same effect happening multiple times in a row
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

			// Play global sound (if existing)
			Mp3Manager::PlayChaosSoundFile("global_effectdispatch");

			// Play a sound if corresponding .mp3 file exists
			Mp3Manager::PlayChaosSoundFile(effectData.Id);

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