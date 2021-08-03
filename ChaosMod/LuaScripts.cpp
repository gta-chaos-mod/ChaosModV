#include <stdafx.h>

#include "LuaScripts.h"

#define _LUAFUNC static __forceinline

#define LUA_SCRIPTS_DIR "chaosmod\\custom_scripts"
#define LUA_NATIVESDEF_DIR "chaosmod\\natives_def.lua"

_LUAFUNC void LuaPrint(const std::string& szText)
{
	COLOR_PREFIX_LOG("[Lua]", szText);
}

_LUAFUNC void LuaPrint(const std::string& szName, const std::string& szText)
{
	COLOR_PREFIX_LOG("[" << szName << "]", szText);
}

_LUAFUNC char* _TryParseString(void* pStr)
{
	__try
	{
		char* pcString = reinterpret_cast<char*>(pStr);

		// Access string to try to trigger a segfault
		for (char* c = pcString; *c; c++)
		{

		}

		return pcString;
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return nullptr;
	}
}

_LUAFUNC bool _TryParseVector3(void** pVector, float& fX, float& fY, float& fZ)
{
	__try
	{
		fX = *reinterpret_cast<float*>(pVector);
		fY = *reinterpret_cast<float*>(pVector + 1);
		fZ = *reinterpret_cast<float*>(pVector + 2);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return false;
	}

	return true;
}

_LUAFUNC bool _CallNative(void*** ppResult)
{
	__try
	{
		*ppResult = reinterpret_cast<void**>(nativeCall());
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

template <typename T, typename... A>
_LUAFUNC T Generate(const A&... args)
{
	return T(args...);
}

class LuaScript
{
private:
	std::string m_szFileName;
	sol::state m_Lua;

public:
	LuaScript(const std::string& fileName, sol::state& lua) : m_szFileName(fileName), m_Lua(std::move(lua))
	{

	}

	LuaScript(const LuaScript&) = delete;

	LuaScript& operator=(const LuaScript&) = delete;

	LuaScript(LuaScript&& script) noexcept : m_szFileName(std::move(script.m_szFileName)), m_Lua(std::move(script.m_Lua))
	{

	}

	LuaScript& operator=(LuaScript&& script) noexcept
	{
		m_szFileName = std::move(script.m_szFileName);
		m_Lua = std::move(script.m_Lua);
	}

	void Execute(const char* szFuncName) const
	{
		const sol::protected_function& func = m_Lua[szFuncName];
		if (!func.valid())
		{
			return;
		}

		const sol::protected_function_result& result = func();
		if (!result.valid())
		{
			const sol::error& error = result;

			LuaPrint(m_szFileName, error.what());
		}
	}
};

struct LuaVector3
{
	alignas(8) float m_fX = 0.f;
	alignas(8) float m_fY = 0.f;
	alignas(8) float m_fZ = 0.f;

	LuaVector3() = default;

	LuaVector3(float fX, float fY, float fZ) : m_fX(fX), m_fY(fY), m_fZ(fZ)
	{

	}
};

class LuaHolder
{
public:
	void* m_pData = nullptr;
	sol::object m_Obj;

	LuaHolder() = default;

	LuaHolder(const sol::object& obj) : m_Obj(obj)
	{
		
	}

	~LuaHolder()
	{
		if (m_Obj.valid())
		{
			m_Obj.abandon();
		}
	}

	template <typename T>
	__forceinline T As()
	{
		if constexpr (std::is_same<T, char*>())
		{
			return _TryParseString(m_pData);
		}
		else if constexpr (std::is_same<T, LuaVector3>())
		{
			float fX, fY, fZ;

			return _TryParseVector3(&m_pData, fX, fY, fZ) ? LuaVector3(fX, fY, fZ) : LuaVector3();
		}

		return *reinterpret_cast<T*>(&m_pData);
	}

	__forceinline _NODISCARD bool IsValid() const
	{
		return m_pData || m_Obj.valid();
	}
};

enum class ELuaNativeReturnType
{
	None,
	Bool,
	Int,
	Float,
	String,
	Vector3
};

static std::unordered_map<std::string, LuaScript> ms_dictRegisteredScripts;

_LUAFUNC sol::object LuaInvoke(const std::string& szFileName, const sol::this_state& lua, DWORD64 ullNativeHash, ELuaNativeReturnType eReturnType, const sol::variadic_args& args)
{
	nativeInit(ullNativeHash);

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

			if (holder.m_Obj.valid())
			{
				DWORD64 data = holder.m_Obj.as<DWORD64>();

				nativePush(&data);
			}
			else
			{
				nativePush(&holder.m_pData);
			}
		}
	}

	void** pReturned;
	if (!_CallNative(&pReturned))
	{
		LuaPrint(szFileName, (std::ostringstream() << "Error while invoking native 0x" << std::uppercase << std::hex << ullNativeHash).str());
	}
	else if (pReturned)
	{
		void* returned = *pReturned;

		switch (eReturnType)
		{
		case ELuaNativeReturnType::Bool:
			return sol::make_object(lua, reinterpret_cast<bool>(returned));
		case ELuaNativeReturnType::Int:
			return sol::make_object(lua, reinterpret_cast<int>(returned));
		case ELuaNativeReturnType::Float:
			return sol::make_object(lua, *reinterpret_cast<float*>(&returned));
		case ELuaNativeReturnType::String:
			return sol::make_object(lua, _TryParseString(returned));
		case ELuaNativeReturnType::Vector3:
		{
			LuaVector3 vector3;
			if (_TryParseVector3(pReturned, vector3.m_fX, vector3.m_fY, vector3.m_fZ))
			{
				return sol::make_object(lua, vector3);
			}
		}

		break;
		}
	}

	return sol::make_object(lua, sol::lua_nil);
}

