#include <stdafx.h>

#include "LuaScripts.h"

#include "Effects/Effect.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffectsMap.h"
#include "Effects/MetaModifiers.h"

#include "Memory/Hooks/ShaderHook.h"
#include "Memory/PedModels.h"
#include "Memory/Snow.h"
#include "Memory/Vehicle.h"
#include "Memory/WeaponPool.h"

#include "Util/EntityIterator.h"
#include "Util/File.h"
#include "Util/PoolSpawner.h"
#include "Util/Script.h"
#include "Util/Vehicle.h"

#if defined(_MSC_VER)
#define _LUAFUNC static __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#define _LUAFUNC __attribute__((always_inline)) static inline
#else
#define _LUAFUNC static inline
#endif

#define LUA_NATIVESDEF "chaosmod\\natives_def.lua"

static const std::vector<const char *> ms_rgScriptDirs { "chaosmod\\scripts", "chaosmod\\custom_scripts" };

_LUAFUNC void LuaPrint(const std::string &szText)
{
	COLOR_PREFIX_LOG("[Lua]", szText);
}

_LUAFUNC void LuaPrint(const std::string &szName, const std::string &szText)
{
	COLOR_PREFIX_LOG("[" << szName << "]", szText);
}

_LUAFUNC LONG WINAPI _TryParseExHandler(_EXCEPTION_POINTERS *pException)
{
	return pException->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER
	                                                                                : EXCEPTION_CONTINUE_SEARCH;
}

_LUAFUNC char *_TryParseString(void *pStr)
{
	auto exHandler = AddVectoredExceptionHandler(1, _TryParseExHandler);

	char *pcString = reinterpret_cast<char *>(pStr);

	// Access string to try to trigger a segfault
	for (char *c = pcString; *c; c++)
	{
	}

	RemoveVectoredContinueHandler(exHandler);

	return pcString;
}

_LUAFUNC bool _TryParseVector3(void **pVector, float &fX, float &fY, float &fZ)
{
	auto exHandler = AddVectoredExceptionHandler(1, _TryParseExHandler);

	fX             = *reinterpret_cast<float *>(pVector);
	fY             = *reinterpret_cast<float *>(pVector + 1);
	fZ             = *reinterpret_cast<float *>(pVector + 2);

	RemoveVectoredContinueHandler(exHandler);

	return true;
}

_LUAFUNC bool _CallNative(void ***ppResult)
{
	*ppResult = reinterpret_cast<void **>(nativeCall());

	return true;
}

template <typename T, typename... A> _LUAFUNC T Generate(const A &...args)
{
	return T(args...);
}

class LuaScript
{
  private:
	std::string m_szFileName;
	sol::state m_Lua;

  public:
	LuaScript(const std::string &fileName, sol::state &lua) : m_szFileName(fileName), m_Lua(std::move(lua))
	{
	}

	LuaScript(const LuaScript &) = delete;

	LuaScript &operator=(const LuaScript &) = delete;

	LuaScript(LuaScript &&script) noexcept
	    : m_szFileName(std::move(script.m_szFileName)), m_Lua(std::move(script.m_Lua))
	{
	}

	LuaScript &operator=(LuaScript &&script) noexcept
	{
		m_szFileName = std::move(script.m_szFileName);
		m_Lua        = std::move(script.m_Lua);
	}

	void Execute(const char *szFuncName) const
	{
		const sol::protected_function &func = m_Lua[szFuncName];
		if (!func.valid())
		{
			return;
		}

		const sol::protected_function_result &result = func();
		if (!result.valid())
		{
			const sol::error &error = result;

			LuaPrint(m_szFileName, error.what());
		}
	}
};

struct LuaVector3
{
	alignas(8) float m_fX = 0.f;
	alignas(8) float m_fY = 0.f;
	alignas(8) float m_fZ = 0.f;

	LuaVector3()          = default;

	LuaVector3(float fX, float fY, float fZ) : m_fX(fX), m_fY(fY), m_fZ(fZ)
	{
	}
};

class LuaHolder
{
  public:
	void *m_pData = nullptr;
	sol::object m_Obj;

	LuaHolder() = default;

	LuaHolder(const sol::object &obj) : m_Obj(obj)
	{
	}

	~LuaHolder()
	{
		if (m_Obj.valid())
		{
			m_Obj.abandon();
		}
	}

	template <typename T> __forceinline T As()
	{
		if constexpr (std::is_same<T, char *>())
		{
			return _TryParseString(m_pData);
		}
		else if constexpr (std::is_same<T, LuaVector3>())
		{
			float fX, fY, fZ;

			return _TryParseVector3(&m_pData, fX, fY, fZ) ? LuaVector3(fX, fY, fZ) : LuaVector3();
		}

		return *reinterpret_cast<T *>(&m_pData);
	}

