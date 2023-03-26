#include <stdafx.h>

#include "Shortcuts.h"

#include "Components/EffectDispatcher.h"

Shortcuts::Shortcuts() : Component()
{
	for (const auto &[effectId, effectData] : g_dictEnabledEffects)
	{
		if (effectData.ShortcutKeycode > 0 && !effectData.IsHidden())
		{
			m_ugAvailableShortcuts[effectData.ShortcutKeycode].push_back(effectId);
		}
	}
}

void Shortcuts::OnRun()
{
	if (!m_effectQueue.empty())
	{
		std::lock_guard lock(m_effectQueueMtx);
		while (!m_effectQueue.empty())
		{
			auto &identifier = m_effectQueue.front();
			GetComponent<EffectDispatcher>()->DispatchEffect(identifier);

			m_effectQueue.pop();
		}
	}
}

void Shortcuts::OnKeyInput(DWORD key, bool repeated, bool isUpNow)
{
	if (repeated)
	{
		return;
	}

	if (m_ugAvailableShortcuts.contains(key))
	{
		std::lock_guard lock(m_effectQueueMtx);
		for (auto &effectId : m_ugAvailableShortcuts.at(key))
		{
			m_effectQueue.push(effectId);
		}
	}
}
