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

public:
	Failsafe();

	virtual void Run() override;
};