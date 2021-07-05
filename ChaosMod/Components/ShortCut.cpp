#include "stdafx.h"

#include "ShortCut.h"



void ShortCut::Run()
{
	if (m_bDispatchEffect && m_pChosenEffectIdentifier)
	{
		m_bDispatchEffect = false;

		g_pEffectDispatcher->DispatchEffect(*m_pChosenEffectIdentifier);
	}
}


void ShortCut::HandleInput(DWORD ulKey, bool bOnRepeat)
{

	if (bOnRepeat)
	{
		return;
	}

	switch (ulKey)
	{
	case VK_NUMPAD1:
		this->DispatchShortcutEffect(EEffectType::EFFECT_10XENGINE_VEHS);
		break;
	case VK_NUMPAD2:
		this->DispatchShortcutEffect(EEffectType::EFFECT_2XENGINE_VEHS);
		break;
	}
}

void ShortCut::DispatchShortcutEffect(EEffectType type)
{
	EffectIdentifier identifier = EffectIdentifier(type);
	if (g_EnabledEffects.contains(identifier))
	{
		m_pChosenEffectIdentifier = std::make_unique<EffectIdentifier>(identifier);
		m_bDispatchEffect = true;
	}
}
