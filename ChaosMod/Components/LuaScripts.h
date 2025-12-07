#pragma once

#include "Components/Component.h"

#include <json.hpp>
#include <sol/sol.hpp>

#include <string>

class LuaScripts : public Component
{
	struct LuaScript
	{
		std::string ScriptName;
		sol::state Lua;
		bool IsTemporary;
		bool LateStateSetup;

		LuaScript(const std::string &scriptName, sol::state &lua, bool isTemporary)
		    : ScriptName(scriptName), Lua(std::move(lua)), IsTemporary(isTemporary), LateStateSetup(false)
		{
		}

		LuaScript(LuaScript &&script) noexcept
		    : ScriptName(std::move(script.ScriptName)),
		      Lua(std::move(script.Lua)),
		      IsTemporary(script.IsTemporary),
		      LateStateSetup(script.LateStateSetup)
		{
		}

		LuaScript &operator=(LuaScript &&script) noexcept
		{
			ScriptName     = std::move(script.ScriptName);
			Lua            = std::move(script.Lua);
			IsTemporary    = script.IsTemporary;
			LateStateSetup = script.LateStateSetup;

			return *this;
		}

		void Execute(const char *funcName) const;
	};
	std::unordered_map<std::string, LuaScript> m_RegisteredEffects;

  public:
	LuaScripts();

	virtual void OnModPauseCleanup(PauseCleanupFlags cleanupFlags = {}) override;

  private:
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
		Error
	};
	ParseScriptReturnReason ParseScript(std::string scriptName, const std::string &script,
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