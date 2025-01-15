#pragma once

#include "Components/Component.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

class Failsafe : public Component
{
	CHAOS_EVENT_LISTENER(Hooks::OnScriptThreadRun) m_SearchFailGlobalListener;

	int *m_StateGlobal  = nullptr;
	int m_LastState     = -1;
	int m_FailTimestamp = 0;
	char m_FailCounts   = 0;
	bool m_Enabled      = false;

  public:
	Failsafe();

	virtual void OnRun() override;
};