#include <stdafx.h>

#include "Info.h"
#include "LuaScripts.h"

#include "Components/DebugSocket.h"
#include "Components/EffectDispatcher.h"

#include "Effects/Effect.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffectsMap.h"
#include "Effects/MetaModifiers.h"

#include "Memory/Hooks/AudioClearnessHook.h"
#include "Memory/Hooks/AudioPitchHook.h"
#include "Memory/Hooks/ShaderHook.h"
#include "Memory/PedModels.h"
#include "Memory/Snow.h"
#include "Memory/Vehicle.h"
#include "Memory/WeaponPool.h"

#include "Util/Camera.h"
#include "Util/EntityIterator.h"
#include "Util/File.h"
#include "Util/Peds.h"
#include "Util/Player.h"
#include "Util/PoolSpawner.h"
#include "Util/Script.h"
#include "Util/Types.h"
#include "Util/Vehicle.h"
#include "Util/Weapon.h"

#define LUA_NATIVESDEF "chaosmod\\natives_def.lua"

#ifdef _MSC_VER
#define _LUAFUNC static __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#define _LUAFUNC __attribute__((always_inline)) static inline
#else
#define _LUAFUNC static inline
#endif

// MinGW doesn't have SEH :(
#ifdef _MSC_VER
#define MAGIC_CATCH_BEGIN \
	__try                 \
	{
#define MAGIC_CATCH_END(x)               \
	}                                    \
	__except (EXCEPTION_EXECUTE_HANDLER) \
	{                                    \
		x;                               \
	}
#else
#define MAGIC_CATCH_BEGIN {
#define MAGIC_CATCH_END(x) }
#endif

#define LUA_LOG(text)                                   \
	do                                                  \
	{                                                   \
		LuaPrint((std::ostringstream() << text).str()); \
	} while (0);
#define LUA_SCRIPT_LOG(scriptName, text)                            \
	do                                                              \
	{                                                               \
		LuaPrint(scriptName, (std::ostringstream() << text).str()); \
	} while (0);

static const std::vector<const char *> ms_rgScriptDirs { "chaosmod\\scripts", "chaosmod\\workshop", "chaosmod\\custom_scripts" };

_LUAFUNC void LuaPrint(const std::string &szText)
{
	COLOR_PREFIX_LOG("(Lua)", szText);
}

_LUAFUNC void LuaPrint(const std::string &szName, const std::string &szText)
{
	COLOR_PREFIX_LOG("(" << szName << ")", szText);

#ifdef WITH_DEBUG_PANEL_SUPPORT
	if (ComponentExists<DebugSocket>())
	{
		GetComponent<DebugSocket>()->ScriptLog(szName, szText);
	}
#endif
}

_LUAFUNC char *_TryParseString(void *pStr)
{
	char *pcString = reinterpret_cast<char *>(pStr);

	MAGIC_CATCH_BEGIN
	// Access string to try to trigger an access violation
	for (char *c = pcString; *c; c++)
	{
	}
	MAGIC_CATCH_END(return nullptr)

	return pcString;
}

_LUAFUNC bool _TryParseVector3(void **pVector, float &fX, float &fY, float &fZ)
{
	MAGIC_CATCH_BEGIN
	fX = *reinterpret_cast<float *>(pVector);
	fY = *reinterpret_cast<float *>(pVector + 1);
	fZ = *reinterpret_cast<float *>(pVector + 2);
	MAGIC_CATCH_END(return false)

	return true;
}

_LUAFUNC bool _CallNative(void ***ppResult)
{
	MAGIC_CATCH_BEGIN
	*ppResult = reinterpret_cast<void **>(nativeCall());
	MAGIC_CATCH_END(return false)

	return true;
}

template <typename T, typename... A> _LUAFUNC T Generate(const A &...args)
{
	return T(args...);
}

class LuaScript
{
  private:
	std::string m_ScriptName;
	sol::state m_Lua;
	bool m_bTemporary;

