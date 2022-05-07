#include <stdafx.h>

#include "Shortcuts.h"

#include <sstream>

Shortcuts::Shortcuts() : Component()
{
	for (const auto& [effectId, effectData] : g_dictEnabledEffects)
	{
		if (effectData.Shortcut > 0)
		{
			m_ugAvailableShortcuts[effectData.Shortcut].push_back(effectId);
		}
	}
}

void Shortcuts::OnRun()
{
	std::lock_guard lock(m_effectQueueMtx);
	while (!m_effectQueue.empty())
	{
		auto& identifier = m_effectQueue.front();
		m_effectQueue.pop();

		GetComponent<EffectDispatcher>()->DispatchEffect(identifier);
	}
}

void Shortcuts::HandleInput(DWORD ulKey, bool bOnRepeat)
{
	if (bOnRepeat)
	{
		return;
	}

	if (m_ugAvailableShortcuts.contains(ulKey))
	{
		std::lock_guard lock(m_effectQueueMtx);
		for (auto& effectId : m_ugAvailableShortcuts.at(ulKey))
		{
			m_effectQueue.push(effectId);
		}
	}
}
