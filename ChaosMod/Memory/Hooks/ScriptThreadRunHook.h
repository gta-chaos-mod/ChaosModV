#pragma once

#include <Util/Events.h>

namespace rage
{
	class scrThread;
}

namespace Hooks
{
	inline ChaosCancellableEvent<rage::scrThread *> OnScriptThreadRun;

	void EnableScriptThreadBlock();

	void DisableScriptThreadBlock();
}