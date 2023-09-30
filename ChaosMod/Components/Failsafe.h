#pragma once

#include "Components/Component.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

class Failsafe : public Component
{
  private:
	bool m_Enabled      = false;

	char m_FailCounts   = 0;
	int m_FailTimestamp = 0;
	int m_LastState     = -1;
	int *m_StateGlobal  = nullptr;

	CHAOS_EVENT_LISTENER(Hooks::OnScriptThreadRun) m_SearchFailGlobalListener;

  protected:
	Failsafe();

  public:
	static void SetGlobalIndex(int idx);
	static int GetGlobalIndex();

	virtual void OnRun() override;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};