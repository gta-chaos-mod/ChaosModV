#pragma once

#include "Components/Component.h"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_DEFAULT_PASS_ON_ERROR 1
#include <sol/sol.hpp>

#include <json.hpp>

#include <string>

class LuaScripts : public Component
{
	class LuaScript
	{
		std::string m_ScriptName;
		sol::state m_Lua;
		bool m_IsTemporary;

	  public:
		LuaScript(const std::string &scriptName, sol::state &lua, bool isTemporary)
		    : m_ScriptName(scriptName), m_Lua(std::move(lua)), m_IsTemporary(isTemporary)
		{
		}

		LuaScript(const LuaScript &)            = delete;

		LuaScript &operator=(const LuaScript &) = delete;

		LuaScript(LuaScript &&script) noexcept
		    : m_ScriptName(std::move(script.m_ScriptName)),
		      m_Lua(std::move(script.m_Lua)),
		      m_IsTemporary(script.m_IsTemporary)
		{
		}

		LuaScript &operator=(LuaScript &&script) noexcept
		{
			m_ScriptName  = std::move(script.m_ScriptName);
			m_Lua         = std::move(script.m_Lua);
			m_IsTemporary = script.m_IsTemporary;

			return *this;
		}

		const std::string &GetScriptName() const
		{
			return m_ScriptName;
		}

		bool IsTemporary() const
		{
			return m_IsTemporary;
		}

		void Execute(const char *funcName) const
		{
			const sol::protected_function &func = m_Lua[funcName];
			if (!func.valid())
				return;

			const sol::protected_function_result &result = func();
			if (!result.valid())
			{
				const sol::error &error = result;

				extern void LuaPrint(const std::string &name, const std::string &text);
				LuaPrint(m_ScriptName, error.what());
			}
		}
	};
	std::unordered_map<std::string, LuaScript> m_RegisteredEffects;

  public:
	LuaScripts();

	virtual void OnModPauseCleanup() override;

  private:
	enum ParseScriptFlags
	{
		ParseScriptFlag_None,
		// Immediately dispatch effect (if script registers one) and remove it OnStop
		// Assumes this is only being called from the main thread!
		ParseScriptFlag_IsTemporary      = (1 << 0),
		// Whether this is called from an alien thread, aborts and returns ParseScriptReturnReason::Error_ThreadUnsafe
		// if thread-unsafe function was called
		ParseScriptFlag_IsAlienThread    = (1 << 1),
		// Whether the script parameter is not the code but rather a file path to it
		ParseScriptFlag_ScriptIsFilePath = (1 << 2)
	};
	enum class ParseScriptReturnReason
	{
		Success,
		Error,
		Error_ThreadUnsafe
	};
	ParseScriptReturnReason ParseScriptRaw(std::string scriptName, const std::string &script,
	                                       ParseScriptFlags flags = ParseScriptFlag_None,
	                                       std::unordered_map<std::string, nlohmann::json> settingOverrides = {});
	void RemoveScriptEntry(const std::string &effectId);

  public:
	enum class ExecuteFuncType
	{
		Start,
		Stop,
		Tick,
	};
	void Execute(const std::string &effectId, ExecuteFuncType funcType);

	void RegisterScriptRawTemporary(std::string scriptName, std::string script);
};