  public:
	LuaScript(const std::string &scriptName, sol::state &lua, bool temporary)
	    : m_ScriptName(scriptName), m_Lua(std::move(lua)), m_bTemporary(temporary)
	{
	}

	LuaScript(const LuaScript &)            = delete;

	LuaScript &operator=(const LuaScript &) = delete;

	LuaScript(LuaScript &&script) noexcept
	    : m_ScriptName(std::move(script.m_ScriptName)),
	      m_Lua(std::move(script.m_Lua)),
	      m_bTemporary(script.m_bTemporary)
	{
	}

	LuaScript &operator=(LuaScript &&script) noexcept
	{
		m_ScriptName = std::move(script.m_ScriptName);
		m_Lua        = std::move(script.m_Lua);
		m_bTemporary = script.m_bTemporary;

		return *this;
	}

	const std::string &GetScriptName() const
	{
		return m_ScriptName;
	}

	bool IsTemporary() const
	{
		return m_bTemporary;
	}

	void Execute(const char *funcName) const
	{
		const sol::protected_function &func = m_Lua[funcName];
		if (!func.valid())
		{
			return;
		}

		const sol::protected_function_result &result = func();
		if (!result.valid())
		{
			const sol::error &error = result;

			LuaPrint(m_ScriptName, error.what());
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

	__forceinline bool IsValid() const
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

static std::unordered_map<std::string, LuaScript> ms_dictRegisteredEffects;

_LUAFUNC sol::object LuaInvoke(const std::string &scriptName, const sol::this_state &lua, DWORD64 ullNativeHash,
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
		LuaPrint(scriptName, (std::ostringstream()
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

static void RemoveScriptEntry(const std::string &effectId)
{
	ms_dictRegisteredEffects.erase(effectId);
	g_dictEnabledEffects.erase(effectId);

	auto result = std::find(g_RegisteredEffects.begin(), g_RegisteredEffects.end(), effectId);
	if (result != g_RegisteredEffects.end())
	{
		g_RegisteredEffects.erase(result);
	}
}

enum ParseScriptFlags
{
	// Immediately dispatch effect (if script registers one) and remove it OnStop
	ParseScript_IsTemporary = (1 << 0),
};
static void ParseScriptRaw(std::string scriptName, std::string_view script, ParseScriptFlags flags = {})
{
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

	lua["print"] = [scriptName](const std::string &szText)
	{
		LuaPrint(scriptName, szText);
	};
	lua["GetTickCount"] = GetTickCount64;
	lua["GET_HASH_KEY"] = GET_HASH_KEY;

	lua.new_usertype<LuaHolder>("_Holder", "IsValid", &LuaHolder::IsValid, "AsBoolean", &LuaHolder::As<bool>,
	                            "AsInteger", &LuaHolder::As<int>, "AsFloat", &LuaHolder::As<float>, "AsString",
	                            &LuaHolder::As<char *>, "AsVector3", &LuaHolder::As<LuaVector3>);
	lua["Holder"] = sol::overload(Generate<LuaHolder>, Generate<LuaHolder, const sol::object &>);

	lua.new_usertype<LuaVector3>("_Vector3", "x", &LuaVector3::m_fX, "y", &LuaVector3::m_fY, "z", &LuaVector3::m_fZ);
	lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

	lua["_invoke"] = [scriptName](const sol::this_state &lua, DWORD64 ullHash, ELuaNativeReturnType eReturnType,
	                              const sol::variadic_args &args)
	{
		return LuaInvoke(scriptName, lua, ullHash, eReturnType, args);
	};
	lua["WAIT"]                                 = WAIT;
	// Replace those natives with our own safe versions
	lua["APPLY_FORCE_TO_ENTITY"]                = APPLY_FORCE_TO_ENTITY;
	lua["APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS"] = APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS;

	lua["LoadModel"]                            = LoadModel;

	lua["GetAllPeds"]                           = GetAllPedsArray;
	lua["CreatePoolPed"]                        = CreatePoolPed;

	lua["GetAllVehicles"]                       = GetAllVehsArray;
	lua["CreatePoolVehicle"]                    = CreatePoolVehicle;
	lua["CreateTempVehicle"]                    = CreateTempVehicle;
	lua["CreateTempVehicleOnPlayerPos"]         = CreateTempVehicleOnPlayerPos;
	lua["SetSurroundingPedsInVehicles"]         = SetSurroundingPedsInVehicles;
	lua["ReplaceVehicle"]                       = ReplaceVehicle;

	lua["GetAllProps"]                          = GetAllPropsArray;
	lua["CreatePoolProp"]                       = CreatePoolProp;

	lua["GetAllWeapons"]                        = Memory::GetAllWeapons;
	lua["GetAllPedModels"]                      = Memory::GetAllPedModels;
	lua["GetAllVehicleModels"]                  = Memory::GetAllVehModels;

	lua.new_enum("EOverrideShaderType", "LensDistortion", EOverrideShaderType::LensDistortion, "Snow",
	             EOverrideShaderType::Snow);
	lua["OverrideShader"]           = Hooks::OverrideShader;
	lua["ResetShader"]              = Hooks::ResetShader;

	lua["SetSnowState"]             = Memory::SetSnow;

	lua["IsVehicleBraking"]         = Memory::IsVehicleBraking;

	lua["EnableScriptThreadBlock"]  = Hooks::EnableScriptThreadBlock;
	lua["DisableScriptThreadBlock"] = Hooks::DisableScriptThreadBlock;
	
	lua["SetAudioPitch"]                           = Hooks::SetAudioPitch;
	lua["ResetAudioPitch"]                         = Hooks::ResetAudioPitch;
	lua["SetAudioClearness"]                       = Hooks::SetAudioClearness;
	lua["ResetAudioClearness"]                     = Hooks::ResetAudioClearness;

	lua["GetGameplayCamOffsetInWorldCoords"] = [](LuaVector3 vOffset)
	{
		Vector3 vReturn =
		    Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(vOffset.m_fX, vOffset.m_fY, vOffset.m_fZ));
		return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
	};
	lua["GetCoordsFromGameplayCam"] = [](float fDistance)
	{
		Vector3 vReturn = Util::GetCoordsFromGameplayCam(fDistance);
		return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
	};

	lua["GetCoordAround"] = [](Entity iEntity, float fAngle, float fRadius, float fZOffset, bool bRelative)
	{
		Vector3 vReturn = GetCoordAround(iEntity, fAngle, fRadius, fZOffset, bRelative);
		return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
	};

	lua["IsWeaponShotgun"]                   = Util::IsWeaponShotgun;

	lua["GetChaosModVersion"]       = []()
	{
		return MOD_VERSION;
	};
	lua["GetGameBuild"] = Memory::GetGameBuild;

	const auto &result  = lua.safe_script(script);
	if (!result.valid())
	{
		const sol::error &error = result;
		LuaPrint(scriptName, error.what());

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

	const sol::optional<sol::table> &effectGroupInfoOpt = lua["EffectGroupInfo"];
	if (effectGroupInfoOpt)
	{
		const auto &effectGroupInfo                    = *effectGroupInfoOpt;

		const sol::optional<std::string> &groupNameOpt = effectGroupInfo["Name"];
		if (!groupNameOpt)
		{
			LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect group: Missing Name!");
		}
		else
		{
			const auto &groupName = trim(*groupNameOpt);

			if (!(flags & ParseScript_IsTemporary))
			{
				const auto &result = g_dictEffectGroups.find(groupName);
				if (result != g_dictEffectGroups.end() && !result->second.IsPlaceholder)
				{
					LUA_SCRIPT_LOG(scriptName, "WARNING: Could not register effect group \""
					                               << groupName << "\": Already registered!");
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

					LUA_SCRIPT_LOG(scriptName, "Registered effect group \""
					                               << groupName << "\" with weight multiplier: "
					                               << g_dictEffectGroups[groupName].WeightMult);
				}
			}
		}
	}
	sol::optional<sol::table> effectInfoOpt = lua["EffectInfo"];
	if (!effectInfoOpt)
	{
		// Backwards compatibility
		effectInfoOpt = lua["ScriptInfo"].get<sol::optional<sol::table>>();
		if (!effectInfoOpt)
		{
			return;
		}
	}

	const auto &effectInfo                          = *effectInfoOpt;

	const sol::optional<std::string> &effectNameOpt = effectInfo["Name"];
	if (!effectNameOpt)
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect: Missing Name!");
		return;
	}
	const auto &effectName = trim(*effectNameOpt);
	if (effectName.empty())
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect: Invalid Name!");
		return;
	}

	sol::optional<std::string> effectIdOpt = effectInfo["EffectId"];
	if (!effectIdOpt)
	{
		// Also backwards compat
		effectIdOpt = effectInfo["ScriptId"].get<sol::optional<std::string>>();
		if (!effectIdOpt)
		{
			LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": Missing EffectId!");
			return;
		}
	}
	const auto &effectId = trim(*effectIdOpt);
	if (effectId.empty() || effectId.starts_with('.'))
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": Invalid EffectId!");
		return;
	}

	{
		auto result              = ms_dictRegisteredEffects.find(effectId);
		bool bDoesIdAlreadyExist = result != ms_dictRegisteredEffects.end();
		if (bDoesIdAlreadyExist)
		{
			if (result->second.IsTemporary())
			{
				/* Replace existing temporary effect with this one */

				if (ComponentExists<EffectDispatcher>())
				{
					GetComponent<EffectDispatcher>()->ClearEffect(effectId);
				}

				RemoveScriptEntry(effectId);

				bDoesIdAlreadyExist = false;
			}
		}
		else
		{
			for (const auto &effect : g_dictEffectsMap)
			{
				if (effect.second.Id == effectId)
				{
					bDoesIdAlreadyExist = true;

					break;
				}
			}
		}
		if (bDoesIdAlreadyExist)
		{
			LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": EffectId \""
			                                                                 << effectId << "\" already exists!");

			return;
		}
	}

