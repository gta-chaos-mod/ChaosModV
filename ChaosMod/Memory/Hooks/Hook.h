#pragma once

#include <string>

enum MH_STATUS : int;

namespace Memory
{
	bool AddHook(void* pTarget, void* pDetour, void* ppOrig = nullptr);
}

class RegisteredHook;
inline RegisteredHook* g_pRegisteredHooks = nullptr;

class RegisteredHook
{
public:
	RegisteredHook(bool(*hookFunc)(), const std::string& name) : m_hookFunc(hookFunc), m_name(name)
	{
		if (g_pRegisteredHooks)
		{
			m_pNext = g_pRegisteredHooks;
		}

		g_pRegisteredHooks = this;
	}

	inline bool RunHook()
	{
		return m_hookFunc();
	}

	inline const std::string& GetName() const
	{
		return m_name;
	}

	inline RegisteredHook* GetNext() const
	{
		return m_pNext;
	}

private:
	RegisteredHook* m_pNext = nullptr;
	const std::string m_name;
	bool(*m_hookFunc)();
};

class RegisterHook
{
public:
	RegisterHook(bool(*hookFunc)(), const std::string&& name) : m_registeredHook(hookFunc, name)
	{

	}

private:
	const RegisteredHook m_registeredHook;
};