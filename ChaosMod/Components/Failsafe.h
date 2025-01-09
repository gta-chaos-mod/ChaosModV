#pragma once

#include "Components/Component.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

class Failsafe : public Component
{
  private:
	CHAOS_EVENT_LISTENER(Hooks::OnScriptThreadRun) m_SearchFailGlobalListener;

	int *m_StateGlobal  = nullptr;
	int m_LastState     = -1;
	int m_FailTimestamp = 0;
	char m_FailCounts   = 0;
	bool m_Enabled      = false;

  protected:
	Failsafe();

  public:
	virtual void OnRun() override;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};