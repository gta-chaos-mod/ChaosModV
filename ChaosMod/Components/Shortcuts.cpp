#include <stdafx.h>

#include "Shortcuts.h"

#include "Components/EffectDispatcher.h"

Shortcuts::Shortcuts() : Component()
{
	for (const auto &[effectId, effectData] : g_EnabledEffects)
	{
		if (effectData.ShortcutKeycode > 0 && !effectData.IsHidden())
		{
			m_AvailableShortcuts[effectData.ShortcutKeycode].push_back(effectId);
		}
	}
}

void Shortcuts::OnRun()
{
	if (!m_EffectQueue.empty())
	{
		std::lock_guard lock(m_EffectQueueMtx);
		while (!m_EffectQueue.empty())
		{
			auto &identifier = m_EffectQueue.front();
			GetComponent<EffectDispatcher>()->DispatchEffect(identifier);

			m_EffectQueue.pop();
		}
	}
}

void Shortcuts::OnKeyInput(DWORD key, bool repeated, bool isUpNow)
{
	if (repeated)
	{
		return;
	}

	if (m_AvailableShortcuts.contains(key))
	{
		std::lock_guard lock(m_EffectQueueMtx);
		for (auto &effectId : m_AvailableShortcuts.at(key))
		{
			m_EffectQueue.push(effectId);
		}
	}
}
