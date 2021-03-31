#include <stdafx.h>

#define LUA_SCRIPTS_DIR "chaosmod\\custom_scripts"
#define LUA_NATIVESDEF_DIR "chaosmod\\natives_def.lua"

static __forceinline void LuaPrint(const std::string& text)
{
	COLOR_PREFIX_LOG("[Lua]", text);
}

static __forceinline void LuaPrint(const std::string& name, const std::string& text)
{
	COLOR_PREFIX_LOG("[" << name << "]", text);
}

static __forceinline char* _TryParseString(void* ptr)
{
	__try
	{
		char* string = reinterpret_cast<char*>(ptr);

		for (char* c = string; *c; c++)
		{

		}

		return string;
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return nullptr;
	}
}

static __forceinline bool _TryParseVector3(void** ptr, float& x, float& y, float& z)
{
	__try
	{
		x = *reinterpret_cast<float*>(ptr);
		y = *reinterpret_cast<float*>(ptr + 1);
		z = *reinterpret_cast<float*>(ptr + 2);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return false;
	}

	return true;
}

static __forceinline bool _CallNative(void*** result)
{
	__try
	{
		*result = reinterpret_cast<void**>(nativeCall());
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

template <typename T, typename... A>
static __forceinline T Generate(const A&... args)
{
	return T(args...);
}

struct LuaScript
{
	LuaScript() = default;

	LuaScript(const std::string& fileName, sol::state& lua) : m_fileName(fileName), m_lua(std::move(lua))
	{

	}

	void Execute(const char* const funcName) const
	{
		const sol::protected_function& func = m_lua[funcName];

		if (!func.valid())
		{
			return;
		}

		const sol::protected_function_result& result = func();

		if (!result.valid())
		{
			const sol::error& error = result;

			LuaPrint(m_fileName, error.what());
		}
	}

private:
	const std::string m_fileName;
	sol::state m_lua;
};

static std::unordered_map<std::string, LuaScript> s_registeredScripts;

static struct LuaVector3
{
	LuaVector3() = default;

	LuaVector3(float x, float y, float z) : x(x), y(y), z(z)
	{

	}

	float x = 0.f;
	DWORD _paddingX;
	float y = 0.f;
	DWORD _paddingY;
	float z = 0.f;
	DWORD _paddingZ;
};

static struct LuaHolder
{
	LuaHolder() = default;

	LuaHolder(const sol::object& obj) : obj(obj)
	{

	}

	void* data = nullptr;
	const sol::object obj;

	template <typename T>
	__forceinline T As()
	{
		if constexpr (std::is_same<T, char*>())
		{
			return _TryParseString(data);
		}
		else if constexpr (std::is_same<T, LuaVector3>())
		{
			float x, y, z;

			return _TryParseVector3(&data, x, y, z) ? LuaVector3(x, y, z) : LuaVector3();
		}

		return *reinterpret_cast<T*>(&data);
	}

	__forceinline bool IsValid() const
	{
		return data || obj.valid();
	}
};

enum class LuaNativeReturnType
{
	NONE,
	BOOL,
	INT,
	FLOAT,
	STRING,
	VECTOR3
};

static __forceinline sol::object LuaInvoke(const std::string& fileName, const sol::this_state& lua, DWORD64 nativeHash, LuaNativeReturnType returnType, const sol::variadic_args& args)
{
	nativeInit(nativeHash);

	for (const sol::stack_proxy& arg : args)
	{
		if (arg.is<bool>())
		{
			nativePush(arg.get<bool>());
		}
		else if (arg.is<int>())
		{
			nativePush(arg.get<int>());
		}
		else if (arg.is<float>())
		{
			nativePush(arg.get<float>());
		}
		else if (arg.is<const char*>())
		{
			nativePush(arg.get<const char*>());
		}
		else if (arg.is<LuaHolder>())
		{
			LuaHolder& holder = arg.get<LuaHolder>();

			if (holder.obj.valid())
			{
				DWORD64 data = holder.obj.as<DWORD64>();

				nativePush(&data);
			}
			else
			{
				nativePush(&holder.data);
			}
		}
	}

	void** returned;
	if (!_CallNative(&returned))
	{
		LuaPrint(fileName, (std::ostringstream() << "Error while invoking native 0x" << nativeHash).str());
	}
	else if (returned)
	{
		void* result = *returned;

		switch (returnType)
		{
		case LuaNativeReturnType::BOOL:
			return sol::make_object(lua, reinterpret_cast<bool>(result));
		case LuaNativeReturnType::INT:
			return sol::make_object(lua, reinterpret_cast<int>(result));
		case LuaNativeReturnType::FLOAT:
			return sol::make_object(lua, *reinterpret_cast<float*>(&result));
		case LuaNativeReturnType::STRING:
			return sol::make_object(lua, _TryParseString(result));
		case LuaNativeReturnType::VECTOR3:
		{
			LuaVector3 vector3;
			if (_TryParseVector3(returned, vector3.x, vector3.y, vector3.z))
			{
				return sol::make_object(lua, vector3);
			}
		}
		}
	}

	return sol::make_object(lua, sol::lua_nil);
}

namespace LuaManager
{
	void Load()
	{
		s_registeredScripts.clear();

		ClearRegisteredScriptEffects();

		if (!DoesFileExist(LUA_SCRIPTS_DIR))
		{
			return;
		}

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(LUA_SCRIPTS_DIR))
		{
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".lua" && entry.file_size() > 0)
			{
				const std::filesystem::path& path = entry.path();
				const std::string& fileName = path.filename().string();

				const std::string& pathStr = path.string();
				LOG("Running script " << pathStr.substr(strlen(LUA_SCRIPTS_DIR) + 1));

				sol::state lua;
				lua.open_libraries(sol::lib::base);
				lua.open_libraries(sol::lib::math);
				lua.open_libraries(sol::lib::table);
				lua.open_libraries(sol::lib::string);
				lua.open_libraries(sol::lib::bit32);

				lua["ReturnType"] = lua.create_table_with(
					"None", LuaNativeReturnType::NONE,
					"Boolean", LuaNativeReturnType::BOOL,
					"Integer", LuaNativeReturnType::INT,
					"String", LuaNativeReturnType::STRING,
					"Float", LuaNativeReturnType::FLOAT,
					"Vector3", LuaNativeReturnType::VECTOR3
				);

				if (DoesFileExist(LUA_NATIVESDEF_DIR))
				{
					lua.unsafe_script_file(LUA_NATIVESDEF_DIR);
				}

				lua["print"] = [fileName](const std::string& text) { LuaPrint(fileName, text); };
				lua["GetTickCount"] = GetTickCount64;
				lua["GET_HASH_KEY"] = GET_HASH_KEY;

				lua.new_usertype<LuaHolder>("_Holder",
					"IsValid", &LuaHolder::IsValid,
					"AsBoolean", &LuaHolder::As<bool>,
					"AsInteger", &LuaHolder::As<int>,
					"AsFloat", &LuaHolder::As<float>,
					"AsString", &LuaHolder::As<char*>,
					"AsVector3", &LuaHolder::As<LuaVector3>);
				lua["Holder"] = sol::overload(Generate<LuaHolder>, Generate<LuaHolder, const sol::object&>);

				lua.new_usertype<LuaVector3>("_Vector3",
					"x", &LuaVector3::x,
					"y", &LuaVector3::y,
					"z", &LuaVector3::z);
				lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

				lua["_invoke"] = [fileName](const sol::this_state& lua, DWORD64 hash, LuaNativeReturnType returnType, const sol::variadic_args& args)
					{ return LuaInvoke(fileName, lua, hash, returnType, args); };
				lua["WAIT"] = WAIT;

				lua["GetAllPeds"] = GetAllPeds;
				lua["CreatePoolPed"] = CreatePoolPed;

				lua["GetAllVehicles"] = GetAllVehs;
				lua["CreatePoolVehicle"] = CreatePoolVehicle;
				lua["CreateTempVehicle"] = CreateTempVehicle;

				lua["GetAllProps"] = GetAllProps;
				lua["CreatePoolProp"] = CreatePoolProp;

				lua["GetAllWeapons"] = Memory::GetAllWeapons;
				lua["GetAllPedModels"] = Memory::GetAllPedModels;
				lua["GetAllVehicleModels"] = Memory::GetAllVehModels;

				const sol::protected_function_result& result = lua.safe_script_file(path.string(), sol::load_mode::text);

				if (!result.valid())
				{
					const sol::error& error = result;

					LuaPrint(fileName, error.what());
				}
				else
				{
					const sol::optional<sol::table>& scriptInfoOpt = lua["ScriptInfo"];

					if (scriptInfoOpt)
					{
						const sol::table& scriptInfo = *scriptInfoOpt;

						const sol::optional<std::string>& scriptNameOpt = scriptInfo["Name"];
						const sol::optional<std::string>& scriptIdOpt = scriptInfo["ScriptId"];

						if (scriptNameOpt && scriptIdOpt)
						{
							const std::string& scriptId = *scriptIdOpt;
							const std::string& scriptName = *scriptNameOpt;

							bool idAlreadyExists = s_registeredScripts.find(scriptId) != s_registeredScripts.end();

							if (!idAlreadyExists)
							{
								for (const auto& pair : g_effectsMap)
								{
									if (pair.second.Id == scriptId)
									{
										idAlreadyExists = true;

										break;
									}
								}
							}

							if (idAlreadyExists)
							{
								LOG("Could not register script \"" << fileName << "\" (with effect name \"" << scriptName << "\") as effect with id \"" << scriptId << "\" (already exists!)");
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

									if (timedTypeText == "None")
									{
										effectData.TimedType = EffectTimedType::TIMED_NOTTIMED;
									}
									else if (timedTypeText == "Normal")
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
										const sol::optional<int>& durationOpt = scriptInfo["CustomTime"];

										if (durationOpt && *durationOpt > 0)
										{
											effectData.TimedType = EffectTimedType::TIMED_CUSTOM;
											effectData.CustomTime = *durationOpt;
										}
									}
								}

								const sol::optional<int>& weightMultOpt = scriptInfo["WeightMultiplier"];

								if (weightMultOpt && *weightMultOpt > 0)
								{
									effectData.WeightMult = *weightMultOpt;
								}

								const sol::optional<bool>& isMetaOpt = scriptInfo["IsMeta"];

								if (isMetaOpt)
								{
									effectData.IsMeta = *isMetaOpt;
								}

								const sol::optional<sol::table>& incompatibleIdsOpt = scriptInfo["IncompatibleIds"];
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

								LOG("Registered script \"" << fileName << "\" as effect with id \"" << scriptId << "\" and name \"" << scriptName << "\"");
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
		const LuaScript& script = s_registeredScripts[scriptId];

		script.Execute(funcName);
	}
}