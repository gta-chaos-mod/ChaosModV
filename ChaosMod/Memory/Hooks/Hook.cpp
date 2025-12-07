#include "Hook.h"

#include "list"

namespace Memory
{
	RegisteredHook::RegisteredHook(const std::function<bool()> &hookFunc, const std::function<void()> &cleanupFunc,
	                               const std::string &name, const bool isLateHook)
	    : m_Name(name), m_HookFunc(hookFunc), m_CleanupFunc(cleanupFunc), m_IsLateHook(isLateHook)
	{
        ms_RegisteredHooks.push_back(this);
	}

	std::list<RegisteredHook*> GetRegisteredHooks()
	{
		return RegisteredHook::GetRegisteredHooks();
	}
}

RegisterHook::RegisterHook(std::function<bool()> hookFunc, std::function<void()> cleanupFunc, std::string name,
                           bool isLateHook)
    : m_RegisteredHook(hookFunc, cleanupFunc, name, isLateHook)
{
}