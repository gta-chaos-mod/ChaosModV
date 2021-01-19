#include <stdafx.h>

static void LuaPrint(const std::string& text)
{
	g_log << "[Lua] " << text << std::endl;
}

static void LuaPrint(const std::string& name, const std::string& text)
{
	g_log << "[Lua] " << name << ": " << text << std::endl;
}

struct LuaScript
{
	LuaScript(const std::string& fileName, sol::state& lua, LuaScriptInfo scriptInfo) : m_fileName(fileName), m_lua(std::move(lua)), ScriptInfo(scriptInfo)
	{

	}

	void Execute(const char* funcName) const
	{
		sol::protected_function func = m_lua[funcName];

		if (!func.valid())
		{
			return;
		}

		sol::protected_function_result result = func();

		if (!result.valid())
		{
			sol::error error = result;

			LuaPrint(m_fileName, error.what());
		}
	}

	const LuaScriptInfo ScriptInfo;

private:
	const std::string m_fileName;
	sol::state m_lua;
};

static std::map<std::string, LuaScript> s_registeredScripts;

namespace LuaManager
{
	void Load()
	{
		s_registeredScripts.clear();

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("chaosmod\\custom_scripts"))
		{
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".lua" && entry.file_size() > 0)
			{
				const std::filesystem::path& path = entry.path();
				const std::string& fileName = path.filename().string();

				LOG("Running script \"" << fileName << "\"");

				sol::state lua;

				lua["print"] = [fileName](const char* text) { LuaPrint(fileName, text); };

				sol::protected_function_result result = lua.safe_script_file(path.string());

				if (!result.valid())
				{
					sol::error error = result;

					LuaPrint(fileName, error.what());
				}
				else
				{
					sol::optional<sol::table> scriptInfoOpt = lua["ScriptInfo"];

					if (scriptInfoOpt)
					{
						const sol::table& scriptInfo = *scriptInfoOpt;

						sol::optional<std::string> scriptNameOpt = scriptInfo["Name"];

						if (scriptNameOpt)
						{
							const std::string& scriptName = *scriptNameOpt;

							if (s_registeredScripts.find(scriptName) != s_registeredScripts.end())
							{
								LOG("Could not register script \"" << fileName << "\" as \"" << scriptName << "\" (already exists!)");
							}
							else
							{
								sol::optional<bool> isTimedOpt = scriptInfo["IsTimed"];
								sol::optional<bool> isShortTimedOpt = scriptInfo["IsShortTimed"];

								s_registeredScripts.emplace(scriptName, LuaScript(fileName, lua, { isTimedOpt ? *isTimedOpt : false, isShortTimedOpt ? *isShortTimedOpt : false }));

								LOG("Registered script \"" << fileName << "\" as \"" << scriptName << "\"");
							}
						}
					}
				}
			}
		}
	}

	std::vector<std::string> GetScriptNames()
	{
		std::vector<std::string> scriptNames;

		for (const auto& pair : s_registeredScripts)
		{
			scriptNames.push_back(pair.first);
		}

		return scriptNames;
	}

	void Execute(const std::string& scriptName, const char* funcName)
	{
		const auto& result = s_registeredScripts.find(scriptName);

		if (result == s_registeredScripts.end())
		{
			return;
		}

		const LuaScript& script = result->second;

		script.Execute(funcName);
	}

	LuaScriptInfo GetScriptInfo(const std::string& scriptName)
	{
		const auto& result = s_registeredScripts.find(scriptName);

		if (result == s_registeredScripts.end())
		{
			return LuaScriptInfo();
		}

		return result->second.ScriptInfo;
	}
}