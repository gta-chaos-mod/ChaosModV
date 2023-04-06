#include <stdafx.h>

#include "Info.h"
#include "LuaScripts.h"

#include "Components/DebugSocket.h"
#include "Components/EffectDispatcher.h"
#include "Components/KeyStates.h"

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
#include "Util/Workshop.h"

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
		std::lock_guard lock(ms_PrintMutex);            \
		LuaPrint((std::ostringstream() << text).str()); \
	} while (0)
static std::mutex ms_PrintMutex;
#define LUA_SCRIPT_LOG(scriptName, text)                            \
	do                                                              \
	{                                                               \
		std::lock_guard lock(ms_PrintMutex);                        \
		LuaPrint(scriptName, (std::ostringstream() << text).str()); \
	} while (0)

static const std::vector<const char *> ms_rgScriptDirs { "chaosmod\\scripts", "chaosmod\\workshop",
	                                                     "chaosmod\\custom_scripts" };

static std::string ms_NativesDefCache;

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
	if (ullNativeHash == 0x213AEB2B90CBA7AC || ullNativeHash == 0x5A5F40FE637EB584
	    || ullNativeHash == 0x933D6A9EEC1BACD0 || ullNativeHash == 0xE80492A9AC099A93
	    || ullNativeHash == 0x8EF07E15701D61ED)
	{
		return sol::make_object(lua, sol::lua_nil);
	}

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
	ParseScript_None,
	// Immediately dispatch effect (if script registers one) and remove it OnStop
	// Assumes this is only being called from the main thread!
	ParseScript_IsTemporary   = (1 << 0),
	// Whether this is called from an alien thread, aborts and returns ParseScriptReturnReason::Error_ThreadUnsafe if
	// thread-unsafe function was called
	ParseScript_IsAlienThread = (1 << 1),
};
enum class ParseScriptReturnReason
{
	Success,
	Error,
	Error_ThreadUnsafe
};
static ParseScriptReturnReason ParseScriptRaw(std::string scriptName, std::string_view script,
                                              ParseScriptFlags flags = ParseScript_None)
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

	auto metaModifiersTable = lua.create_named_table("MetaModifiers");
#define P(x) sol::property(getMetaModFactory(x), setMetaModFactory(x))
	auto metaModifiersMetaTable = lua.create_table_with(
	    "EffectDurationModifier", P(MetaModifiers::m_fEffectDurationModifier), "TimerSpeedModifier",
	    P(MetaModifiers::m_fTimerSpeedModifier), "AdditionalEffectsToDispatch",
	    P(MetaModifiers::m_ucAdditionalEffectsToDispatch), "HideChaosUI", P(MetaModifiers::m_bHideChaosUI),
	    "DisableChaos", P(MetaModifiers::m_bDisableChaos), "FlipChaosUI", P(MetaModifiers::m_bFlipChaosUI));
#undef P
	metaModifiersMetaTable[sol::meta_function::new_index] = [] {
	};
	metaModifiersMetaTable[sol::meta_function::index] = metaModifiersMetaTable;
	metaModifiersTable[sol::metatable_key]            = metaModifiersMetaTable;

	if (!ms_NativesDefCache.empty())
	{
		lua.unsafe_script(ms_NativesDefCache);
	}

	lua["GetTickCount"] = GetTickCount64;
	lua["GET_HASH_KEY"] = GET_HASH_KEY;

	lua.new_usertype<LuaHolder>("_Holder", "IsValid", &LuaHolder::IsValid, "AsBoolean", &LuaHolder::As<bool>,
	                            "AsInteger", &LuaHolder::As<int>, "AsFloat", &LuaHolder::As<float>, "AsString",
	                            &LuaHolder::As<char *>, "AsVector3", &LuaHolder::As<LuaVector3>);
	lua["Holder"] = sol::overload(Generate<LuaHolder>, Generate<LuaHolder, const sol::object &>);

	lua.new_usertype<LuaVector3>("_Vector3", "x", &LuaVector3::m_fX, "y", &LuaVector3::m_fY, "z", &LuaVector3::m_fZ);
	lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

