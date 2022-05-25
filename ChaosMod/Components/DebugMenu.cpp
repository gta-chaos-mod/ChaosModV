#include <stdafx.h>

#include "DebugMenu.h"

#include "Components/EffectDispatcher.h"

#include "Util/OptionsManager.h"

#define MAX_VIS_ITEMS 15

DebugMenu::DebugMenu() : Component()
{
	m_bIsEnabled = g_OptionsManager.GetConfigValue<bool>("EnableDebugMenu", OPTION_DEFAULT_DEBUG_MENU);
	if (!m_bIsEnabled)
	{
		return;
	}

	for (const auto &pair : g_dictEnabledEffects)
	{
		const auto &[effectIdentifier, effectData] = pair;

		if (effectData.TimedType != EEffectTimedType::Permanent)
		{
			m_rgEffects.emplace_back(effectIdentifier,
			                         effectData.HasCustomName() ? effectData.CustomName : effectData.Name);
		}
	}

	if (m_rgEffects.empty())
	{
		m_rgEffects.emplace_back(EffectIdentifier(), "No enabled effects :(");

		return;
	}

	std::sort(m_rgEffects.begin(), m_rgEffects.end(),
	          [](const DebugEffect &a, const DebugEffect &b)
	          {
		          for (int idx = 0;; idx++)
		          {
			          if (idx >= a.m_szEffectName.size()
			              || std::toupper(a.m_szEffectName[idx]) < std::toupper(b.m_szEffectName[idx]))
			          {
				          return true;
			          }
			          else if (idx >= b.m_szEffectName.size()
			                   || std::toupper(b.m_szEffectName[idx]) < std::toupper(a.m_szEffectName[idx]))
			          {
				          return false;
			          }
		          }
	          });
}

void DebugMenu::OnRun()
{
	if (!m_bIsEnabled || !m_bVisible)
	{
		return;
	}

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

	if (m_bDispatchEffect)
	{
		m_bDispatchEffect = false;

		GetComponent<EffectDispatcher>()->DispatchEffect(m_rgEffects[m_iSelectedIdx].m_EffectIdentifier);
	}

	float fY                   = .1f;
	WORD culRemainingDrawItems = MAX_VIS_ITEMS;

	for (int idx = 0; culRemainingDrawItems > 0; idx++)
	{
		short sOverflow = MAX_VIS_ITEMS / 2 - (m_rgEffects.size() - 1 - m_iSelectedIdx);

		if (idx < 0 || idx < m_iSelectedIdx - culRemainingDrawItems / 2 - (sOverflow > 0 ? sOverflow : 0))
		{
			continue;
		}
		else if (idx >= m_rgEffects.size())
		{
			break;
		}

		BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(m_rgEffects[idx].m_szEffectName.c_str());
		SET_TEXT_SCALE(.3f, .3f);
		SET_TEXT_CENTRE(true);
		SET_TEXT_PROPORTIONAL(true);
		SET_TEXT_JUSTIFICATION(0);

		if (idx == m_iSelectedIdx)
		{
			DRAW_RECT(.1f, fY, .2f, .05f, 255, 255, 255, 200, true);

			SET_TEXT_COLOUR(0, 0, 0, 255);
		}
		else
		{
			DRAW_RECT(.1f, fY, .2f, .05f, 0, 0, 0, 200, true);

			SET_TEXT_COLOUR(255, 255, 255, 255);
		}

		END_TEXT_COMMAND_DISPLAY_TEXT(.1f, fY - .0125f, 0);

		fY += .05f;
		culRemainingDrawItems--;
	}
}

_NODISCARD bool DebugMenu::IsEnabled() const
{
	return m_bIsEnabled;
}

void DebugMenu::HandleInput(DWORD ulKey, bool bOnRepeat)
{
	if (!m_bIsEnabled || !m_bVisible)
	{
		return;
	}

	if (bOnRepeat)
	{
		DWORD ulCurTime = GetTickCount64();

		if (ulKey == VK_RETURN || m_ulRepeatTime > ulCurTime - 250)
		{
			return;
		}
	}
	else
	{
		m_ulRepeatTime = GetTickCount64();
	}

	switch (ulKey)
	{
	case VK_UP:
		if (--m_iSelectedIdx < 0)
		{
			m_iSelectedIdx = m_rgEffects.size() - 1;
		}

		break;
	case VK_DOWN:
		if (++m_iSelectedIdx >= m_rgEffects.size())
		{
			m_iSelectedIdx = 0;
		}

		break;
	case VK_RIGHT:
	{
		char cSearchChar = std::tolower(m_rgEffects[m_iSelectedIdx].m_szEffectName[0]);

		bool bFound      = false;
		while (!bFound)
		{
			if (cSearchChar++ == SCHAR_MAX)
			{
				cSearchChar = SCHAR_MIN;
			}

			for (int idx = 0; idx < m_rgEffects.size(); idx++)
			{
				if (std::tolower(m_rgEffects[idx].m_szEffectName[0]) == cSearchChar)
				{
					m_iSelectedIdx = idx;

					bFound         = true;

					break;
				}
			}
		}

		break;
	}
	case VK_LEFT:
	{
		char cSearchChar = std::tolower(m_rgEffects[m_iSelectedIdx].m_szEffectName[0]);

		bool bFound      = false;
		while (!bFound)
		{
			if (cSearchChar-- == SCHAR_MIN)
			{
				cSearchChar = SCHAR_MAX;
			}

			for (int idx = 0; idx < m_rgEffects.size(); idx++)
			{
				if (std::tolower(m_rgEffects[idx].m_szEffectName[0]) == cSearchChar)
				{
					m_iSelectedIdx = idx;

					bFound         = true;

					break;
				}
			}
		}

		break;
	}
	case VK_RETURN:
		if (!m_rgEffects[m_iSelectedIdx].m_EffectIdentifier.GetEffectId().empty())
		{
			m_bDispatchEffect = true;
		}

		break;
	case VK_BACK:
		m_bVisible = false;

		break;
	}
}

void DebugMenu::SetVisible(bool bState)
{
	m_bVisible = bState;
}

_NODISCARD bool DebugMenu::IsVisible() const
{
	return m_bVisible;
}