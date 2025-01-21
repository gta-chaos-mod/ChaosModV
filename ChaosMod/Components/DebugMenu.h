#pragma once

#include "Components/Component.h"

#include "Effects/EffectIdentifier.h"

#include <vector>

using DWORD = unsigned long;

class DebugMenu : public Component
{
	struct DebugEffect
	{
		EffectIdentifier Id;
		std::string EffectName;

		DebugEffect(const EffectIdentifier &effectId, const std::string &effectName)
		    : Id(effectId), EffectName(effectName)
		{
		}
	};

	std::vector<DebugEffect> m_Effects;

	bool m_IsEnabled;

	int m_SelectedIdx     = 0;
	bool m_Visible        = false;

	DWORD m_RepeatTime    = 0;
	bool m_DispatchEffect = false;

  public:
	DebugMenu();

	virtual void OnRun() override;

	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed) override;

	bool IsEnabled() const;

	void SetVisible(bool state);
	bool IsVisible() const;
};