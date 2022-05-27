#include <stdafx.h>

#include "Failsafe.h"

#include "Components/EffectDispatcher.h"

#include "Util/OptionsManager.h"
#include "Util/PoolSpawner.h"

Failsafe::Failsafe() : Component()
{
	m_bEnabled = g_OptionsManager.GetConfigValue<bool>("EnableFailsafe", OPTION_DEFAULT_FAILSAFE);
	if (!m_bEnabled)
	{
		LOG("Failsafe has been disabled in the config!");
	}

	m_piStateGlobal = nullptr;
}

void Failsafe::SetGlobalIndex(int idx)
{
	ms_iStateGlobalIdx = idx;
}

int Failsafe::GetGlobalIndex()
{
	return ms_iStateGlobalIdx;
}

void Failsafe::OnRun()
{
	if (!m_bEnabled || !ms_iStateGlobalIdx)
	{
		return;
	}

	if (!m_piStateGlobal)
	{
		m_piStateGlobal = reinterpret_cast<int *>(getGlobalPtr(ms_iStateGlobalIdx));
	}

	if (!*m_piStateGlobal && m_iLastState)
	{
		// Use game timer to exclude loading times
		int iCurTimestamp = GET_GAME_TIMER();

		if (m_iFailTimestamp < iCurTimestamp - 10000)
		{
			m_cFailCounts = 0;
		}

		switch (++m_cFailCounts)
		{
		case 3:
			LOG("[3 Fails] Clear most recent effect");

			GetComponent<EffectDispatcher>()->ClearMostRecentEffect();

			break;
		case 4:
			LOG("[4 Fails] Clear all spawned entities within 500 meters");

			ClearEntityPool(500);

			break;
		case 5:
			LOG("[5 Fails] Clear all effects and spawned entities");

			GetComponent<EffectDispatcher>()->ClearEffects(false);
			ClearEntityPool();

			m_cFailCounts = 0;

			break;
		}

		m_iFailTimestamp = iCurTimestamp;
	}

	m_iLastState = *m_piStateGlobal;
}