	EffectData effectData;
	effectData.Name                                    = effectName;
	effectData.Id                                      = *effectIdOpt;

	const sol::optional<std::string> &timedTypeTextOpt = effectInfo["TimedType"];
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

			if (flags & ParseScript_IsTemporary)
			{
				LUA_SCRIPT_LOG(scriptName, "ERROR: TimedType \"Permanent\" for effect \""
				                               << effectName
				                               << " is invalid for temporary effects, please use another TimedType!");
			}
		}
		else if (szTimedTypeText == "Custom")
		{
			const sol::optional<int> &durationOpt = effectInfo["CustomTime"];
			if (!durationOpt)
			{
				LUA_SCRIPT_LOG(scriptName, "WARNING: TimedType \"Custom\" for effect \""
				                               << effectName
				                               << " but no CustomTime defined? Falling back to \"Normal\" TimedType!");
				effectData.TimedType = EEffectTimedType::Normal;
			}
			else
			{
				effectData.TimedType  = EEffectTimedType::Custom;
				effectData.CustomTime = (std::max)(1, *durationOpt);
			}
		}
		else
		{
			LUA_SCRIPT_LOG(scriptName, "WARNING: Unknown TimedType \""
			                               << szTimedTypeText << "\" specified for effect \"" << effectName << "\"!");
		}
	}

	const sol::optional<int> &weightMultOpt = effectInfo["WeightMultiplier"];
	if (weightMultOpt)
	{
		effectData.WeightMult = (std::max)(1, *weightMultOpt);
		effectData.Weight     = effectData.WeightMult;
	}

	const sol::optional<bool> &isMetaOpt = effectInfo["IsMeta"];
	if (isMetaOpt)
	{
		effectData.SetAttribute(EEffectAttributes::IsMeta, *isMetaOpt);
	}

	const sol::optional<bool> &excludeFromVotingOpt = effectInfo["ExcludeFromVoting"];
	if (excludeFromVotingOpt)
	{
		effectData.SetAttribute(EEffectAttributes::ExcludedFromVoting, *excludeFromVotingOpt);
	}

	const sol::optional<bool> &isUtilityOpt = effectInfo["IsUtility"];
	if (isUtilityOpt)
	{
		effectData.SetAttribute(EEffectAttributes::IsUtility, *isUtilityOpt);
	}

	const sol::optional<sol::table> &incompatibleIdsOpt = effectInfo["IncompatibleIds"];
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

	const sol::optional<std::string> &effectCategoryOpt = effectInfo["EffectCategory"];
	if (effectCategoryOpt)
	{
		const auto &effectCategoryStr = *effectCategoryOpt;
		auto effectCategoryIt         = g_dictNameToEffectCategory.find(effectCategoryStr);
		if (effectCategoryIt != g_dictNameToEffectCategory.end())
		{
			effectData.EffectCategory = effectCategoryIt->second;
		}
	}

	const sol::optional<std::string> &effectGroupOpt = effectInfo["EffectGroup"];
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

	const sol::optional<int> &shortcutKeycodeOpt = effectInfo["ShortcutKeycode"];
	if (shortcutKeycodeOpt)
	{
		int shortcutKeycode = *shortcutKeycodeOpt;
		if (shortcutKeycode > 0 && shortcutKeycode < 255)
		{
			effectData.ShortcutKeycode = shortcutKeycode;
		}
	}

	// Exclude temporary effects from choices pool
	effectData.SetAttribute(EEffectAttributes::IsTemporary, flags & ParseScript_IsTemporary);

	ms_dictRegisteredEffects.emplace(effectId, LuaScript(scriptName, lua, flags & ParseScript_IsTemporary));
	g_dictEnabledEffects.emplace(effectId, effectData);
	g_RegisteredEffects.emplace_back(effectId);

	if (flags & ParseScript_IsTemporary)
	{
		// Immediately dispatch it too
		if (ComponentExists<EffectDispatcher>())
		{
			GetComponent<EffectDispatcher>()->DispatchEffect(effectId, nullptr, false);
		}
	}
	else
	{
		LUA_SCRIPT_LOG(scriptName, "Registered effect \"" << effectName << "\" with id \"" << effectId << "\"");
	}
}

