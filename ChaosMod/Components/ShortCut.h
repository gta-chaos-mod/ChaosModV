#pragma once

#include "Component.h"
#include "Effects/EffectIdentifier.h"

#include <vector>

#define _NODISCARD [[nodiscard]]

using DWORD = unsigned long;

class ShortCut : public Component
{
private:

	bool m_bIsEnabled;
	std::unique_ptr<EffectIdentifier> m_pChosenEffectIdentifier;
	DWORD m_ulRepeatTime = 0;
	bool m_bDispatchEffect = false;
	void DispatchShortcutEffect(EEffectType type);
public:
	virtual void Run() override;

	void HandleInput(DWORD ulKey, bool bOnRepeat);

};