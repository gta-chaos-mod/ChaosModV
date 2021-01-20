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
	LuaScript(const std::string& fileName, sol::state& lua) : m_fileName(fileName), m_lua(std::move(lua))
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

		ClearRegisteredScriptEffects();

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("chaosmod\\custom_scripts"))
		{
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".lua" && entry.file_size() > 0)
			{
				const std::filesystem::path& path = entry.path();
				const std::string& fileName = path.filename().string();

				LOG("Running script \"" << fileName << "\"");

				sol::state lua;

				lua["print"] = [fileName](const char* text) { LuaPrint(fileName, text); };

				sol::protected_function_result result = lua.safe_script_file(path.string(), sol::load_mode::text);

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
						sol::optional<std::string> scriptIdOpt = scriptInfo["ScriptId"];

						if (scriptNameOpt && scriptIdOpt)
						{
							const std::string& scriptId = *scriptIdOpt;
							const std::string& scriptName = *scriptNameOpt;

							if (s_registeredScripts.find(scriptId) != s_registeredScripts.end())
							{
								LOG("Could not register script \"" << fileName << "\" (with name \"" << scriptName << "\") as \"" << scriptId << "\" (already exists!)");
							}
							else
							{
								EffectData effectData;
								effectData.Name = scriptName;
								effectData.Id = *scriptIdOpt;

								sol::optional<std::string> timedTypeTextOpt = scriptInfo["TimedType"];

								if (timedTypeTextOpt)
								{
									const std::string& timedTypeText = *timedTypeTextOpt;

									if (timedTypeText == "Normal")
									{
										effectData.TimedType = EffectTimedType::TIMED_NORMAL;
									}
									else if (timedTypeText == "Short")
									{
										effectData.TimedType = EffectTimedType::TIMED_SHORT;
									}
									else if (timedTypeText == "Permanent")
									{
										effectData.TimedType = EffectTimedType::TIMED_PERMANENT;
									}
									else if (timedTypeText == "Custom")
									{
										sol::optional<int> durationOpt = scriptInfo["CustomTime"];

										if (durationOpt && *durationOpt > 0)
										{
											effectData.TimedType = EffectTimedType::TIMED_CUSTOM;
											effectData.CustomTime = *durationOpt;
										}
									}
								}

								sol::optional<int> weightMultOpt = scriptInfo["WeightMultiplier"];

								if (weightMultOpt && *weightMultOpt > 0)
								{
									effectData.WeightMult = *weightMultOpt;
								}

								sol::optional<sol::table> incompatibleIdsOpt = scriptInfo["IncompatibleIds"];
								if (incompatibleIdsOpt)
								{
									const sol::table& incompatibleIds = *incompatibleIdsOpt;

									for (const auto& entry : incompatibleIds)
									{
										if (entry.second.valid() && entry.second.is<std::string>())
										{
											effectData.IncompatibleIds.push_back(entry.second.as<std::string>());
										}
									}
								}

								s_registeredScripts.emplace(scriptId, LuaScript(fileName, lua));

								g_enabledEffects.emplace(scriptId, effectData);

								g_registeredEffects.emplace_back(scriptId);

								LOG("Registered script \"" << fileName << "\" as \"" << scriptId << "\" with name \"" << scriptName << "\"");
							}
						}
					}
				}
			}
		}
	}

	std::vector<std::string> GetScriptIds()
	{
		std::vector<std::string> scriptIds;

		for (const auto& pair : s_registeredScripts)
		{
			scriptIds.push_back(pair.first);
		}

		return scriptIds;
	}

	void Execute(const std::string& scriptId, const char* funcName)
	{
		const auto& result = s_registeredScripts.find(scriptId);

		if (result == s_registeredScripts.end())
		{
			return;
		}

		const LuaScript& script = result->second;

		script.Execute(funcName);
	}
}