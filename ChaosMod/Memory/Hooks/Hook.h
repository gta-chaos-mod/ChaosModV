#pragma once

#include <string>

#define _NODISCARD [[nodiscard]]

enum MH_STATUS : int;

namespace Memory
{
	class RegisteredHook;

	inline RegisteredHook* g_pRegisteredHooks = nullptr;

	class RegisteredHook
	{
	private:
		RegisteredHook* m_pNext = nullptr;
		const std::string m_szName;
		bool(*m_pHookFunc)();

	public:
		RegisteredHook(bool(*pHookFunc)(), const std::string& szName) : m_pHookFunc(pHookFunc), m_szName(szName)
		{
			if (g_pRegisteredHooks)
			{
				m_pNext = g_pRegisteredHooks;
			}

			g_pRegisteredHooks = this;
		}

		RegisteredHook(const RegisteredHook&) = delete;

		RegisteredHook& operator=(const RegisteredHook&) = delete;

		inline _NODISCARD bool RunHook()
		{
			return m_pHookFunc();
		}

		inline _NODISCARD const std::string& GetName() const
		{
			return m_szName;
		}

		inline _NODISCARD RegisteredHook* GetNext() const
		{
			return m_pNext;
		}
	};
}

class RegisterHook
{
private:
	const Memory::RegisteredHook m_RegisteredHook;

public:
	RegisterHook(bool(*pHookFunc)(), const std::string&& szName) : m_RegisteredHook(pHookFunc, szName)
	{

	}

	RegisterHook(const RegisterHook&) = delete;

	RegisterHook& operator=(const RegisterHook&) = delete;

	RegisterHook(RegisterHook&&) noexcept = delete;

	RegisterHook& operator=(RegisterHook&&) noexcept = delete;
};