namespace LuaScripts
{
	void Load()
	{
		ms_dictRegisteredScripts.clear();

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
				const std::string& szFileName = path.filename().string();

				const std::string& szPathStr = path.string();
				LOG("Running script " << szPathStr.substr(strlen(LUA_SCRIPTS_DIR) + 1));

				sol::state lua;
				lua.open_libraries(sol::lib::base);
				lua.open_libraries(sol::lib::math);
				lua.open_libraries(sol::lib::table);
				lua.open_libraries(sol::lib::string);
				lua.open_libraries(sol::lib::bit32);

				lua["ReturnType"] = lua.create_table_with(
					"None", ELuaNativeReturnType::None,
					"Boolean", ELuaNativeReturnType::Bool,
					"Integer", ELuaNativeReturnType::Int,
					"String", ELuaNativeReturnType::String,
					"Float", ELuaNativeReturnType::Float,
					"Vector3", ELuaNativeReturnType::Vector3
				);

				if (DoesFileExist(LUA_NATIVESDEF_DIR))
				{
					lua.unsafe_script_file(LUA_NATIVESDEF_DIR);
				}

				lua["print"] = [szFileName](const std::string& szText) { LuaPrint(szFileName, szText); };
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
					"x", &LuaVector3::m_fX,
					"y", &LuaVector3::m_fY,
					"z", &LuaVector3::m_fZ);
				lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

				lua["_invoke"] = [szFileName](const sol::this_state& lua, DWORD64 ullHash, ELuaNativeReturnType eReturnType, const sol::variadic_args& args)
					{ return LuaInvoke(szFileName, lua, ullHash, eReturnType, args); };
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

					LuaPrint(szFileName, error.what());
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
							const std::string& szScriptId = *scriptIdOpt;
							const std::string& szScriptName = *scriptNameOpt;

							bool bDoesIdAlreadyExist = ms_dictRegisteredScripts.find(szScriptId) != ms_dictRegisteredScripts.end();

							if (!bDoesIdAlreadyExist)
							{
								for (const auto& pair : g_dictEffectsMap)
								{
									if (pair.second.Id == szScriptId)
									{
										bDoesIdAlreadyExist = true;

										break;
									}
								}
							}

							if (bDoesIdAlreadyExist)
							{
								LOG("Could not register script \"" << szFileName << "\" (with effect name \"" << szScriptName << "\") as effect with id \"" << szScriptId << "\" (already exists!)");
							}
							else
							{
								EffectData effectData;
								effectData.Name = szScriptName;
								effectData.Id = *scriptIdOpt;

								sol::optional<std::string> timedTypeTextOpt = scriptInfo["TimedType"];

								if (timedTypeTextOpt)
								{
									const std::string& szTimedTypeText = *timedTypeTextOpt;

									if (szTimedTypeText == "None")
									{
										effectData.TimedType = EEffectTimedType::NotTimed;
									}
									else if (szTimedTypeText == "Normal")
									{
										effectData.TimedType = EEffectTimedType::Normal;
									}
									else if (szTimedTypeText == "Short")
									{
										effectData.TimedType = EEffectTimedType::Short;
									}
									else if (szTimedTypeText == "Permanent")
									{
										effectData.TimedType = EEffectTimedType::Permanent;
									}
									else if (szTimedTypeText == "Custom")
									{
										const sol::optional<int>& durationOpt = scriptInfo["CustomTime"];

										if (durationOpt && *durationOpt > 0)
										{
											effectData.TimedType = EEffectTimedType::Custom;
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
									const sol::table& rgIncompatibleIds = *incompatibleIdsOpt;

									for (const auto& entry : rgIncompatibleIds)
									{
										if (entry.second.valid() && entry.second.is<std::string>())
										{
											effectData.IncompatibleIds.push_back(entry.second.as<std::string>());
										}
									}
								}

								ms_dictRegisteredScripts.emplace(szScriptId, LuaScript(szFileName, lua));

								g_EnabledEffects.emplace(szScriptId, effectData);

								g_RegisteredEffects.emplace_back(szScriptId);

								LOG("Registered script \"" << szFileName << "\" as effect with id \"" << szScriptId << "\" and name \"" << szScriptName << "\"");
							}
						}
					}
				}
			}
		}
	}

	_NODISCARD std::vector<std::string> GetScriptIds()
	{
		std::vector<std::string> rgScriptIds;

		for (const auto& pair : ms_dictRegisteredScripts)
		{
			rgScriptIds.push_back(pair.first);
		}

		return rgScriptIds;
	}

	void Execute(const std::string& szScriptId, const char* szFuncName)
	{
		const LuaScript& script = ms_dictRegisteredScripts.at(szScriptId);

		script.Execute(szFuncName);
	}
}