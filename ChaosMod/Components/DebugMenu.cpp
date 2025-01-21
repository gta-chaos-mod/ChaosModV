#include <stdafx.h>

#include "DebugMenu.h"

#include "Components/EffectDispatcher.h"
#include "Effects/EnabledEffects.h"
#include "Util/OptionsManager.h"

#define MAX_VIS_ITEMS 15

DebugMenu::DebugMenu() : Component()
{
	m_IsEnabled = g_OptionsManager.GetConfigValue({ "EnableDebugMenu" }, OPTION_DEFAULT_DEBUG_MENU);
	if (!m_IsEnabled)
		return;

	for (const auto &pair : g_EnabledEffects)
	{
		const auto &[effectId, effectData] = pair;

		if (!effectData.IsHidden())
			m_Effects.emplace_back(effectId, effectData.HasCustomName() ? effectData.CustomName : effectData.Name);
	}

	if (m_Effects.empty())
	{
		m_Effects.emplace_back(EffectIdentifier(), "No enabled effects :(");

		return;
	}

	std::sort(m_Effects.begin(), m_Effects.end(),
	          [](const DebugEffect &a, const DebugEffect &b)
	          {
		          for (int idx = 0;; idx++)
		          {
			          if (idx >= a.EffectName.size())
				          return false;
			          else if (idx >= b.EffectName.size())
				          return true;

			          auto ai = std::toupper(a.EffectName[idx]);
			          auto bi = std::toupper(b.EffectName[idx]);
			          if (ai != bi)
				          return ai < bi;
		          }
	          });
}

void DebugMenu::OnRun()
{
	if (!m_IsEnabled || !m_Visible)
		return;

	// Arrow Up
	DISABLE_CONTROL_ACTION(1, 27, true);
	DISABLE_CONTROL_ACTION(1, 127, true);
	DISABLE_CONTROL_ACTION(1, 188, true);
	DISABLE_CONTROL_ACTION(1, 300, true);

	// Arrow Down
	DISABLE_CONTROL_ACTION(1, 173, true);
	DISABLE_CONTROL_ACTION(1, 187, true);
	DISABLE_CONTROL_ACTION(1, 299, true);

	// Enter
	DISABLE_CONTROL_ACTION(1, 18, true);
	DISABLE_CONTROL_ACTION(1, 176, true);
	DISABLE_CONTROL_ACTION(1, 191, true);
	DISABLE_CONTROL_ACTION(1, 201, true);
	DISABLE_CONTROL_ACTION(1, 215, true);

	// Backspace
	DISABLE_CONTROL_ACTION(1, 177, true);
	DISABLE_CONTROL_ACTION(1, 194, true);
	DISABLE_CONTROL_ACTION(1, 202, true);

	if (m_DispatchEffect)
	{
		m_DispatchEffect = false;

		if (ComponentExists<EffectDispatcher>())
			GetComponent<EffectDispatcher>()->DispatchEffect(m_Effects[m_SelectedIdx].Id);
	}

	float y                 = .1f;
	WORD remainingDrawItems = MAX_VIS_ITEMS;

	for (int i = 0; remainingDrawItems > 0; i++)
	{
		short overflow = MAX_VIS_ITEMS / 2 - (m_Effects.size() - 1 - m_SelectedIdx);

		if (i < 0 || i < m_SelectedIdx - remainingDrawItems / 2 - (overflow > 0 ? overflow : 0))
			continue;
		else if (i >= m_Effects.size())
			break;

		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(m_Effects[i].EffectName.c_str());
		SET_TEXT_SCALE(.3f, .3f);
		SET_TEXT_CENTRE(true);
		SET_TEXT_PROPORTIONAL(true);
		SET_TEXT_JUSTIFICATION(0);

		if (i == m_SelectedIdx)
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

bool DebugMenu::IsEnabled() const
{
	return m_IsEnabled;
}

void DebugMenu::OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
                           bool isAltPressed)
{
	if (!m_IsEnabled || !m_Visible)
		return;

	if (repeated)
	{
		auto curTime = GetTickCount64();

		if (key == VK_RETURN || m_RepeatTime > curTime - 250)
			return;
	}
	else
	{
		m_RepeatTime = GetTickCount64();
	}

	switch (key)
	{
	case VK_UP:
		if (--m_SelectedIdx < 0)
			m_SelectedIdx = m_Effects.size() - 1;

		break;
	case VK_DOWN:
		if (++m_SelectedIdx >= m_Effects.size())
			m_SelectedIdx = 0;

		break;
	case VK_RIGHT:
	{
		char searchChar = std::tolower(m_Effects[m_SelectedIdx].EffectName[0]);

		bool found      = false;
		while (!found)
		{
			if (searchChar++ == SCHAR_MAX)
				searchChar = SCHAR_MIN;

			for (int idx = 0; idx < m_Effects.size(); idx++)
			{
				if (std::tolower(m_Effects[idx].EffectName[0]) == searchChar)
				{
					m_SelectedIdx = idx;

					found         = true;

					break;
				}
			}
		}

		break;
	}
	case VK_LEFT:
	{
		char searchChar = std::tolower(m_Effects[m_SelectedIdx].EffectName[0]);

		bool found      = false;
		while (!found)
		{
			if (searchChar-- == SCHAR_MIN)
				searchChar = SCHAR_MAX;

			for (int idx = 0; idx < m_Effects.size(); idx++)
			{
				if (std::tolower(m_Effects[idx].EffectName[0]) == searchChar)
				{
					m_SelectedIdx = idx;

					found         = true;

					break;
				}
			}
		}

		break;
	}
	case VK_RETURN:
		if (!m_Effects[m_SelectedIdx].Id.Id().empty())
			m_DispatchEffect = true;

		break;
	case VK_BACK:
		m_Visible = false;

		break;
	}
}

void DebugMenu::SetVisible(bool state)
{
	m_Visible = state;
}

bool DebugMenu::IsVisible() const
{
	return m_Visible;
}