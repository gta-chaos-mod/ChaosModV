#include <stdafx.h>

#include "Failsafe.h"

Failsafe::Failsafe()
{
	m_bEnabled = g_OptionsManager.GetConfigValue<bool>("EnableFailsafe", OPTION_DEFAULT_FAILSAFE);
	if (!m_bEnabled)
	{
		LOG("Failsafe has been disabled in the config!");

		return;
	}

	eGameVersion eGameVer = getGameVersion();
	m_bEnabled = eGameVer >= VER_1_0_2215_0_STEAM
		&& eGameVer < VER_SIZE
		|| DoesFileExist("chaosmod\\.forcefailsafe");

	if (!m_bEnabled)
	{
		LOG("Failsafe is incompatible with this version.");
		LOG("Use the .forcefailsafe feature flag to enable it anyways.");

		return;
	}

	LOG("Failsafe enabled");

	switch (eGameVer)
	{
	case VER_1_0_2215_0_STEAM:
	case VER_1_0_2215_0_NOSTEAM:
	case VER_1_0_2245_0_STEAM:
	case VER_1_0_2245_0_NOSTEAM:
		m_piStateGlobal = reinterpret_cast<int*>(getGlobalPtr(98955));

		break;
	case VER_1_0_2374_0_STEAM:
	case VER_1_0_2374_0_NOSTEAM:
		m_piStateGlobal = reinterpret_cast<int*>(getGlobalPtr(99370));

		break;
	}
}

void Failsafe::Run()
{
	if (!m_bEnabled)
	{
		return;
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

			g_pEffectDispatcher->ClearMostRecentEffect();

			break;
		case 4:
			LOG("[4 Fails] Clear all spawned entities within 500 meters");

			ClearEntityPool(500);

			break;
		case 5:
			LOG("[5 Fails] Clear all effects and spawned entities");

			g_pEffectDispatcher->ClearEffects(false);
			ClearEntityPool();

			m_cFailCounts = 0;

			break;
		}

		m_iFailTimestamp = iCurTimestamp;
	}

	m_iLastState = *m_piStateGlobal;
}