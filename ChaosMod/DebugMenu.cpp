#include "stdafx.h"

#include "DebugMenu.h"
#include "Effects.h"
#include "EffectDispatcher.h"

#define MAX_VIS_ITEMS 15

DebugMenu::DebugMenu()
{
	for (const auto& pair : g_enabledEffects)
	{
		const EffectData& effectData = pair.second;

		if (effectData.TimedType != EffectTimedType::TIMED_PERMANENT)
		{
			m_effects.emplace_back(pair.first, effectData.HasCustomName ? effectData.CustomName : effectData.Name);
		}
	}

	if (m_effects.empty())
	{
		m_effects.emplace_back(static_cast<EffectType>(-1), "No enabled effects :(");

		return;
	}

	std::sort(m_effects.begin(), m_effects.end(), [](DebugEffect a, DebugEffect b)
	{
		for (int i = 0; ; i++)
		{
			if (i >= a.EffectName.size() || std::toupper(a.EffectName[i]) < std::toupper(b.EffectName[i]))
			{
				return true;
			}
			else if (i >= b.EffectName.size() || std::toupper(b.EffectName[i]) < std::toupper(a.EffectName[i]))
			{
				return false;
			}
		}
	});
}

void DebugMenu::Tick()
{
	if (!m_visible)
	{
		return;
	}

	if (m_dispatchEffect)
	{
		m_dispatchEffect = false;

		g_effectDispatcher->DispatchEffect(m_effects[m_selected].EffectIdentifier);
	}

	float y = .1f;
	int remainingDrawItems = MAX_VIS_ITEMS;

	for (int i = 0; remainingDrawItems > 0; i++)
	{
		int overflow = MAX_VIS_ITEMS / 2 - (m_effects.size() - 1 - m_selected);

		if (i < 0 || i < m_selected - remainingDrawItems / 2 - (overflow > 0 ? overflow : 0))
		{
			continue;
		}
		else if (i >= m_effects.size())
		{
			break;
		}

		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(m_effects[i].EffectName.c_str());
		SET_TEXT_SCALE(.3f, .3f);
		SET_TEXT_CENTRE(true);
		SET_TEXT_PROPORTIONAL(true);
		SET_TEXT_JUSTIFICATION(0);

		if (i == m_selected)
		{
			DRAW_RECT(.1f, y, .2f, .05f, 255, 255, 255, 200, true);

			SET_TEXT_COLOUR(0, 0, 0, 255);
		}
		else
		{
			DRAW_RECT(.1f, y, .2f, .05f, 0, 0, 0, 200, true);

			SET_TEXT_COLOUR(255, 255, 255, 255);
		}

		END_TEXT_COMMAND_DISPLAY_TEXT(.1f, y - .0125f, 0);

		y += .05f;
		remainingDrawItems--;
	}
}

void DebugMenu::HandleInput(DWORD key, bool onRepeat)
{
	if (onRepeat)
	{
		DWORD curTime = GetTickCount64();

		if (key == VK_RETURN || m_repeatTime > curTime - 250)
		{
			return;
		}
	}
	else
	{
		m_repeatTime = GetTickCount64();
	}

	switch (key)
	{
	case VK_UP:
		if (--m_selected < 0)
		{
			m_selected = m_effects.size() - 1;
		}
		break;
	case VK_DOWN:
		if (++m_selected >= m_effects.size())
		{
			m_selected = 0;
		}
		break;
	case VK_RIGHT:
	{
		char searchChar = std::tolower(m_effects[m_selected].EffectName[0]);

		bool found = false;
		while (!found)
		{
			if (searchChar++ == SCHAR_MAX)
			{
				searchChar = SCHAR_MIN;
			}

			for (int i = 0; i < m_effects.size(); i++)
			{
				if (std::tolower(m_effects[i].EffectName[0]) == searchChar)
				{
					m_selected = i;

					found = true;

					break;
				}
			}
		}

		break;
	}
	case VK_LEFT:
	{
		char searchChar = std::tolower(m_effects[m_selected].EffectName[0]);

		bool found = false;
		while (!found)
		{
			if (searchChar-- == SCHAR_MIN)
			{
				searchChar = SCHAR_MAX;
			}

			for (int i = 0; i < m_effects.size(); i++)
			{
				if (std::tolower(m_effects[i].EffectName[0]) == searchChar)
				{
					m_selected = i;

					found = true;

					break;
				}
			}
		}

		break;
	}
	case VK_RETURN:
		if (m_effects[m_selected].EffectIdentifier.GetEffectType() != -1)
		{
			m_dispatchEffect = true;
		}
		break;
	case VK_BACK:
		m_visible = false;
		break;
	}
}

void DebugMenu::SetVisible(bool state)
{
	m_visible = state;
}