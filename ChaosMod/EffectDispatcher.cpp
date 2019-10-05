#include "stdafx.h"
#include "EffectDispatcher.h"
#include "Effects.h"

EffectDispatcher::EffectDispatcher(int effectSpawnTime, int effectTimedDur, bool timedEffects) : m_percentage(.0f), m_effects(new Effects()),
	m_effectSpawnTime(effectSpawnTime), m_effectTimedDur(effectTimedDur), m_spawnTimedEffects(timedEffects)
{
	Reset();
}

EffectDispatcher::~EffectDispatcher()
{
	ClearEffects();
	delete m_effects;
}

void EffectDispatcher::Draw()
{
	// New Effect Bar
	DRAW_RECT(.5f, .0f, 1.f, .05f, 0, 0, 0, 127, false);
	DRAW_RECT(m_percentage * .5f, .0f, m_percentage, .05f, 40, 40, 255, 255, false);

	// Effect Texts
	float y = .2f;
	for (ActiveEffect effect : m_activeEffects)
	{
		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(effect.Name);
		SET_TEXT_SCALE(.5f, .5f);
		SET_TEXT_COLOUR(255, 255, 255, 255);
		SET_TEXT_OUTLINE();
		SET_TEXT_WRAP(.0f, .86f);
		SET_TEXT_RIGHT_JUSTIFY(true);
		END_TEXT_COMMAND_DISPLAY_TEXT(.86f, y, 0);

		if (effect.Timer > 0)
		{
			DRAW_RECT(.9f, y + .02f, .05f, .02f, 0, 0, 0, 127, false);
			DRAW_RECT(.9f - effect.Timer / m_effectTimedDur, y + .02f, .05f * effect.Timer / m_effectTimedDur, .02f, 180, 180, 180, 255, false);
		}

		y += .075f;
	}
}

void EffectDispatcher::UpdateTimer()
{
	DWORD64 currentUpdateTime = GetTickCount64();

	float thing = currentUpdateTime - m_timerTimer;

	if (thing > 1000.f)
	{
		m_timerTimerRuns++;

		m_timerTimer = currentUpdateTime;
		thing = 0;
	}

	if ((m_percentage = (thing + (m_timerTimerRuns * 1000)) / (m_effectSpawnTime * 1000)) > 1.f)
	{
		DispatchRandomEffect();

		m_timerTimerRuns = 0;
	}
}

void EffectDispatcher::UpdateEffects()
{
	m_effects->UpdateEffects();

	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - m_effectsTimer) > 1000)
	{
		m_effectsTimer = currentUpdateTime;

		int activeEffectsSize = (int)m_activeEffects.size();
		std::vector<ActiveEffect>::iterator it;
		for (it = m_activeEffects.begin(); it != m_activeEffects.end(); )
		{
			ActiveEffect& effect = *it;

			effect.Timer--;

			if (effect.Timer == 0)
			{
				m_effects->StopEffect(effect.EffectType);
				it = m_activeEffects.erase(it);
			}
			// Prevent too many non-timed effects from displaying at once
			else if (effect.Timer < -180 + (activeEffectsSize > 3 ? ((activeEffectsSize - 3) * 20 < 160 ? (activeEffectsSize - 3) * 20 : 160) : 0))
			{
				it = m_activeEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

void EffectDispatcher::DispatchEffect(EffectType effectType)
{
	EffectInfo effectInfo = Effect.at(effectType);

	static std::ofstream log("chaosmod/effectsLog.txt");
	log << effectInfo.Name << std::endl;

	// Check if timed effect already is active, reset timer if so
	// Also check for incompatible effects
	bool alreadyExists = false;
	if (effectInfo.IsTimed)
	{
		const std::vector<EffectType> incompatibleEffects = effectInfo.IncompatibleWith;

		for (ActiveEffect& effect : m_activeEffects)
		{
			if (effect.EffectType == effectType)
			{
				alreadyExists = true;
				effect.Timer = m_effectTimedDur;
			}

			for (EffectType incompatibleEffect : incompatibleEffects)
			{
				if (effect.EffectType == incompatibleEffect)
				{
					effect.Timer = -180;
				}
			}
		}
	}

	if (!alreadyExists)
	{
		m_effects->StartEffect(effectType);
		m_activeEffects.emplace_back(effectType, effectInfo.Name,
			effectInfo.IsTimed ? effectInfo.Duration + m_effectTimedDur - effectInfo.Duration: -1);
	}

	m_percentage = .0f;
}

void EffectDispatcher::DispatchRandomEffect()
{
	std::vector<EffectType> effects;

	for (const auto& pair : Effect)
	{
		if (m_spawnTimedEffects || !pair.second.IsTimed)
		{
			effects.push_back(pair.first);
		}
	}

	int index = Random::GetRandomInt(0, effects.size() - 1);

	DispatchEffect(effects[index]);
}

void EffectDispatcher::ClearEffects()
{
	for (ActiveEffect effect : m_activeEffects)
	{
		m_effects->StopEffect(effect.EffectType);
	}

	m_activeEffects.clear();
}

void EffectDispatcher::Reset()
{
	ClearEffects();
	m_timerTimer = GetTickCount64();
	m_timerTimerRuns = 0;
	m_effectsTimer = GetTickCount64();
}