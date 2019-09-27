#include "stdafx.h"
#include "EffectDispatcher.h"
#include "Effects.h"

EffectDispatcher::EffectDispatcher() : m_percentage(.0f), m_effects(new Effects())
{

}

EffectDispatcher::~EffectDispatcher()
{
	delete m_effects;
}

void EffectDispatcher::Draw()
{
	// New Effects bar
	GRAPHICS::DRAW_RECT(.5f, .0f, 1.f, .05f, 0, 0, 0, 127);
	GRAPHICS::DRAW_RECT(m_percentage * .5f, .0f, m_percentage, .05f, 40, 40, 255, 255);

	// Effect Texts
	float y = .2f;
	for (DisplayedEffect effect : m_displayedEffects)
	{
		UI::BEGIN_TEXT_COMMAND_DISPLAY_TEXT((char*) "STRING");
		UI::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME((char*) effect.Name);
		UI::SET_TEXT_SCALE(.5f, .5f);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_OUTLINE();
		UI::END_TEXT_COMMAND_DISPLAY_TEXT(.8f, y += .075f);
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

void EffectDispatcher::UpdateDisplayedTexts()
{
	static DWORD64 pastUpdateTime = GetTickCount64();
	DWORD64 currentUpdateTime = GetTickCount64();

	if ((currentUpdateTime - pastUpdateTime) > 1000)
	{
		pastUpdateTime = currentUpdateTime;

		std::vector<DisplayedEffect>::iterator it;
		for (it = m_displayedEffects.begin(); it != m_displayedEffects.end(); )
		{
			DisplayedEffect& effect = *it;

			effect.Update();

			if (!effect.ShouldDisplay)
			{
				it = m_displayedEffects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

void EffectDispatcher::DispatchRandomEffect()
{
	EffectType effectType = (EffectType) GAMEPLAY::GET_RANDOM_INT_IN_RANGE(0, _EFFECT_ENUM_MAX - 1);
	EffectInfo effectInfo = Effect.at(effectType);
	m_effects->StartEffect(effectType);
	m_displayedEffects.emplace_back(effectInfo.Name, effectInfo.IsTimed ? effectInfo.Duration : 0);

	m_percentage = .0f;
}