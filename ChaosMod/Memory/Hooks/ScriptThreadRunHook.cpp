#include <stdafx.h>

static bool ms_bEnabledHook = false;

__int64(*OG_rage__scrThread__Run)(rage::scrThread*);
__int64 HK_rage__scrThread__Run(rage::scrThread* pThread)
{
	if (ms_bEnabledHook)
	{
		const char* szScriptName = pThread->m_szName;

		// Scripthook (most likely) relies on these to run our script thread
		// We don't want to block ourselves of course :p
		if (strcmp(szScriptName, "main") && strcmp(szScriptName, "main_persistent") && strcmp(szScriptName, "control_thread"))
		{
			return 0;
		}
	}

	return OG_rage__scrThread__Run(pThread);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8D 81 D0 00 00 00");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Get<void>(), HK_rage__scrThread__Run, &OG_rage__scrThread__Run);

	return true;
}

static RegisterHook registerHook(OnHook, "rage::scrThread::Run");

namespace Hooks
{
	void EnableScriptThreadBlock()
	{
		ms_bEnabledHook = true;
	}

	void DisableScriptThreadBlock()
	{
		ms_bEnabledHook = false;
	}
}