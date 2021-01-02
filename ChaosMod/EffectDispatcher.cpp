#include "stdafx.h"

EffectDispatcher::EffectDispatcher(int effectSpawnTime, int effectTimedDur, int effectTimedShortDur, bool disableTwiceInRow, int metaEffectSpawnTime,
	std::array<int, 3> timerColor, std::array<int, 3> textColor, std::array<int, 3> effectTimerColor, bool enableTwitchVoting,
	TwitchOverlayMode twitchOverlayMode)
	: m_percentage(.0f), m_effectSpawnTime(effectSpawnTime), m_effectTimedDur(effectTimedDur), m_effectTimedShortDur(effectTimedShortDur), m_disableTwiceInRow(disableTwiceInRow), m_metaEffectSpawnTime(metaEffectSpawnTime),
	m_timerColor(timerColor), m_textColor(textColor), m_effectTimerColor(effectTimerColor), m_enableTwitchVoting(enableTwitchVoting), m_twitchOverlayMode(twitchOverlayMode)
{
	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	ClearEffects();
}

void EffectDispatcher::DrawTimerBar()
{
	if (!m_enableNormalEffectDispatch || g_metaInfo.shouldHideChaosUI)
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
		if (effect.HideText || (g_metaInfo.shouldHideChaosUI && effect.EffectType != EFFECT_META_HIDE_CHAOS_UI))
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

	if ((m_percentage = (delta + (m_timerTimerRuns * 1000)) / (m_effectSpawnTime / g_metaInfo.timerSpeedModifier * 1000)) > 1.f && m_dispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		if (g_metaInfo.additionalEffectsToDispatch > 0)
		{
			for (int i = 0; i < g_metaInfo.additionalEffectsToDispatch; i++)
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

		UpdateMetaEffects();

		int activeEffectsSize = m_activeEffects.size();
		std::vector<ActiveEffect>::iterator it;
		for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
		{
			ActiveEffect& effect = *it;
			EffectData& effectData = g_enabledEffects.at(effect.EffectType);
			if (effectData.isMeta)
			{
				effect.Timer--;
			} 
			else
			{
				effect.Timer -= 1 / g_metaInfo.effectDurationModifier;
			}

			if ((effect.MaxTime > 0 && effect.Timer <= 0)
				|| effect.Timer < -m_effectTimedDur + (activeEffectsSize > 3 ? ((activeEffectsSize - 3) * 20 < 160 ? (activeEffectsSize - 3) * 20 : 160) : 0))
			{
				ThreadManager::StopThread(effect.ThreadId);

				//effect.RegisteredEffect->Stop();
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
	m_metaEffectTimer -= 1;
	if (m_metaEffectTimer <= 0)
	{
		m_metaEffectTimer = m_metaEffectSpawnTime;
		std::vector<EffectType> availableMetaEffects;
		for (const auto& pair : g_enabledEffects)
		{
			if (pair.second.isMeta)
			{
				availableMetaEffects.push_back(pair.first);
			}
		}
		if (!availableMetaEffects.empty()) 
		{
			EffectType randomMetaEffect = availableMetaEffects[g_random.GetRandomInt(0, availableMetaEffects.size() - 1)];
			DispatchEffect(randomMetaEffect, " (Meta)");
		}
		else
		{
			// maybe add a flag instead 
			m_metaEffectTimer = INT_MAX;
		}

	}
}

void EffectDispatcher::DispatchEffect(EffectType effectType, const char* suffix)
{
	// Increase weight for all effects first
	for (auto& pair : g_enabledEffects)
	{
		pair.second.Weight += pair.second.WeightMult;
	}

	const EffectInfo& effectInfo = g_effectsMap.at(effectType);
	EffectData& effectData = g_enabledEffects.at(effectType);

	// Reset weight of this effect to reduce / stop chance of same effect happening multiple times in a row
	effectData.Weight = m_disableTwiceInRow ? 0 : effectData.WeightMult;

#ifdef _DEBUG
	// Write weight distribution to file
	std::ofstream weightLog("chaosmod/effectweights.txt");

	for (const auto& pair : g_enabledEffects)
	{
		const EffectData& effectData = pair.second;

		weightLog << effectData.Name << " " << effectData.Weight << " (" << effectData.WeightMult << ")" << std::endl;
	}
#endif

	float effectTime = effectInfo.IsTimed
		? effectData.CustomTime >= 0
			? effectData.CustomTime
			: effectData.TimedType == EffectTimedType::TIMED_SHORT
				? m_effectTimedShortDur
				: m_effectTimedDur
		: -1;

	static std::ofstream log("chaosmod/effectslog.txt");
	log << effectInfo.Name << std::endl;

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool alreadyExists = false;

	const std::vector<EffectType>& incompatibleEffects = effectInfo.IncompatibleWith;

	std::vector<ActiveEffect>::iterator it;
	for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
	{
		ActiveEffect& effect = *it;

		if (effectInfo.IsTimed && effect.EffectType == effectType)
		{
			alreadyExists = true;
			effect.Timer = effect.MaxTime;
		}

		bool found = false;
		if (std::find(incompatibleEffects.begin(), incompatibleEffects.end(), effect.EffectType) != incompatibleEffects.end())
		{
			found = true;
		}

		// Check if current effect is marked as incompatible in active effect
		if (!found)
		{
			const std::vector<EffectType>& activeIncompatibleEffects = g_effectsMap.at(effect.EffectType).IncompatibleWith;

			if (std::find(activeIncompatibleEffects.begin(), activeIncompatibleEffects.end(), effectType) != activeIncompatibleEffects.end())
			{
				found = true;
			}
		}

		if (found)
		{
			ThreadManager::StopThread(effect.ThreadId);

			//effect.RegisteredEffect->Stop();
			it = m_activeEffects.erase(it);
		}
		else
		{
			it++;
		}
	}

	if (!alreadyExists)
	{
		RegisteredEffect* registeredEffect = GetRegisteredEffect(effectType);

		if (registeredEffect)
		{
			//registeredEffect->Start();

			std::ostringstream ossEffectName;
			ossEffectName << effectData.Name;

			if (suffix && strlen(suffix) > 0)
			{
				ossEffectName << " " << suffix;
			}

			ossEffectName << std::endl;

			// Play a sound if corresponding .mp3 file exists
			Mp3Manager::PlayChaosSoundFile(effectInfo.Id);

			m_activeEffects.emplace_back(effectType, registeredEffect, ossEffectName.str(), effectTime);
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

	std::map<EffectType, EffectData> choosableEffects;
	for (const auto& pair : g_enabledEffects)
	{
		EffectType effectType = pair.first;
		const EffectData& effectData = pair.second;
		
		if (!effectData.Permanent && !effectData.isMeta && (!m_disableTwiceInRow || effectType != m_lastEffect))
		{
			choosableEffects.emplace(effectType, effectData);
		}
	}

	int effectsTotalWeight = 0;
	for (const auto& pair : choosableEffects)
	{
		effectsTotalWeight += pair.second.Weight;
	}

	int index = g_random.GetRandomInt(0, effectsTotalWeight);

	int addedUpWeight = 0;
	auto targetEffectType = _EFFECT_ENUM_MAX;
	for (const auto& pair : choosableEffects)
	{
		if (pair.second.Permanent)
		{
			continue;
		}

		addedUpWeight += pair.second.Weight;

		if (index <= addedUpWeight)
		{
			targetEffectType = pair.first;
			break;
		}
	}

	if (targetEffectType != _EFFECT_ENUM_MAX)
	{
		if (m_disableTwiceInRow)
		{
			m_lastEffect = targetEffectType;
		}

		DispatchEffect(targetEffectType, suffix);
	}
}

void EffectDispatcher::ClearEffects()
{
	ThreadManager::StopThreads();

	/*for (RegisteredEffect* effect : m_permanentEffects)
	{
		effect->Stop();
	}*/
	m_permanentEffects.clear();

	/*for (ActiveEffect& effect : m_activeEffects)
	{
		effect.RegisteredEffect->Stop();
	}*/
	m_activeEffects.clear();
}

void EffectDispatcher::Reset()
{
	ClearEffects();
	ResetTimer();

	m_enableNormalEffectDispatch = false;

	for (const auto& pair : g_enabledEffects)
	{
		if (pair.second.Permanent)
		{
			// Always run permanent timed effects in background
			RegisteredEffect* registeredEffect = GetRegisteredEffect(pair.first);

			if (registeredEffect)
			{
				//registeredEffect->Start();
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