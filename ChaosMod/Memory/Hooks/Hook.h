#pragma once

#include <minhook/include/MinHook.h>

#include <string>

namespace Memory
{
	class RegisteredHook;

	inline RegisteredHook *g_pRegisteredHooks = nullptr;

	class RegisteredHook
	{
	  private:
		std::string m_Name;
		std::function<bool()> m_HookFunc;
		std::function<void()> m_CleanupFunc;
		RegisteredHook *m_Next = nullptr;
		bool m_IsLateHook      = false;

	  public:
		RegisteredHook(std::function<bool()> hookFunc, std::function<void()> cleanupFunc, const std::string &name,
		               bool isLateHook)
		    : m_Name(name), m_HookFunc(hookFunc), m_CleanupFunc(cleanupFunc), m_IsLateHook(isLateHook)
		{
			if (g_pRegisteredHooks)
				m_Next = g_pRegisteredHooks;

			g_pRegisteredHooks = this;
		}

		RegisteredHook(const RegisteredHook &)            = delete;

		RegisteredHook &operator=(const RegisteredHook &) = delete;

		inline bool RunHook()
		{
			return m_HookFunc ? m_HookFunc() : true;
		}

		inline void RunCleanup()
		{
			if (m_CleanupFunc)
				m_CleanupFunc();
		}

		inline const std::string &GetName() const
		{
			return m_Name;
		}

		inline RegisteredHook *GetNext() const
		{
			return m_Next;
		}

		inline bool IsLateHook() const
		{
			return m_IsLateHook;
		}
	};
}

class RegisterHook
{
  private:
	const Memory::RegisteredHook m_RegisteredHook;

  public:
	RegisterHook(bool (*hookFunc)(), void (*cleanupFunc)(), const std::string &&name, bool isLateHook = false)
	    : m_RegisteredHook(hookFunc, cleanupFunc, name, isLateHook)
	{
	}

	RegisterHook(const RegisterHook &)                = delete;

	RegisterHook &operator=(const RegisterHook &)     = delete;

	RegisterHook(RegisterHook &&) noexcept            = delete;

	RegisterHook &operator=(RegisterHook &&) noexcept = delete;
};