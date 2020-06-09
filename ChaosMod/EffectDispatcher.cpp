#include "stdafx.h"

EffectDispatcher::EffectDispatcher(int effectSpawnTime, int effectTimedDur, std::map<EffectType, EffectData> enabledEffects,
	int effectTimedShortDur, bool disableTwiceInRow, std::array<int, 3> timerColor, std::array<int, 3> textColor, std::array<int, 3> effectTimerColor,
	bool enableTwitchVoteablesOnscreen)
	: m_percentage(.0f), m_effectSpawnTime(effectSpawnTime), m_effectTimedDur(effectTimedDur),
		m_enabledEffects(enabledEffects), m_effectTimedShortDur(effectTimedShortDur), m_disableTwiceInRow(disableTwiceInRow),
		m_timerColor(timerColor), m_textColor(textColor), m_effectTimerColor(effectTimerColor), m_enableTwitchVoteablesOnscreen(enableTwitchVoteablesOnscreen)
{
	
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
	float y = m_enableTwitchVoteablesOnscreen ? .3f : .2f;
	for (ActiveEffect effect : m_activeEffects)
	{
		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(effect.Name);
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

	float thing = currentUpdateTime - m_timerTimer;

	if (thing > 1000.f)
	{
		m_timerTimerRuns++;

		m_timerTimer = currentUpdateTime;
		thing = 0;
	}

	if ((m_percentage = (thing + (m_timerTimerRuns * 1000)) / (m_effectSpawnTime * 1000)) > 1.f && m_dispatchEffectsOnTimer)
	{
		DispatchRandomEffect();

		m_timerTimerRuns = 0;
	}
}

void EffectDispatcher::OverrideTimerDontDispatch(bool state)
{
	m_dispatchEffectsOnTimer = !state;
}

void EffectDispatcher::UpdateEffects()
{
	if (m_enabledEffects.empty())
	{
		return;
	}

	for (auto activeEffect : m_activeEffects)
	{
		activeEffect.RegisteredEffect->Tick();
	}

	auto currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - m_effectsTimer) > 1000)
	{
		m_effectsTimer = currentUpdateTime;

		int activeEffectsSize = m_activeEffects.size();
		std::vector<ActiveEffect>::iterator it;
		for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
		{
			auto& effect = *it;

			effect.Timer--;

			if (effect.Timer == 0
				|| effect.Timer < -m_effectTimedDur + (activeEffectsSize > 3 ? ((activeEffectsSize - 3) * 20 < 160 ? (activeEffectsSize - 3) * 20 : 160) : 0))
			{
				effect.RegisteredEffect->Stop();
				it = m_activeEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

void EffectDispatcher::DispatchEffect(EffectType effectType, const char* suffix)
{
	auto effectInfo = g_effectsMap.at(effectType);
	auto effectData = m_enabledEffects.at(effectType);

	int effectTime = effectInfo.IsTimed
		? effectData.EffectCustomTime >= 0
			? effectData.EffectCustomTime
			: effectData.EffectTimedType == EffectTimedType::TIMED_SHORT
				? m_effectTimedShortDur
				: m_effectTimedDur
		: -1;

	static std::ofstream log("chaosmod/effectslog.txt");
	log << effectInfo.Name << std::endl;

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool alreadyExists = false;
	if (effectInfo.IsTimed)
	{
		const std::vector<EffectType> incompatibleEffects = effectInfo.IncompatibleWith;

		std::vector<ActiveEffect>::iterator it;
		for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
		{
			auto& effect = *it;

			if (effect.EffectType == effectType)
			{
				alreadyExists = true;
				effect.Timer = effect.MaxTime;
			}

			bool found = false;
			for (auto incompatibleEffect : incompatibleEffects)
			{
				if (effect.EffectType == incompatibleEffect)
				{
					found = true;

					break;
				}
			}

			// Check if current effect is marked as incompatible in active effect
			if (!found)
			{
				for (auto incompatibleEffect : g_effectsMap.at(effect.EffectType).IncompatibleWith)
				{
					if (effect.EffectType == effectType)
					{
						found = true;

						break;
					}
				}
			}

			if (found)
			{
				effect.RegisteredEffect->Stop();
				it = m_activeEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	if (!alreadyExists)
	{
		RegisteredEffect* registeredEffect = GetRegisteredEffect(effectType);

		if (registeredEffect)
		{
			registeredEffect->Start();

			std::ostringstream oss;
			oss << effectInfo.Name;

			if (suffix && strlen(suffix) > 0)
			{
				oss << " " << suffix;
			}

			oss << std::endl;

			m_activeEffects.emplace_back(effectType, registeredEffect, oss.str().c_str(), effectTime);
		}
	}

	m_percentage = .0f;
}

void EffectDispatcher::DispatchRandomEffect(const char* suffix)
{
	// Make sure we only dispatch enabled effects

	if (!m_enableNormalEffectDispatch)
	{
		return;
	}

	std::map<EffectType, EffectData> choosableEffects;
	for (const auto& pair : m_enabledEffects)
	{
		EffectType effectType = pair.first;
		const EffectData& effectData = pair.second;

		if (!effectData.EffectPermanent && (!m_disableTwiceInRow || effectType != m_lastEffect))
		{
			choosableEffects.emplace(effectType, effectData);
		}
	}

	int effectsTotalWeight = 0;
	for (const auto& pair : choosableEffects)
	{
		effectsTotalWeight += pair.second.EffectWeight * 10;
	}

	int index = Random::GetRandomInt(0, effectsTotalWeight);

	int addedUpWeight = 0;
	auto targetEffectType = _EFFECT_ENUM_MAX;
	for (const auto& pair : choosableEffects)
	{
		if (pair.second.EffectPermanent)
		{
			continue;
		}

		addedUpWeight += pair.second.EffectWeight * 10;

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
	for (auto effect : m_permanentEffects)
	{
		effect->Stop();
	}
	m_permanentEffects.clear();

	for (auto effect : m_activeEffects)
	{
		effect.RegisteredEffect->Stop();
	}
	m_activeEffects.clear();
}

void EffectDispatcher::Reset()
{
	ClearEffects();
	ResetTimer();

	m_enableNormalEffectDispatch = false;

	for (auto pair : m_enabledEffects)
	{
		if (pair.second.EffectPermanent)
		{
			auto registeredEffect = GetRegisteredEffect(pair.first);

			if (registeredEffect)
			{
				registeredEffect->Start();
				m_permanentEffects.push_back(registeredEffect);
			}
		}
		else
		{
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