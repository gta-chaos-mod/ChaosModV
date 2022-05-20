#pragma once

#include <minhook/include/MinHook.h>

#include <string>

#define _NODISCARD [[nodiscard]]

namespace Memory
{
	class RegisteredHook;

	inline RegisteredHook *g_pRegisteredHooks = nullptr;

	class RegisteredHook
	{
	  private:
		RegisteredHook *m_pNext = nullptr;
		const std::string m_szName;
		bool (*m_pHookFunc)();
		void (*m_pCleanupFunc)();
		const bool m_bIsLateHook = false;

	  public:
		RegisteredHook(bool (*pHookFunc)(), void (*pCleanupFunc)(), const std::string &szName, bool bIsLateHook)
			: m_pHookFunc(pHookFunc), m_pCleanupFunc(pCleanupFunc), m_szName(szName), m_bIsLateHook(bIsLateHook)
		{
			if (g_pRegisteredHooks)
			{
				m_pNext = g_pRegisteredHooks;
			}

			g_pRegisteredHooks = this;
		}

		RegisteredHook(const RegisteredHook &) = delete;

		RegisteredHook &operator=(const RegisteredHook &) = delete;

		_NODISCARD inline bool RunHook()
		{
			return m_pHookFunc ? m_pHookFunc() : true;
		}

		inline void RunCleanup()
		{
			if (m_pCleanupFunc)
			{
				m_pCleanupFunc();
			}
		}

		_NODISCARD inline const std::string &GetName() const
		{
			return m_szName;
		}

		_NODISCARD inline RegisteredHook *GetNext() const
		{
			return m_pNext;
		}

		_NODISCARD inline bool IsLateHook() const
		{
			return m_bIsLateHook;
		}
	};
}

class RegisterHook
{
  private:
	const Memory::RegisteredHook m_RegisteredHook;

  public:
	RegisterHook(bool (*pHookFunc)(), void (*pCleanupFunc)(), const std::string &&szName, bool bIsLateHook = false)
		: m_RegisteredHook(pHookFunc, pCleanupFunc, szName, bIsLateHook)
	{
	}

	RegisterHook(const RegisterHook &) = delete;

	RegisterHook &operator=(const RegisterHook &) = delete;

	RegisterHook(RegisterHook &&) noexcept        = delete;

	RegisterHook &operator=(RegisterHook &&) noexcept = delete;
};