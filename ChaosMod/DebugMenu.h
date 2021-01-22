#pragma once

#include "Effects/EffectIdentifier.h"

#include <vector>
#include <memory>

typedef unsigned long DWORD;

struct DebugEffect
{
	DebugEffect(const EffectIdentifier& effectIdentifier, const std::string& effectName) : EffectIdentifier(effectIdentifier), EffectName(effectName)
	{
	
	}

	EffectIdentifier EffectIdentifier;
	std::string EffectName;
};

class DebugMenu
{
public:
	DebugMenu();

	void Tick();
	void HandleInput(DWORD key, bool onRepeat);
	void SetVisible(bool state);
	inline bool IsVisible() const
	{
		return m_visible;
	}

private:
	std::vector<DebugEffect> m_effects;
	int m_selected = 0;
	bool m_visible = false;
	DWORD m_repeatTime = 0;
	bool m_dispatchEffect = false;
};