#define E(x, y)                 \
	{                           \
		x, [=](sol::state &lua) \
		{                       \
			lua[x] = y;         \
		}                       \
	}
	struct ExposableFunc
	{
		const char *Name;
		const std::function<void(sol::state &)> ExposeFunc;

		ExposableFunc(const char *name, std::function<void(sol::state &)> exposeFunc)
		    : Name(name), ExposeFunc(exposeFunc)
		{
		}

		void operator()(sol::state &lua) const
		{
			ExposeFunc(lua);
		}
	};
	static const std::vector<ExposableFunc> exposables {
		E("print", [scriptName](const std::string &szText) { LuaPrint(scriptName, szText); }),

		E("_invoke", [scriptName](const sol::this_state &lua, DWORD64 ullHash, ELuaNativeReturnType eReturnType,
		                          const sol::variadic_args &args)
		  { return LuaInvoke(scriptName, lua, ullHash, eReturnType, args); }),

		E("WAIT", scriptWait),

		E("IsKeyPressed",
		  [](unsigned char key)
		  {
		      if (ComponentExists<KeyStates>())
		      {
			      return GetComponent<KeyStates>()->IsKeyPressed(key);
		      }

		      return false;
		  }),
		E("IsKeyJustPressed",
		  [](unsigned char key)
		  {
		      if (ComponentExists<KeyStates>())
		      {
			      return GetComponent<KeyStates>()->IsKeyJustPressed(key);
		      }

		      return false;
		  }),

		E("APPLY_FORCE_TO_ENTITY", APPLY_FORCE_TO_ENTITY),
		E("APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS", APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS),

		E("LoadModel", LoadModel),

		E("GetAllPeds", GetAllPedsArray),
		E("CreatePoolPed", CreatePoolPed),

		E("GetAllVehicles", GetAllVehsArray),
		E("CreatePoolVehicle", CreatePoolVehicle),
		E("CreateTempVehicle", CreateTempVehicle),
		E("CreateTempVehicleOnPlayerPos", CreateTempVehicleOnPlayerPos),
		E("SetSurroundingPedsInVehicles", SetSurroundingPedsInVehicles),
		E("ReplaceVehicle", ReplaceVehicle),

		E("GetAllProps", GetAllProps),
		E("CreatePoolProp", CreatePoolProp),

		E("GetAllWeapons", Memory::GetAllWeapons),
		E("GetAllPedModels", Memory::GetAllPedModels),
		E("GetAllVehicleModels", Memory::GetAllVehModels),

		E("OverrideShader", Hooks::OverrideShader),
		E("ResetShader", Hooks::ResetShader),

		E("SetSnowState", Memory::SetSnow),

		E("IsVehicleBraking", Memory::IsVehicleBraking),

		E("EnableScriptThreadBlock", Hooks::EnableScriptThreadBlock),
		E("DisableScriptThreadBlock", Hooks::DisableScriptThreadBlock),

		E("SetAudioPitch", Hooks::SetAudioPitch),
		E("ResetAudioPitch", Hooks::ResetAudioPitch),
		E("SetAudioClearness", Hooks::SetAudioClearness),
		E("ResetAudioClearness", Hooks::ResetAudioClearness),

		E("GetGameplayCamOffsetInWorldCoords",
		  [](LuaVector3 vOffset)
		  {
		      Vector3 vReturn =
		          Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(vOffset.m_fX, vOffset.m_fY, vOffset.m_fZ));
		      return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
		  }),
		E("GetCoordsFromGameplayCam",
		  [](float fDistance)
		  {
		      Vector3 vReturn = Util::GetCoordsFromGameplayCam(fDistance);
		      return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
		  }),

		E("GetCoordAround",
		  [](Entity iEntity, float fAngle, float fRadius, float fZOffset, bool bRelative)
		  {
		      Vector3 vReturn = GetCoordAround(iEntity, fAngle, fRadius, fZOffset, bRelative);
		      return LuaVector3(vReturn.x, vReturn.y, vReturn.z);
		  }),

		E("IsWeaponShotgun", Util::IsWeaponShotgun),

		E("GetChaosModVersion", []() { return MOD_VERSION; }),
		E("GetGameBuild", Memory::GetGameBuild),
	};
