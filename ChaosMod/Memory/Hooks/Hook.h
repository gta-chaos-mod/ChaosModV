#pragma once

#include <minhook/include/MinHook.h>

#include <string>

namespace Memory
{
	class RegisteredHook
	{
	  private:
		inline static std::list<RegisteredHook*> ms_RegisteredHooks = {};

		std::string m_Name;
		std::function<bool()> m_HookFunc;
		std::function<void()> m_CleanupFunc;
		bool m_IsLateHook;

	  public:
		RegisteredHook(const std::function<bool()> &hookFunc, const std::function<void()> &cleanupFunc,
		               const std::string &name, bool isLateHook);

		inline bool RunHook() const
		{
			return m_HookFunc ? m_HookFunc() : true;
		}

		inline void RunCleanup() const
		{
			if (m_CleanupFunc)
				m_CleanupFunc();
		}

		inline const std::string GetName() const
		{
			return m_Name;
		}

		inline bool IsLateHook() const
		{
			return m_IsLateHook;
		}

		static inline std::list<RegisteredHook*> GetRegisteredHooks()
		{
			return ms_RegisteredHooks;
		}
	};

	std::list<RegisteredHook*> GetRegisteredHooks();
}

class RegisterHook
{
  private:
	const Memory::RegisteredHook m_RegisteredHook;

  public:
	RegisterHook(std::function<bool()> hookFunc, std::function<void()> cleanupFunc, std::string name,
	             bool isLateHook = false);

	RegisterHook(const RegisterHook &)                = delete;

	RegisterHook &operator=(const RegisterHook &)     = delete;

	RegisterHook(RegisterHook &&) noexcept            = delete;

	RegisterHook &operator=(RegisterHook &&) noexcept = delete;
};