#include "stdafx.h"
#include <sstream>

#include "ShortCut.h"



void ShortCut::Run()
{
	if (!m_pChosenEffectIdentifiers.empty())
	{
		EffectIdentifier identifier = m_pChosenEffectIdentifiers[m_pChosenEffectIdentifiers.size() - 1];
		m_pChosenEffectIdentifiers.pop_back();
		g_pEffectDispatcher->DispatchEffect(identifier);
	}
}

void ShortCut::ParseShortcuts()
{
	m_mAvailableShortcuts.clear();
	for (auto eff : g_EnabledEffects)
	{
		if (eff.second.Shortcut > 0)
		{
			if (m_mAvailableShortcuts.contains(eff.second.Shortcut))
			{
				m_mAvailableShortcuts[eff.second.Shortcut].push_back(eff.first);
			}
			else
			{
				m_mAvailableShortcuts[eff.second.Shortcut] = { eff.first };
			}
		}
	}
}

void ShortCut::HandleInput(DWORD ulKey, bool bOnRepeat)
{

	if (bOnRepeat)
	{
		return;
	}

	if (m_mAvailableShortcuts.contains(ulKey))
	{
		std::vector<EffectIdentifier> shortcutEffects = m_mAvailableShortcuts[ulKey];
		std::vector<EffectIdentifier> effectsToDispatch;
		for (auto identifier : shortcutEffects)
		{
			if (g_EnabledEffects.contains(identifier))
			{
				effectsToDispatch.push_back(identifier);
			}
		}
		if (!effectsToDispatch.empty())
		{
			m_pChosenEffectIdentifiers = effectsToDispatch;
		}

	}
}
