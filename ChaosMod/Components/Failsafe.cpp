#include <stdafx.h>

#include "Failsafe.h"

#include "Components/EffectDispatcher.h"

#include "Lib/scrThread.h"

#include "Memory/Script.h"

#include "Util/OptionsManager.h"
#include "Util/PoolSpawner.h"

static int ms_StateGlobalIdx                 = 0;
static bool ms_SearchedForMissionStateGlobal = false;

Failsafe::Failsafe() : Component()
{
	m_Enabled = g_OptionsManager.GetConfigValue({ "EnableFailsafe" }, OPTION_DEFAULT_FAILSAFE);
	if (!m_Enabled)
	{
		LOG("Failsafe has been disabled in the config!");
		return;
	}

	m_SearchFailGlobalListener.Register(
	    Hooks::OnScriptThreadRun,
	    [](rage::scrThread *thread)
	    {
		    if (!ms_SearchedForMissionStateGlobal && !Failsafe::GetGlobalIndex() && !strcmp(thread->GetName(), "main"))
		    {
			    auto program = Memory::ScriptThreadToProgram(thread);
			    if (program->m_CodeBlocks)
			    {
				    ms_SearchedForMissionStateGlobal = true;

				    Handle handle;
				    if (getGameVersion() < VER_1_0_2802_0)
				    {
					    handle = Memory::FindScriptPattern("2D ? ? 00 00 25 0D 60 ? ? ? 6D 5E", program);
				    }
				    else
				    {
					    handle = Memory::FindScriptPattern("2D ? ? 00 00 25 0D 63 ? ? ? 70 61", program);
				    }

				    if (!handle.IsValid())
				    {
					    LOG("Fail state global not found; Failsafe can not be enabled!");
				    }
				    else
				    {
					    Failsafe::SetGlobalIndex(handle.At(8).Value<int>() & 0xFFFFFF);

					    LOG("Fail state global found (Global: " << Failsafe::GetGlobalIndex() << ")");
				    }
			    }
		    }

		    return true;
	    });
}

void Failsafe::SetGlobalIndex(int idx)
{
	ms_StateGlobalIdx = idx;
}

int Failsafe::GetGlobalIndex()
{
	return ms_StateGlobalIdx;
}

void Failsafe::OnRun()
{
	if (!m_Enabled || !ms_StateGlobalIdx || !ComponentExists<EffectDispatcher>())
	{
		return;
	}

	if (!m_StateGlobal)
	{
		m_StateGlobal = reinterpret_cast<int *>(Memory::GetGlobalPtr(ms_StateGlobalIdx));
	}

	if (!*m_StateGlobal && m_LastState)
	{
		// Use game timer to exclude loading times
		int curTimestamp = GET_GAME_TIMER();

		if (m_FailTimestamp < curTimestamp - 10000)
		{
			m_FailCounts = 0;
		}

		switch (++m_FailCounts)
		{
		case 3:
			LOG("[3 Fails] Clearing most recent effect");

			GetComponent<EffectDispatcher>()->ClearMostRecentEffect();

			break;
		case 4:
			LOG("[4 Fails] Clearing all spawned entities within 500 meters");

			ClearEntityPool(500);

			break;
		case 5:
			LOG("[5 Fails] Clearing all effects and spawned entities");

			GetComponent<EffectDispatcher>()->ClearEffects();
			ClearEntityPool();

			m_FailCounts = 0;

			break;
		}

		m_FailTimestamp = curTimestamp;
	}

	m_LastState = *m_StateGlobal;
}