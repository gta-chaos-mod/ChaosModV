#include "stdafx.h"
#include "EffectDispatcher.h"
#include "Effects.h"

EffectDispatcher::EffectDispatcher() : m_percentage(.0f), m_effects(new Effects())
{

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
			DRAW_RECT(.9f - effect.Timer / 180, y + .02f, .05f * effect.Timer / 180, .02f, 180, 180, 180, 255, false);
		}

		y += .075f;
	}
}

void EffectDispatcher::UpdateTimer()
{
	static DWORD64 pastUpdateTime = GetTickCount64();
	DWORD64 currentUpdateTime = GetTickCount64();

	if ((m_percentage = (currentUpdateTime - pastUpdateTime) / 60000.f) > 1.f)
	{
		pastUpdateTime = currentUpdateTime;

		DispatchRandomEffect();
	}
}

void EffectDispatcher::UpdateEffects()
{
	m_effects->UpdateEffects();

	static DWORD64 pastUpdateTime = GetTickCount64();
	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - pastUpdateTime) > 1000)
	{
		pastUpdateTime = currentUpdateTime;

		int activeEffectsSize = m_activeEffects.size();
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
			else if (effect.Timer < -180 + activeEffectsSize > 3 ? activeEffectsSize - 3 : 0) // Prevent too many effects displaying at once
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

#ifdef _DEBUG
	static std::ofstream log("effectsLog.txt");
	log << effectInfo.Name << std::endl;
#endif

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
				effect.Timer = 180;
			}

			for (EffectType incompatibleEffect : incompatibleEffects)
			{
				if (effect.EffectType == incompatibleEffect)
				{
					effect.Timer = 0;
				}
			}
		}
	}

	if (!alreadyExists)
	{
		m_effects->StartEffect(effectType);
		m_activeEffects.emplace_back(effectType, effectInfo.Name, effectInfo.IsTimed ? effectInfo.Duration : -1);
	}

	m_percentage = .0f;
}

void EffectDispatcher::DispatchRandomEffect()
{
	DispatchEffect((EffectType)Random::GetRandomInt(0, _EFFECT_ENUM_MAX - 1));
}

void EffectDispatcher::ClearEffects()
{
	for (ActiveEffect effect : m_activeEffects)
	{
		m_effects->StopEffect(effect.EffectType);
	}

	m_activeEffects.clear();
}