static void ParseScriptEntry(const std::filesystem::directory_entry &entry)
{
	const auto &path     = entry.path();
	const auto &fileName = path.filename().string();
	const auto &pathStr  = path.string();

	// Don't print first two directories (e.g. chaosmod\scripts\)
	LUA_LOG("Running script " << pathStr.substr(pathStr.find("\\", 9) + 1));

	std::ifstream fileStream(path.c_str());
	std::stringstream buffer;
	buffer << fileStream.rdbuf();

	ParseScriptRaw(fileName, buffer.str());
}

namespace LuaScripts
{
	void Load()
	{
		ms_dictRegisteredEffects.clear();

		ClearRegisteredScriptEffects();

		for (auto dir : ms_rgScriptDirs)
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

	void Execute(const std::string &effectId, ExecuteFuncType funcType)
	{
		const auto &script = ms_dictRegisteredEffects.at(effectId);

		switch (funcType)
		{
		case ExecuteFuncType::Start:
			script.Execute("OnStart");
			break;
		case ExecuteFuncType::Stop:
			script.Execute("OnStop");

			// Yes, OnStop also gets called on non-timed effects
			if (script.IsTemporary())
			{
				RemoveScriptEntry(effectId);
			}

			break;
		case ExecuteFuncType::Tick:
			script.Execute("OnTick");
			break;
		}
	}

	void RegisterScriptRawTemporary(std::string scriptName, std::string script)
	{
		ParseScriptRaw(scriptName, script, ParseScript_IsTemporary);
	}
}
