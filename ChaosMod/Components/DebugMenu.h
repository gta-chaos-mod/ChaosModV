#pragma once

#include "Components/Component.h"

#include "Effects/EffectIdentifier.h"

#include <vector>

using DWORD = unsigned long;

class DebugMenu : public Component
{
  private:
	struct DebugEffect
	{
		EffectIdentifier Identifier;
		std::string EffectName;

		DebugEffect(const EffectIdentifier &effectIdentifier, const std::string &effectName)
		    : Identifier(effectIdentifier), EffectName(effectName)
		{
		}
	};

	std::vector<DebugEffect> m_Effects;

	bool m_IsEnabled;

	int m_SelectedIdx     = 0;
	bool m_Visible        = false;

	DWORD m_RepeatTime    = 0;
	bool m_DispatchEffect = false;

  protected:
	DebugMenu();

  public:
	virtual void OnRun() override;

	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed) override;

	bool IsEnabled() const;

	void SetVisible(bool state);
	bool IsVisible() const;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};