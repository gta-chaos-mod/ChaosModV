#include <stdafx.h>

struct LuaScript
{
	LuaScript(sol::state& lua, sol::protected_function_result& script) : m_lua(std::move(lua)), m_script(std::move(script))
	{

	}

private:
	sol::state m_lua;
	sol::protected_function_result m_script;
};

static std::map<std::string, LuaScript> s_loadedScripts;

static void LuaPrint(const std::string& text)
{
	g_log << "[Lua] " << text << std::endl;
}

static void LuaPrint(const std::string& name, const std::string& text)
{
	g_log << "[Lua] " << name << ": " << text << std::endl;
}

namespace LuaLoader
{
	void Load()
	{
		s_loadedScripts.clear();

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("chaosmod\\custom_scripts"))
		{
			if (entry.is_regular_file() && entry.file_size() > 0)
			{
				const std::filesystem::path& path = entry.path();
				const std::string& fileName = path.filename().string();

				LOG("Loading script " << fileName);

				sol::state lua;
				lua.open_libraries(sol::lib::base);

				lua["print"] = [fileName](const char* text) { LuaPrint(fileName, text); };

				sol::protected_function_result result = lua.safe_script_file(path.string());

				if (!result.valid())
				{
					sol::error error = result;

					LuaPrint(fileName, error.what());
				}
				else
				{
					s_loadedScripts.emplace(fileName, LuaScript(lua, result));
				}
			}
		}
	}

	const std::vector<std::string>& GetScriptNames()
	{
		std::vector<std::string> scriptNames;

		for (const auto& pair : s_loadedScripts)
		{
			scriptNames.push_back(pair.first);
		}

		return scriptNames;
	}
}