	_NODISCARD __forceinline bool IsValid() const
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

_LUAFUNC sol::object LuaInvoke(const std::string &szFileName, const sol::this_state &lua, DWORD64 ullNativeHash,
                               ELuaNativeReturnType eReturnType, const sol::variadic_args &args)
{
	nativeInit(ullNativeHash);

	for (const sol::stack_proxy &arg : args)
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
		else if (arg.is<const char *>())
		{
			nativePush(arg.get<const char *>());
		}
		else if (arg.is<LuaHolder>())
		{
			LuaHolder &holder = arg.get<LuaHolder>();

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

	void **pReturned;
	if (!_CallNative(&pReturned))
	{
		LuaPrint(szFileName, (std::ostringstream()
		                      << "Error while invoking native 0x" << std::uppercase << std::hex << ullNativeHash)
		                         .str());
	}
	else if (pReturned)
	{
		void *returned = *pReturned;

		switch (eReturnType)
		{
		case ELuaNativeReturnType::Bool:
			return sol::make_object(lua, reinterpret_cast<bool>(returned));
		case ELuaNativeReturnType::Int:
			return sol::make_object(lua, reinterpret_cast<int>(returned));
		case ELuaNativeReturnType::Float:
			return sol::make_object(lua, *reinterpret_cast<float *>(&returned));
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

static void ParseScriptEntry(const std::filesystem::directory_entry &entry)
{
	const auto &path       = entry.path();
	const auto &szFileName = path.filename().string();

	const auto &szPathStr  = path.string();
	LOG("Running script " << szPathStr.substr(szPathStr.find_last_of("\\") + 1));

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::bit32);

	lua["ReturnType"] =
	    lua.create_table_with("None", ELuaNativeReturnType::None, "Boolean", ELuaNativeReturnType::Bool, "Integer",
	                          ELuaNativeReturnType::Int, "String", ELuaNativeReturnType::String, "Float",
	                          ELuaNativeReturnType::Float, "Vector3", ELuaNativeReturnType::Vector3);

	auto getMetaModFactory = []<typename T>(T &modifier)
	{
		return [&]()
		{
			return modifier;
		};
	};

	auto setMetaModFactory = []<typename T>(T &modifier)
	{
		return [&](T value)
		{
			modifier = value;
		};
	};

#define P(x) sol::property(getMetaModFactory(x), setMetaModFactory(x))

	auto metaModifiersTable     = lua.create_named_table("MetaModifiers");
	auto metaModifiersMetaTable = lua.create_table_with(
	    "EffectDurationModifier", P(MetaModifiers::m_fEffectDurationModifier), "TimerSpeedModifier",
	    P(MetaModifiers::m_fTimerSpeedModifier), "AdditionalEffectsToDispatch",
	    P(MetaModifiers::m_ucAdditionalEffectsToDispatch), "HideChaosUI", P(MetaModifiers::m_bHideChaosUI),
	    "DisableChaos", P(MetaModifiers::m_bDisableChaos), "FlipChaosUI", P(MetaModifiers::m_bFlipChaosUI));
	metaModifiersMetaTable[sol::meta_function::new_index] = [] {
	};
	metaModifiersMetaTable[sol::meta_function::index] = metaModifiersMetaTable;
	metaModifiersTable[sol::metatable_key]            = metaModifiersMetaTable;

#undef P

	if (DoesFileExist(LUA_NATIVESDEF))
	{
		lua.unsafe_script_file(LUA_NATIVESDEF);
	}

	lua["print"] = [szFileName](const std::string &szText)
	{
		LuaPrint(szFileName, szText);
	};
	lua["GetTickCount"] = GetTickCount64;
	lua["GET_HASH_KEY"] = GET_HASH_KEY;

	lua.new_usertype<LuaHolder>("_Holder", "IsValid", &LuaHolder::IsValid, "AsBoolean", &LuaHolder::As<bool>,
	                            "AsInteger", &LuaHolder::As<int>, "AsFloat", &LuaHolder::As<float>, "AsString",
	                            &LuaHolder::As<char *>, "AsVector3", &LuaHolder::As<LuaVector3>);
	lua["Holder"] = sol::overload(Generate<LuaHolder>, Generate<LuaHolder, const sol::object &>);

	lua.new_usertype<LuaVector3>("_Vector3", "x", &LuaVector3::m_fX, "y", &LuaVector3::m_fY, "z", &LuaVector3::m_fZ);
	lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

	lua["_invoke"] = [szFileName](const sol::this_state &lua, DWORD64 ullHash, ELuaNativeReturnType eReturnType,
	                              const sol::variadic_args &args)
	{
		return LuaInvoke(szFileName, lua, ullHash, eReturnType, args);
	};
	lua["WAIT"]                                 = WAIT;
	// Replace those natives with our own safe versions
	lua["APPLY_FORCE_TO_ENTITY"]                = APPLY_FORCE_TO_ENTITY;
	lua["APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS"] = APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS;

	lua["GetAllPeds"]                           = GetAllPedsArray;
	lua["CreatePoolPed"]                        = CreatePoolPed;

	lua["GetAllVehicles"]                       = GetAllVehsArray;
	lua["CreatePoolVehicle"]                    = CreatePoolVehicle;
	lua["CreateTempVehicle"]                    = CreateTempVehicle;

	lua["GetAllProps"]                          = GetAllPropsArray;
	lua["CreatePoolProp"]                       = CreatePoolProp;

	lua["GetAllWeapons"]                        = Memory::GetAllWeapons;
	lua["GetAllPedModels"]                      = Memory::GetAllPedModels;
	lua["GetAllVehicleModels"]                  = Memory::GetAllVehModels;

	lua.new_enum("EOverrideShaderType", "LensDistortion", EOverrideShaderType::LensDistortion, "Snow",
	             EOverrideShaderType::Snow);
	lua["OverrideShader"]   = Hooks::OverrideShader;
	lua["ResetShader"]      = Hooks::ResetShader;

	lua["SetSnowState"]     = Memory::SetSnow;

	lua["IsVehicleBraking"] = Memory::IsVehicleBraking;

	const auto &result      = lua.safe_script_file(path.string(), sol::load_mode::text);
	if (!result.valid())
	{
		const sol::error &error = result;
		LuaPrint(szFileName, error.what());

		return;
	}

	const sol::optional<sol::table> &effectGroupInfoOpt = lua["EffectGroupInfo"];
	if (effectGroupInfoOpt)
	{
		const auto &effectGroupInfo                    = *effectGroupInfoOpt;

		const sol::optional<std::string> &groupNameOpt = effectGroupInfo["Name"];
		if (groupNameOpt)
		{
			const auto &groupName = *groupNameOpt;
			const auto &result    = g_dictEffectGroups.find(groupName);
			if (result != g_dictEffectGroups.end() && !result->second.IsPlaceholder)
			{
				LOG(szFileName << ": WARNING: Could not register effect group \"" << groupName
				               << "\": Already registered!");
			}
			else
			{
				g_dictEffectGroups[groupName].IsPlaceholder         = false;
				g_dictEffectGroups[groupName].WasRegisteredByScript = true;
				g_dictEffectGroupMemberCount[groupName];

				const sol::optional<int> &groupWeightMultOpt = effectGroupInfo["WeightMultiplier"];
				if (groupWeightMultOpt)
				{
					g_dictEffectGroups[groupName].WeightMult =
					    std::clamp(*groupWeightMultOpt, 1, (int)(std::numeric_limits<unsigned short>::max)());
				}

				LOG(szFileName << ": Registered effect group \"" << groupName
				               << "\" with weight multiplier: " << g_dictEffectGroups[groupName].WeightMult);
			}
		}
	}

	const sol::optional<sol::table> &scriptInfoOpt = lua["ScriptInfo"];
	if (!scriptInfoOpt)
	{
		return;
	}

	const auto &scriptInfo                          = *scriptInfoOpt;

	const sol::optional<std::string> &scriptNameOpt = scriptInfo["Name"];
	const sol::optional<std::string> &scriptIdOpt   = scriptInfo["ScriptId"];
	if (!scriptNameOpt || !scriptIdOpt)
	{
		return;
	}

	auto trim = [](std::string str) -> std::string
	{
		if (str.find_first_not_of(' ') == str.npos)
		{
			return "";
		}

		str = str.substr(str.find_first_not_of(' '));
		str = str.substr(0, str.find_last_not_of(' ') == str.npos ? str.npos : str.find_last_not_of(' ') + 1);
		return str;
	};

	const auto &szScriptId = trim(*scriptIdOpt);
	if (szScriptId.empty())
	{
		// Id is empty
		return;
	}

	const auto &szScriptName = trim(*scriptNameOpt);

	bool bDoesIdAlreadyExist = ms_dictRegisteredScripts.find(szScriptId) != ms_dictRegisteredScripts.end();
	if (!bDoesIdAlreadyExist)
	{
		for (const auto &pair : g_dictEffectsMap)
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
		LOG(szFileName << ": ERROR: Could not register effect \"" << szScriptName << "\": Id \"" << szScriptId
		               << "\" already registered!");

		return;
	}

	EffectData effectData;
	effectData.Name                                    = szScriptName;
	effectData.Id                                      = *scriptIdOpt;

	const sol::optional<std::string> &timedTypeTextOpt = scriptInfo["TimedType"];
	if (timedTypeTextOpt)
	{
		const auto &szTimedTypeText = *timedTypeTextOpt;

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
			const sol::optional<int> &durationOpt = scriptInfo["CustomTime"];
			if (durationOpt)
			{
				effectData.TimedType  = EEffectTimedType::Custom;
				effectData.CustomTime = (std::max)(1, *durationOpt);
			}
		}
	}

	const sol::optional<int> &weightMultOpt = scriptInfo["WeightMultiplier"];
	if (weightMultOpt)
	{
		effectData.WeightMult = (std::max)(1, *weightMultOpt);
		effectData.Weight     = effectData.WeightMult;
	}

	const sol::optional<bool> &isMetaOpt = scriptInfo["IsMeta"];
	if (isMetaOpt)
	{
		effectData.SetAttribute(EEffectAttributes::IsMeta, *isMetaOpt);
	}

	const sol::optional<bool> &excludeFromVotingOpt = scriptInfo["ExcludeFromVoting"];
	if (excludeFromVotingOpt)
	{
		effectData.SetAttribute(EEffectAttributes::ExcludedFromVoting, *excludeFromVotingOpt);
	}

	const sol::optional<bool> &isUtilityOpt = scriptInfo["IsUtility"];
	if (isUtilityOpt)
	{
		effectData.SetAttribute(EEffectAttributes::IsUtility, *isUtilityOpt);
	}

	const sol::optional<sol::table> &incompatibleIdsOpt = scriptInfo["IncompatibleIds"];
	if (incompatibleIdsOpt)
	{
		const auto &rgIncompatibleIds = *incompatibleIdsOpt;
		for (const auto &entry : rgIncompatibleIds)
		{
			if (entry.second.valid() && entry.second.is<std::string>())
			{
				effectData.IncompatibleIds.push_back(entry.second.as<std::string>());
			}
		}
	}

	const sol::optional<std::string> &effectCategoryOpt = scriptInfo["EffectCategory"];
	if (effectCategoryOpt)
	{
		const auto &effectCategoryStr = *effectCategoryOpt;
		auto effectCategoryIt         = g_dictNameToEffectCategory.find(effectCategoryStr);
		if (effectCategoryIt != g_dictNameToEffectCategory.end())
		{
			effectData.EffectCategory = effectCategoryIt->second;
		}
	}

	const sol::optional<std::string> &effectGroupOpt = scriptInfo["EffectGroup"];
	if (effectGroupOpt)
	{
		const auto &effectGroup = *effectGroupOpt;
		effectData.GroupType    = effectGroup;

		if (g_dictEffectGroups.find(effectGroup) == g_dictEffectGroups.end())
		{
			g_dictEffectGroups[effectGroup] = { .IsPlaceholder = true, .WasRegisteredByScript = true };
		}

		g_dictEffectGroupMemberCount[effectGroup]++;
	}

	const sol::optional<int> &shortcutKeycodeOpt = scriptInfo["ShortcutKeycode"];
	if (shortcutKeycodeOpt)
	{
		int shortcutKeycode = *shortcutKeycodeOpt;
		if (shortcutKeycode > 0 && shortcutKeycode < 255)
		{
			effectData.ShortcutKeycode = shortcutKeycode;
		}
	}

	ms_dictRegisteredScripts.emplace(szScriptId, LuaScript(szFileName, lua));
	g_dictEnabledEffects.emplace(szScriptId, effectData);
	g_RegisteredEffects.emplace_back(szScriptId);

	LOG(szFileName << ": Registered effect \"" << szScriptName << "\" with id \"" << szScriptId << "\"");
}

namespace LuaScripts
{
	void Load()
	{
		ms_dictRegisteredScripts.clear();

		ClearRegisteredScriptEffects();

		for (auto &dir : ms_rgScriptDirs)
		{
			if (!DoesFileExist(dir))
			{
				continue;
			}

			for (const auto &entry : std::filesystem::recursive_directory_iterator(dir))
			{
				if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() == ".lua"
				    && entry.file_size() > 0)
				{
					ParseScriptEntry(entry);
				}
			}
		}
	}

	void Unload()
	{
		// Clean up all effect groups registered by scripts
		for (auto it = g_dictEffectGroups.begin(); it != g_dictEffectGroups.end();)
		{
			const auto &[groupName, groupData] = *it;
			if (groupData.WasRegisteredByScript)
			{
				it = g_dictEffectGroups.erase(it);
				g_dictEffectGroupMemberCount.erase(groupName);
			}
			else
			{
				it++;
			}
		}
	}

	_NODISCARD std::vector<std::string> GetScriptIds()
	{
		std::vector<std::string> rgScriptIds;

		for (const auto &pair : ms_dictRegisteredScripts)
		{
			rgScriptIds.push_back(pair.first);
		}

		return rgScriptIds;
	}

	void Execute(const std::string &szScriptId, const char *szFuncName)
	{
		const LuaScript &script = ms_dictRegisteredScripts.at(szScriptId);

		script.Execute(szFuncName);
	}
}