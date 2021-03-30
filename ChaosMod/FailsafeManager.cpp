#include <stdafx.h>

static bool s_enabled = false;

static int s_failCounts;
static int s_failTimestamp;
static int s_lastState;

static int* s_stateGlobal;

namespace FailsafeManager
{
	void Init()
	{
		eGameVersion gameVer = getGameVersion();
		s_enabled = gameVer >= VER_1_0_2215_0_STEAM && gameVer < VER_SIZE || DoesFileExist("chaosmod\\.forcefailsafe");

		if (!s_enabled)
		{
			LOG("Failsafe is incompatible with this version. :(");
			LOG("Use the .forcefailsafe feature flag to enable it anyways.");

			return;
		}

		LOG("Failsafe enabled");

		s_failCounts = 0;
		s_failTimestamp = 0;
		s_lastState = -1;

		s_stateGlobal = reinterpret_cast<int*>(getGlobalPtr(98955));
	}

	void Run()
	{
		if (!s_enabled)
		{
			return;
		}

		if (!*s_stateGlobal && s_lastState)
		{
			// Use game timer to exclude loading times
			int curTimestamp = GET_GAME_TIMER();

			if (s_failTimestamp < curTimestamp - 10000)
			{
				s_failCounts = 0;
			}

			switch (++s_failCounts)
			{
			case 3:
				LOG("[3 Fails] Clear most recent effect");

				g_effectDispatcher->ClearMostRecentEffect();

				break;
			case 4:
				LOG("[4 Fails] Clear all spawned entities within 500 meters");

				ClearEntityPool(500);

				break;
			case 5:
				LOG("[5 Fails] Clear all effects and spawned entities");

				g_effectDispatcher->ClearEffects(false);
				ClearEntityPool();

				s_failCounts = 0;

				break;
			}

			s_failTimestamp = curTimestamp;
		}

		s_lastState = *s_stateGlobal;
	}
}