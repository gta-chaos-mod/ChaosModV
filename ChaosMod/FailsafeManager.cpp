#include <stdafx.h>

static int s_failCounts;
static int s_failTimestamp;
static int s_lastState;

namespace FailsafeManager
{
	void Reset()
	{
		s_failCounts = 0;
		s_failTimestamp = 0;
		s_lastState = -1;
	}

	void Run()
	{
		static bool enabled = getGameVersion() >= VER_1_0_2215_0_STEAM || DoesFileExist("chaosmod\\.forcefailsafe");
		static bool firstTime = true;
		if (!enabled)
		{
			if (firstTime)
			{
				LOG("Failsafe is incompatible with this version. :(");
				LOG("Use the .forcefailsafe feature flag to enable it anyways.");
			}

			return;
		}
		
		if (firstTime)
		{
			LOG("Failsafe enabled");
		}

		firstTime = false;

		int curState = *reinterpret_cast<int*>(getGlobalPtr(98955)); // Global found in replay_controller.ysc:119 as of build 2215

		if (!curState && s_lastState)
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

				g_effectDispatcher->ClearEffects();
				ClearEntityPool();

				s_failCounts = 0;

				break;
			}

			s_failTimestamp = curTimestamp;
		}

		s_lastState = curState;
	}
}