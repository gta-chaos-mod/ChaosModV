#pragma once

#include "Component.h"

class Failsafe : public Component
{
private:
	bool m_bEnabled = false;

	char m_cFailCounts = 0;
	int m_iFailTimestamp = 0;
	int m_iLastState = -1;
	int* m_piStateGlobal = nullptr;

	static inline int ms_iStateGlobalIdx = 0;

public:
	Failsafe();

	static void SetGlobalIndex(int idx);
	static int GetGlobalIndex();

	virtual void Run() override;
};