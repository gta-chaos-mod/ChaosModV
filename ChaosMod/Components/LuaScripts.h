#pragma once

#include "Components/Component.h"

#define SOL_ALL_SAFETIES_ON 1
#define SOL_DEFAULT_PASS_ON_ERROR 1
#include <sol/sol.hpp>

#include <json.hpp>

#include <mutex>
#include <string>

class LuaScripts : public Component
{
	sol::state m_GlobalState;
	class LuaScript
	{
		std::string m_ScriptName;
		sol::environment m_Env;
		bool m_IsTemporary;

	  public:
		LuaScript(const std::string &scriptName, sol::environment &env, bool isTemporary)
		    : m_ScriptName(scriptName), m_Env(env), m_IsTemporary(isTemporary)
		{
		}

		LuaScript(LuaScript &&script) noexcept
		    : m_ScriptName(std::move(script.m_ScriptName)),
		      m_Env(std::move(script.m_Env)),
		      m_IsTemporary(script.m_IsTemporary)
		{
		}

		LuaScript &operator=(LuaScript &&script) noexcept
		{
			m_ScriptName  = std::move(script.m_ScriptName);
			m_Env         = std::move(script.m_Env);
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
			const sol::protected_function &func = m_Env[funcName];
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
	std::mutex m_RegisteredEffectsMutex;

  public:
	LuaScripts();

	virtual void OnModPauseCleanup() override;

  private:
	void SetupGlobalState();

	enum ParseScriptFlags
	{
		ParseScriptFlag_None,
		// Immediately dispatch effect (if script registers one) and remove it OnStop
		// Assumes this is only being called from the main thread!
		ParseScriptFlag_IsTemporary      = (1 << 0),
		// Whether the script parameter is not the code but rather a file path to it
		ParseScriptFlag_ScriptIsFilePath = (1 << 1)
	};
	enum class ParseScriptReturnReason
	{
		Success,
		Error,
		Error_ThreadUnsafe
	};
	ParseScriptReturnReason ParseScriptRaw(std::string scriptName, const std::string &script,
	                                       ParseScriptFlags flags = ParseScriptFlag_None,
	                                       bool *isThreadUnsafe   = nullptr,
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