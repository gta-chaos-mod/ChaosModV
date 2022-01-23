#pragma once

#include "Component.h"
#include <map>
#include "Effects/EffectIdentifier.h"


class ShortCut : public Component
{
private:

	std::map<int, std::vector<EffectIdentifier>> m_mAvailableShortcuts;
	std::vector<EffectIdentifier> m_pChosenEffectIdentifiers;
public:
	virtual void Run() override;
	void ParseShortcuts();
	void HandleInput(DWORD ulKey, bool bOnRepeat);

};