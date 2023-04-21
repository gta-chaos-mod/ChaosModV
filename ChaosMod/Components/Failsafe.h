#pragma once

#include "Component.h"

class Failsafe : public Component
{
  private:
	bool m_Enabled                      = false;

	char m_FailCounts                   = 0;
	int m_FailTimestamp                 = 0;
	int m_LastState                     = -1;
	int *m_StateGlobal                  = nullptr;

	static inline int ms_StateGlobalIdx = 0;

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