#undef E

	bool threadUnsafeFuncCalled = false;

	for (const auto &exposable : exposables)
	{
		if (flags & ParseScript_IsAlienThread)
		{
			lua[exposable.Name] = [&](const sol::variadic_args &args)
			{
				threadUnsafeFuncCalled = true;
			};
		}
		else
		{
			exposable(lua);
		}
	}

	lua.new_enum("EOverrideShaderType", "LensDistortion", EOverrideShaderType::LensDistortion, "Snow",
	             EOverrideShaderType::Snow);

	const auto &result = lua.safe_script(script);
	if (!result.valid())
	{
		const sol::error &error = result;
		LUA_SCRIPT_LOG(scriptName, error.what());

		return ParseScriptReturnReason::Error;
	}

	if (threadUnsafeFuncCalled)
	{
		return ParseScriptReturnReason::Error_ThreadUnsafe;
	}

	if (flags & ParseScript_IsAlienThread)
	{
		for (auto exposable : exposables)
		{
			exposable(lua);
		}
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

	static std::mutex effectGroupMutex;
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
				std::lock_guard lock(effectGroupMutex);

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
			return ParseScriptReturnReason::Error;
		}
	}

	const auto &effectInfo                          = *effectInfoOpt;

	const sol::optional<std::string> &effectNameOpt = effectInfo["Name"];
	if (!effectNameOpt)
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect: Missing Name!");
		return ParseScriptReturnReason::Error;
	}
	const auto &effectName = trim(*effectNameOpt);
	if (effectName.empty())
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect: Invalid Name!");
		return ParseScriptReturnReason::Error;
	}

	sol::optional<std::string> effectIdOpt = effectInfo["EffectId"];
	if (!effectIdOpt)
	{
		// Also backwards compat
		effectIdOpt = effectInfo["ScriptId"].get<sol::optional<std::string>>();
		if (!effectIdOpt)
		{
			LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": Missing EffectId!");
			return ParseScriptReturnReason::Error;
		}
	}
	const auto &effectId = trim(*effectIdOpt);
	if (effectId.empty() || effectId.starts_with('.'))
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": Invalid EffectId!");
		return ParseScriptReturnReason::Error;
	}

	static std::mutex effectsMutex;
	{
		std::unique_lock lock(effectsMutex);
		auto result             = ms_dictRegisteredEffects.find(effectId);
		bool doesIdAlreadyExist = result != ms_dictRegisteredEffects.end();

		if (doesIdAlreadyExist)
		{
			if (result->second.IsTemporary())
			{
				/* Replace existing temporary effect with this one */

				if (ComponentExists<EffectDispatcher>())
				{
					GetComponent<EffectDispatcher>()->ClearEffect(effectId);
				}

				RemoveScriptEntry(effectId);

				doesIdAlreadyExist = false;
			}
		}
		else
		{
			for (const auto &effect : g_dictEffectsMap)
			{
				if (effect.second.Id == effectId)
				{
					doesIdAlreadyExist = true;
					break;
				}
			}
		}
		lock.unlock();

		if (doesIdAlreadyExist)
		{
			LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": EffectId \""
			                                                                 << effectId << "\" already exists!");
			return ParseScriptReturnReason::Error;
		}
	}

	lua["WAIT"] = [](DWORD time)
	{
		WAIT(time);
	};

	lua["OverrideEffectName"] = [effectId](const sol::this_state &lua, const std::string &name)
	{
		if (ComponentExists<EffectDispatcher>())
		{
			GetComponent<EffectDispatcher>()->OverrideEffectName(effectId, name);
		}
	};
	lua["OverrideEffectNameById"] = [effectId](const sol::this_state &lua, const std::string &overrideId)
	{
		if (ComponentExists<EffectDispatcher>())
		{
			GetComponent<EffectDispatcher>()->OverrideEffectNameId(effectId, overrideId);
		}
	};

	EffectData effectData;
	effectData.Name                                    = effectName;
	effectData.Id                                      = effectId;

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
				                               << "\" is invalid for temporary effects, please use another TimedType!");
			}
		}
		else if (szTimedTypeText == "Custom")
		{
			const sol::optional<int> &durationOpt = effectInfo["CustomTime"];
			if (!durationOpt)
			{
				LUA_SCRIPT_LOG(scriptName,
				               "WARNING: TimedType \"Custom\" for effect \""
				                   << effectName
				                   << "\" but no CustomTime defined? Falling back to \"Normal\" TimedType!");
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
		const auto &incompatibleIds = *incompatibleIdsOpt;
		for (const auto &entry : incompatibleIds)
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

		std::lock_guard lock(effectGroupMutex);

		if (!g_dictEffectGroups.contains(effectGroup))
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

	{
		std::lock_guard lock(effectsMutex);
		ms_dictRegisteredEffects.emplace(effectId, LuaScript(scriptName, lua, flags & ParseScript_IsTemporary));
		g_dictEnabledEffects.emplace(effectId, effectData);
		g_RegisteredEffects.emplace_back(effectId);
	}

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

	return ParseScriptReturnReason::Success;
}

namespace LuaScripts
{
	void Load()
	{
		ms_dictRegisteredEffects.clear();

		ClearRegisteredScriptEffects();

		ms_NativesDefCache.clear();
		if (DoesFileExist(LUA_NATIVESDEF))
		{
			std::ifstream inStream(LUA_NATIVESDEF);
			std::ostringstream ossBuffer;
			ossBuffer << inStream.rdbuf();
			ms_NativesDefCache = ossBuffer.str();
		}

		SYSTEM_INFO systemInfo {};
		GetSystemInfo(&systemInfo);

		struct WorkerThread
		{
			std::thread Thread;
		};
		std::vector<std::unique_ptr<WorkerThread>> workerThreadPool;
		workerThreadPool.resize(systemInfo.dwNumberOfProcessors - 1);

		std::queue<std::filesystem::directory_entry> entryQueue;
		std::mutex entryQueueMutex;

		std::queue<std::filesystem::directory_entry> threadUnsafeEntryQueue;
		std::mutex threadUnsafeEntryQueueMutex;

		auto mainThread  = std::this_thread::get_id();

		bool isDone      = false;

		auto parseScript = [mainThread, &threadUnsafeEntryQueue, &threadUnsafeEntryQueueMutex](
		                       const std::filesystem::directory_entry &entry, bool printRunningLog = true)
		{
			const auto &path     = entry.path();
			const auto &fileName = path.filename().string();
			const auto &pathStr  = path.string();
			auto scriptName      = pathStr.substr(pathStr.find("\\") + 1);

			std::ifstream fileStream(path.c_str());
			std::stringstream buffer;
			buffer << fileStream.rdbuf();

			if (printRunningLog)
			{
				// Don't include chaosmod directory
				LUA_LOG("Running script " << scriptName);
			}

			auto currentThread = std::this_thread::get_id();
			if (ParseScriptRaw(fileName, buffer.str(),
			                   currentThread == mainThread ? ParseScript_None : ParseScript_IsAlienThread)
			    == ParseScriptReturnReason::Error_ThreadUnsafe)
			{
				std::lock_guard lock(threadUnsafeEntryQueueMutex);
				threadUnsafeEntryQueue.push(entry);
			}
		};

		auto parseScriptThreaded = [&](const std::filesystem::directory_entry &entry)
		{
			// For our 1c/1t users out here we'll do evaluation immediately in the main thread
			if (workerThreadPool.empty())
			{
				parseScript(entry);
				return;
			}

			bool foundNewThread = false;
			for (auto &workerThread : workerThreadPool)
			{
				if (workerThread)
				{
					continue;
				}

				workerThread         = std::make_unique<WorkerThread>();
				workerThread->Thread = std::thread(
				    [entry, parseScript, &entryQueue, &entryQueueMutex, &isDone]()
				    {
					    parseScript(entry);

					    while (!isDone || !entryQueue.empty())
					    {
						    if (entryQueue.empty())
						    {
							    continue;
						    }
						    std::unique_lock lock(entryQueueMutex);
						    if (entryQueue.empty())
						    {
							    continue;
						    }

						    auto entry = entryQueue.front();
						    entryQueue.pop();

						    lock.unlock();

						    parseScript(entry);
					    }
				    });

				foundNewThread = true;
				break;
			}

			if (!foundNewThread)
			{
				entryQueue.push(entry);
			}
		};

		for (auto dir : ms_rgScriptDirs)
		{
			if (!DoesFileExist(dir))
			{
				continue;
			}

			if (!strcmp(dir, "chaosmod\\workshop"))
			{
				for (const auto &entry : std::filesystem::directory_iterator(dir))
				{
					if (entry.is_directory())
					{
						for (const auto &entry :
						     GetWorkshopSubmissionFiles(entry.path().string(), WorkshopFileType::Script))
						{
							parseScriptThreaded(entry);
						}
					}
				}
			}
			else
			{
				for (const auto &entry : GetFiles(dir, ".lua", true))
				{
					parseScriptThreaded(entry);
				}
			}
		}

		isDone = true;

		for (auto &workerThread : workerThreadPool)
		{
			if (workerThread && workerThread->Thread.joinable())
			{
				workerThread->Thread.join();
			}
		}

		while (!threadUnsafeEntryQueue.empty())
		{
			auto &entry = threadUnsafeEntryQueue.front();
			parseScript(entry, false);
			threadUnsafeEntryQueue.pop();
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