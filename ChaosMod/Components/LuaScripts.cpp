#include <stdafx.h>

#include "LuaScripts.h"

#include "Components/DebugSocket.h"
#include "Components/EffectDispatcher.h"
#include "Components/KeyStates.h"
#include "Components/MetaModifiers.h"
#include "Components/Workshop.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffects.h"
#include "Effects/Register/RegisteredEffects.h"
#include "Effects/Register/RegisteredEffectsMetadata.h"
#include "Info.h"
#include "Memory/Hooks/AudioSettingsHook.h"
#include "Memory/Hooks/GetLabelTextHook.h"
#include "Memory/Hooks/ShaderHook.h"
#include "Memory/PedModels.h"
#include "Memory/Snow.h"
#include "Memory/Vehicle.h"
#include "Memory/WeaponPool.h"
#include "Util/Camera.h"
#include "Util/EntityIterator.h"
#include "Util/File.h"
#include "Util/HelpText.h"
#include "Util/Peds.h"
#include "Util/Player.h"
#include "Util/PoolSpawner.h"
#include "Util/Script.h"
#include "Util/Text.h"
#include "Util/Types.h"
#include "Util/Vehicle.h"
#include "Util/Weapon.h"

#define LUA_NATIVESDEF "chaosmod\\natives_def.lua"

#ifdef _MSC_VER
#define _LUAFUNC __forceinline
#elif defined(__clang__) || defined(__GNUC__)
#define _LUAFUNC __attribute__((always_inline)) inline
#else
#define _LUAFUNC inline
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
	} while (0)
#define LUA_SCRIPT_LOG(scriptName, text)                            \
	do                                                              \
	{                                                               \
		LuaPrint(scriptName, (std::ostringstream() << text).str()); \
	} while (0)

static const std::vector<const char *> ms_ScriptDirs { "chaosmod\\scripts", "chaosmod\\workshop",
	                                                   "chaosmod\\custom_scripts" };

_LUAFUNC void LuaPrint(const std::string &text)
{
	COLOR_PREFIX_LOG("(Lua)", text);
}

_LUAFUNC void LuaPrint(const std::string &name, const std::string &text)
{
	COLOR_PREFIX_LOG("(" << name << ")", text);

#ifdef WITH_DEBUG_PANEL_SUPPORT
	if (ComponentExists<DebugSocket>())
		GetComponent<DebugSocket>()->ScriptLog(name, text);
#endif
}

_LUAFUNC static char *_TryParseString(void *str)
{
	auto string = reinterpret_cast<char *>(str);

	MAGIC_CATCH_BEGIN
	// Access string to try to trigger an access violation
	for (char *c = string; *c; c++)
	{
	}
	MAGIC_CATCH_END(return nullptr)

	return string;
}

_LUAFUNC static bool _TryParseVector3(void **vector, float &x, float &y, float &z)
{
	MAGIC_CATCH_BEGIN
	x = *reinterpret_cast<float *>(vector);
	y = *reinterpret_cast<float *>(vector + 1);
	z = *reinterpret_cast<float *>(vector + 2);
	MAGIC_CATCH_END(return false)

	return true;
}

_LUAFUNC static bool _CallNative(void ***result)
{
	MAGIC_CATCH_BEGIN
	*result = reinterpret_cast<void **>(nativeCall());
	MAGIC_CATCH_END(return false)

	return true;
}

template <typename T, typename... A> _LUAFUNC static T Generate(const A &&...args)
{
	return T(args...);
}

struct LuaVector3
{
	alignas(8) float X = 0.f;
	alignas(8) float Y = 0.f;
	alignas(8) float Z = 0.f;

	LuaVector3()       = default;

	LuaVector3(float x, float y, float z) : X(x), Y(y), Z(z)
	{
	}
};

class LuaHolder
{
  public:
	void *m_Data = nullptr;
	sol::object m_Obj;

	LuaHolder() = default;

	LuaHolder(const sol::object &obj) : m_Obj(obj)
	{
	}

	~LuaHolder()
	{
		if (m_Obj.valid())
			m_Obj.abandon();
	}

	template <typename T> __forceinline T As()
	{
		if constexpr (std::is_same<T, char *>())
		{
			return _TryParseString(m_Data);
		}
		else if constexpr (std::is_same<T, LuaVector3>())
		{
			float x, y, z;

			return _TryParseVector3(&m_Data, x, y, z) ? LuaVector3(x, y, z) : LuaVector3();
		}

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
		return *reinterpret_cast<T *>(&m_Data);
	}

	__forceinline bool IsValid() const
	{
		return m_Data || m_Obj.valid();
	}
};

enum class LuaNativeReturnType
{
	None,
	Bool,
	Int,
	UInt,
	Float,
	String,
	Vector3
};

_LUAFUNC static sol::object LuaInvoke(const std::string &scriptName, const sol::this_state &lua, DWORD64 nativeHash,
                                      LuaNativeReturnType returnType, const sol::variadic_args &args)
{
	if (nativeHash == 0x213AEB2B90CBA7AC || nativeHash == 0x5A5F40FE637EB584 || nativeHash == 0x933D6A9EEC1BACD0
	    || nativeHash == 0xE80492A9AC099A93 || nativeHash == 0x8EF07E15701D61ED)
	{
		return sol::make_object(lua, sol::lua_nil);
	}

	nativeInit(nativeHash);

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
				nativePush(&holder.m_Data);
			}
		}
	}

	void **returned;
	if (!_CallNative(&returned))
	{
		LuaPrint(scriptName,
		         (std::ostringstream() << "Error while invoking native 0x" << std::uppercase << std::hex << nativeHash)
		             .str());
	}
	else if (returned)
	{
		switch (returnType)
		{
		case LuaNativeReturnType::Bool:
			return sol::make_object(lua, *reinterpret_cast<bool *>(returned));
		case LuaNativeReturnType::Int:
			return sol::make_object(lua, *reinterpret_cast<int *>(returned));
		case LuaNativeReturnType::UInt:
			return sol::make_object(lua, *reinterpret_cast<int *>(returned));
		case LuaNativeReturnType::Float:
			return sol::make_object(lua, *reinterpret_cast<float *>(returned));
		case LuaNativeReturnType::String:
			return sol::make_object(lua, _TryParseString(*returned));
		case LuaNativeReturnType::Vector3:
		{
			LuaVector3 vector3;
			if (_TryParseVector3(returned, vector3.X, vector3.Y, vector3.Z))
				return sol::make_object(lua, vector3);
		}
		break;
		default:
			break;
		}
	}

	return sol::make_object(lua, sol::lua_nil);
}

LuaScripts::LuaScripts()
{
	auto parseScript = [&](const std::filesystem::directory_entry &entry)
	{
		const auto &path     = entry.path();
		const auto &fileName = path.filename().string();
		const auto &pathStr  = path.string();
		auto scriptName      = pathStr.substr(pathStr.find("\\") + 1);

		LUA_LOG("Running script " << scriptName);

		std::unordered_map<std::string, nlohmann::json> userEffectSettings;
		if (pathStr.starts_with("chaosmod\\workshop") && ComponentExists<Workshop>())
		{
			// Read user script settings
			auto tmp           = pathStr.substr(strlen("chaosmod\\workshop\\"));
			userEffectSettings = GetComponent<Workshop>()->GetSubmissionScriptSettings(
			    pathStr.substr(0, pathStr.find('\\', pathStr.find_first_not_of("chaosmod\\workshop\\"))),
			    tmp.substr(tmp.find("\\") + 1));
		}

		ParseScript(fileName, path.string(), ParseScriptFlag_ScriptIsFilePath, userEffectSettings);
	};

	for (auto dir : ms_ScriptDirs)
	{
		if (!DoesFileExist(dir))
			continue;

		if (!strcmp(dir, "chaosmod\\workshop"))
		{
			for (const auto &entry : std::filesystem::directory_iterator(dir))
			{
				if (entry.is_directory() && ComponentExists<Workshop>())
					for (const auto &entry : GetComponent<Workshop>()->GetSubmissionFiles(entry.path().string(),
					                                                                      Workshop::FileType::Script))
						parseScript(entry);
			}
		}
		else
		{
			for (const auto &entry : GetFiles(dir, ".lua", true))
				parseScript(entry);
		}
	}
}

void LuaScripts::OnModPauseCleanup(PauseCleanupFlags cleanupFlags)
{
	// Clean up all registered script effects
	for (auto it = g_RegisteredEffects.begin(); it != g_RegisteredEffects.end();)
		if (it->IsScript())
			it = g_RegisteredEffects.erase(it);
		else
			it++;

	// Clean up all effect groups registered by scripts
	for (auto it = g_EffectGroups.begin(); it != g_EffectGroups.end();)
	{
		const auto &[groupName, groupData] = *it;
		if (groupData.WasRegisteredByScript)
			it = g_EffectGroups.erase(it);
		else
			it++;
	}
}

#define E(x, y) lua[x] = y;

static void SetupState(sol::state &lua, const std::string &scriptName)
{
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::bit32);

	lua["ReturnType"] = lua.create_table_with("None", LuaNativeReturnType::None, "Boolean", LuaNativeReturnType::Bool,
	                                          "Integer", LuaNativeReturnType::Int, "UnsignedInteger",
	                                          LuaNativeReturnType::UInt, "String", LuaNativeReturnType::String, "Float",
	                                          LuaNativeReturnType::Float, "Vector3", LuaNativeReturnType::Vector3);

	lua.new_usertype<LuaHolder>("_Holder", "IsValid", &LuaHolder::IsValid, "AsBoolean", &LuaHolder::As<bool>,
	                            "AsInteger", &LuaHolder::As<int>, "AsUnsignedInteger", &LuaHolder::As<uint>, "AsFloat",
	                            &LuaHolder::As<float>, "AsString", &LuaHolder::As<char *>, "AsVector3",
	                            &LuaHolder::As<LuaVector3>);
	lua["Holder"] = sol::overload(Generate<LuaHolder>, Generate<LuaHolder, const sol::object &>);

	lua.new_usertype<LuaVector3>("_Vector3", "x", &LuaVector3::X, "y", &LuaVector3::Y, "z", &LuaVector3::Z);
	lua["Vector3"] = sol::overload(Generate<LuaVector3>, Generate<LuaVector3, float, float, float>);

	lua.new_enum("OverrideShaderType", "LensDistortion", OverrideShaderType::LensDistortion, "Snow",
	             OverrideShaderType::Snow);
	// Backwards compat
	lua.new_enum("EOverrideShaderType", "LensDistortion", OverrideShaderType::LensDistortion, "Snow",
	             OverrideShaderType::Snow);

	lua["print"] = [scriptName](const std::string &text)
	{
		LuaPrint(scriptName, text);
	};

	E("GetTickCount", GetTickCount64);
	E("GET_HASH_KEY", GET_HASH_KEY);
	E("GetChaosModVersion", []() { return MOD_VERSION; });
	E("GetGameBuild", Memory::GetGameBuild);
}

static sol::bytecode ms_NativesDefCache;

static void SetupLateState(sol::state &lua, const std::string &scriptName)
{
	if (ms_NativesDefCache.empty() && DoesFileExist(LUA_NATIVESDEF))
	{
		sol::state lua;
		sol::load_result lr = lua.load_file(LUA_NATIVESDEF);
		if (!lr.valid())
			LUA_LOG("Natives def file is invalid!");
		else
		{
			sol::protected_function target = lr;
			ms_NativesDefCache             = target.dump();
		}
	}

	lua["_invoke"] = [scriptName](const sol::this_state &lua, DWORD64 hash, LuaNativeReturnType returnType,
	                              const sol::variadic_args &args)
	{
		return LuaInvoke(scriptName, lua, hash, returnType, args);
	};

	lua.unsafe_script(ms_NativesDefCache.as_string_view());

	if (ComponentExists<MetaModifiers>())
	{
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
#define P(x)                                                           \
	sol::property(getMetaModFactory(GetComponent<MetaModifiers>()->x), \
	              setMetaModFactory(GetComponent<MetaModifiers>()->x))
		auto metaModifiersMetaTable = lua.create_table_with(
		    "EffectDurationModifier", P(EffectDurationModifier), "TimerSpeedModifier", P(TimerSpeedModifier),
		    "AdditionalEffectsToDispatch", P(AdditionalEffectsToDispatch), "HideChaosUI", P(HideChaosUI),
		    "DisableChaos", P(DisableChaos), "FlipChaosUI", P(FlipChaosUI));
#undef P
		metaModifiersMetaTable[sol::meta_function::new_index] = [] {};
		metaModifiersMetaTable[sol::meta_function::index] = metaModifiersMetaTable;
		metaModifiersTable[sol::metatable_key]            = metaModifiersMetaTable;
	}

	E("IsKeyPressed",
	  [](unsigned char key)
	  {
		  if (ComponentExists<KeyStates>())
			  return GetComponent<KeyStates>()->IsKeyPressed(key);

		  return false;
	  });
	E("IsKeyJustPressed",
	  [](unsigned char key)
	  {
		  if (ComponentExists<KeyStates>())
			  return GetComponent<KeyStates>()->IsKeyJustPressed(key);

		  return false;
	  });
	E("WAIT", WAIT);
	E("APPLY_FORCE_TO_ENTITY", APPLY_FORCE_TO_ENTITY);
	E("APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS", APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS);
	E("GET_HASH_KEY", GET_HASH_KEY);
	E("LoadModel", LoadModel);
	E("GetAllPeds", GetAllPedsArray);
	E("CreatePoolPed", CreatePoolPed);
	E("SetCompanionRelationship", SetCompanionRelationship);
	E("TeleportPlayer", [](float x, float y, float z, bool noOffset) { TeleportPlayer(x, y, z, noOffset); });
	E("GetAllVehicles", GetAllVehsArray);
	E("CreatePoolVehicle", CreatePoolVehicle);
	E("CreateTempVehicle", CreateTempVehicle);
	E("CreateTempVehicleOnPlayerPos", CreateTempVehicleOnPlayerPos);
	E("SetSurroundingPedsInVehicles", SetSurroundingPedsInVehicles);
	E("ReplaceVehicle", ReplaceVehicle);
	E("GetAllProps", GetAllPropsArray);
	E("CreatePoolProp", CreatePoolProp);
	E("GetAllWeapons", Memory::GetAllWeapons);
	E("GetAllPedModels", Memory::GetAllPedModels);
	E("GetAllVehicleModels", Memory::GetAllVehModels);
	E("OverrideShader", Hooks::OverrideShader);
	E("ResetShader", Hooks::ResetShader);
	E("SetSnowState", Memory::SetSnow);
	E("IsVehicleBraking", Memory::IsVehicleBraking);
	E("EnableScriptThreadBlock", Hooks::EnableScriptThreadBlock);
	E("DisableScriptThreadBlock", Hooks::DisableScriptThreadBlock);
	E("SetAudioPitch", Hooks::SetAudioPitch);
	E("ResetAudioPitch", Hooks::ResetAudioPitch);
	E("SetAudioClearness", Hooks::SetAudioLPFCutoff);
	E("ResetAudioClearness", Hooks::ResetAudioLPFCutoff);
	E("SetAudioHPFCutoff", Hooks::SetAudioHPFCutoff);
	E("ResetAudioHPFCutoff", Hooks::ResetAudioHPFCutoff);
	E("SetAudioLPFCutoff", Hooks::SetAudioLPFCutoff);
	E("ResetAudioLPFCutoff", Hooks::ResetAudioLPFCutoff);
	E("SetAudioVolume", Hooks::SetAudioVolume);
	E("ResetAudioVolume", Hooks::ResetAudioVolume);
	E("GetGameplayCamOffsetInWorldCoords",
	  [](LuaVector3 offset)
	  {
		  const auto &result = Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(offset.X, offset.Y, offset.Z));
		  return LuaVector3(result.x, result.y, result.z);
	  });
	E("GetCoordsFromGameplayCam",
	  [](float distance)
	  {
		  const auto &result = Util::GetCoordsFromGameplayCam(distance);
		  return LuaVector3(result.x, result.y, result.z);
	  });
	E("GetCoordAround",
	  [](Entity entity, float angle, float radius, float zOffset, bool relative)
	  {
		  const auto &result = GetCoordAround(entity, angle, radius, zOffset, relative);
		  return LuaVector3(result.x, result.y, result.z);
	  });
	E("IsWeaponShotgun", Util::IsWeaponShotgun);
	E("AddCustomLabel", Hooks::AddCustomLabel);
	E("DisplayHelpText", DisplayHelpText);
	E("GetRandomInt",
	  [](int lower, int upper) -> int
	  {
		  if (lower > upper)
			  return 0;
		  else if (lower == upper)
			  return lower;
		  return g_Random.GetRandomInt(lower, upper);
	  });
	E("GetRandomFloat",
	  [](float lower, float upper) -> float
	  {
		  if (lower > upper)
			  return 0.f;
		  else if (lower == upper)
			  return lower;
		  return g_Random.GetRandomFloat(lower, upper);
	  });
	E("GetEffectCompletionPercentage", CurrentEffect::GetEffectCompletionPercentage);
	E("OverrideEffectName", [](const std::string &name) { CurrentEffect::OverrideEffectName(name); });
	E("OverrideEffectNameById",
	  [](const std::string &overrideId) { CurrentEffect::OverrideEffectNameFromId(overrideId); });
	E("SetEffectSoundFollowPlayer",
	  []()
	  {
		  auto sharedData = EffectThreads::GetThreadSharedData(GetCurrentFiber());
		  if (sharedData)
			  sharedData->EffectSoundPlayOptions.PlayType = EffectSoundPlayType::FollowPlayer;
	  });
	E("SetEffectSoundFollowEntity",
	  [](Entity entity)
	  {
		  auto sharedData = EffectThreads::GetThreadSharedData(GetCurrentFiber());
		  if (!sharedData)
			  return;
		  sharedData->EffectSoundPlayOptions.PlayType = EffectSoundPlayType::FollowEntity;
		  sharedData->EffectSoundPlayOptions.Entity   = entity;
	  });
	E("SetEffectSoundAtCoords",
	  [](const LuaVector3 &coords)
	  {
		  auto sharedData = EffectThreads::GetThreadSharedData(GetCurrentFiber());
		  if (!sharedData)
			  return;
		  sharedData->EffectSoundPlayOptions.PlayType = EffectSoundPlayType::AtCoords;
		  sharedData->EffectSoundPlayOptions.Coords   = Vector3(coords.X, coords.Y, coords.Z);
	  });
	E("SetEffectSoundLooping",
	  [](bool state)
	  {
		  auto sharedData = EffectThreads::GetThreadSharedData(GetCurrentFiber());
		  if (sharedData)
			  sharedData->EffectSoundPlayOptions.PlayFlags =
			      state ? sharedData->EffectSoundPlayOptions.PlayFlags | EffectSoundPlayFlags_Looping
			            : sharedData->EffectSoundPlayOptions.PlayFlags & ~EffectSoundPlayFlags_Looping;
	  });
	E("SetEffectSoundStopOnEntityDeath",
	  [](bool state)
	  {
		  auto sharedData = EffectThreads::GetThreadSharedData(GetCurrentFiber());
		  if (sharedData)
			  sharedData->EffectSoundPlayOptions.PlayFlags =
			      state ? sharedData->EffectSoundPlayOptions.PlayFlags & ~EffectSoundPlayFlags_DontStopOnEntityDeath
			            : sharedData->EffectSoundPlayOptions.PlayFlags | EffectSoundPlayFlags_DontStopOnEntityDeath;
	  });
}

#undef E

LuaScripts::ParseScriptReturnReason
LuaScripts::ParseScript(std::string scriptName, const std::string &script, ParseScriptFlags flags,
                        std::unordered_map<std::string, nlohmann::json> settingOverrides)
{
	sol::state lua;
	SetupState(lua, scriptName);

	auto scriptResult =
	    flags & ParseScriptFlag_ScriptIsFilePath ? lua.safe_script_file(std::string(script)) : lua.safe_script(script);
	if (!scriptResult.valid())
	{
		const sol::error &error = scriptResult;
		LUA_SCRIPT_LOG(scriptName, error.what());

		return ParseScriptReturnReason::Error;
	}

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
			const auto &groupName = StringTrim(*groupNameOpt);

			if (!(flags & ParseScriptFlag_IsTemporary))
			{
				const auto &result = g_EffectGroups.find(groupName);
				if (result != g_EffectGroups.end() && !result->second.IsPlaceholder)
				{
					LUA_SCRIPT_LOG(scriptName, "WARNING: Could not register effect group \""
					                               << groupName << "\": Already registered!");
				}
				else
				{
					g_EffectGroups[groupName].IsPlaceholder         = false;
					g_EffectGroups[groupName].WasRegisteredByScript = true;

					const sol::optional<int> &groupWeightMultOpt    = effectGroupInfo["WeightMultiplier"];
					if (groupWeightMultOpt)
					{
						g_EffectGroups[groupName].WeightMult =
						    std::clamp(*groupWeightMultOpt, 1, (int)(std::numeric_limits<unsigned short>::max)());
					}

					LUA_SCRIPT_LOG(scriptName, "Registered effect group \"" << groupName
					                                                        << "\" with weight multiplier: "
					                                                        << g_EffectGroups[groupName].WeightMult);
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
			return ParseScriptReturnReason::Error;
	}

	const auto &effectInfo                          = *effectInfoOpt;

	const sol::optional<std::string> &effectNameOpt = effectInfo["Name"];
	if (!effectNameOpt)
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect: Missing Name!");
		return ParseScriptReturnReason::Error;
	}
	const auto &effectName = StringTrim(*effectNameOpt);
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
	const auto &effectId = StringTrim(*effectIdOpt);
	if (effectId.empty() || effectId.starts_with('.'))
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": Invalid EffectId!");
		return ParseScriptReturnReason::Error;
	}

	auto effectResult       = m_RegisteredEffects.find(effectId);
	bool doesIdAlreadyExist = effectResult != m_RegisteredEffects.end();

	if (doesIdAlreadyExist)
	{
		if (effectResult->second.IsTemporary)
		{
			/* Replace existing temporary effect with this one */

			if (ComponentExists<EffectDispatcher>())
				GetComponent<EffectDispatcher>()->ClearEffect(effectId);

			RemoveScriptEntry(effectId);

			doesIdAlreadyExist = false;
		}
	}
	else
	{
		for (const auto &effect : g_RegisteredEffectsMetadata)
		{
			if (effect.second.Id == effectId)
			{
				doesIdAlreadyExist = true;
				break;
			}
		}
	}

	if (doesIdAlreadyExist)
	{
		LUA_SCRIPT_LOG(scriptName, "ERROR: Could not register effect \"" << effectName << "\": EffectId \"" << effectId
		                                                                 << "\" already exists!");
		return ParseScriptReturnReason::Error;
	}

	EffectData effectData;
	effectData.Name                                    = effectName;
	effectData.Id                                      = effectId;

	const sol::optional<std::string> &timedTypeTextOpt = effectInfo["TimedType"];
	if (timedTypeTextOpt)
	{
		const auto &timedTypeText = *timedTypeTextOpt;

		if (timedTypeText == "None")
		{
			effectData.TimedType = EffectTimedType::NotTimed;
		}
		else if (timedTypeText == "Normal")
		{
			effectData.TimedType = EffectTimedType::Normal;
		}
		else if (timedTypeText == "Short")
		{
			effectData.TimedType = EffectTimedType::Short;
		}
		else if (timedTypeText == "Permanent")
		{
			if (flags & ParseScriptFlag_IsTemporary)
			{
				effectData.TimedType  = EffectTimedType::Custom;
				effectData.CustomTime = std::numeric_limits<decltype(effectData.CustomTime)>::max();
			}
			else
				effectData.TimedType = EffectTimedType::Permanent;
		}
		else if (timedTypeText == "Custom")
		{
			const sol::optional<int> &durationOpt = effectInfo["CustomTime"];
			if (!durationOpt)
			{
				LUA_SCRIPT_LOG(scriptName,
				               "WARNING: TimedType \"Custom\" for effect \""
				                   << effectName
				                   << "\" but no CustomTime defined? Falling back to \"Normal\" TimedType!");
				effectData.TimedType = EffectTimedType::Normal;
			}
			else
			{
				effectData.TimedType  = EffectTimedType::Custom;
				effectData.CustomTime = static_cast<float>(std::max(1, *durationOpt));
			}
		}
		else
		{
			LUA_SCRIPT_LOG(scriptName, "WARNING: Unknown TimedType \"" << timedTypeText << "\" specified for effect \""
			                                                           << effectName << "\"!");
		}
	}
	try
	{
		effectData.TimedType = static_cast<EffectTimedType>(settingOverrides["TimedType"]);
	}
	catch (nlohmann::json::exception &)
	{
	}
	try
	{
		effectData.CustomTime = settingOverrides["CustomTime"];
		if (effectData.CustomTime > 0)
			effectData.TimedType = EffectTimedType::Custom;
	}
	catch (nlohmann::json::exception &)
	{
	}

	const sol::optional<int> &weightMultOpt = effectInfo["WeightMultiplier"];
	if (weightMultOpt)
	{
		effectData.WeightMult = static_cast<float>(std::max(1, *weightMultOpt));
		effectData.Weight     = effectData.WeightMult;
	}
	try
	{
		effectData.WeightMult = settingOverrides["WeightMult"];
	}
	catch (nlohmann::json::exception &)
	{
	}

	const sol::optional<bool> &isMetaOpt = effectInfo["IsMeta"];
	if (isMetaOpt)
		effectData.SetAttribute(EffectAttributes::IsMeta, *isMetaOpt);

	const sol::optional<bool> &excludeFromVotingOpt = effectInfo["ExcludeFromVoting"];
	if (excludeFromVotingOpt)
		effectData.SetAttribute(EffectAttributes::ExcludedFromVoting, *excludeFromVotingOpt);
	try
	{
		effectData.SetAttribute(EffectAttributes::ExcludedFromVoting, settingOverrides["ExcludedFromVoting"]);
	}
	catch (nlohmann::json::exception &)
	{
	}

	const sol::optional<bool> &isUtilityOpt = effectInfo["IsUtility"];
	if (isUtilityOpt)
		effectData.SetAttribute(EffectAttributes::IsUtility, *isUtilityOpt);

	const sol::optional<bool> &hideRealNameOnStartOpt = effectInfo["HideRealNameOnStart"];
	if (hideRealNameOnStartOpt)
		effectData.SetAttribute(EffectAttributes::HideRealNameOnStart, *hideRealNameOnStartOpt);

	const sol::optional<sol::table> &incompatibleIdsOpt = effectInfo["IncompatibleIds"];
	if (incompatibleIdsOpt)
	{
		const auto &incompatibleIds = *incompatibleIdsOpt;
		for (const auto &entry : incompatibleIds)
			if (entry.second.valid() && entry.second.is<std::string>())
				effectData.IncompatibleIds.insert(entry.second.as<std::string>());
	}

	const sol::optional<std::string> &effectCategoryOpt = effectInfo["EffectCategory"];
	if (effectCategoryOpt)
	{
		const auto &effectCategoryStr = *effectCategoryOpt;
		auto effectCategoryIt         = g_NameToEffectCategory.find(effectCategoryStr);
		if (effectCategoryIt != g_NameToEffectCategory.end())
		{
			effectData.Category = effectCategoryIt->second;
			if (effectData.Category == EffectCategory::Shader)
				effectData.ConditionType = EffectConditionType::EnhancedShader;
		}
	}

	const sol::optional<std::string> &effectGroupOpt = effectInfo["EffectGroup"];
	if (effectGroupOpt)
	{
		const auto &effectGroup = *effectGroupOpt;
		effectData.GroupType    = effectGroup;

		if (!g_EffectGroups.contains(effectGroup))
			g_EffectGroups[effectGroup] = { .IsPlaceholder = true, .WasRegisteredByScript = true };

		g_EffectGroups[effectGroup].MemberCount++;
	}

	const sol::optional<int> &shortcutKeycodeOpt = effectInfo["ShortcutKeycode"];
	if (shortcutKeycodeOpt)
	{
		int shortcutKeycode = *shortcutKeycodeOpt;
		if (shortcutKeycode > 0 && shortcutKeycode < 255)
		{
			shortcutKeycode += (effectInfo["ShortcutWithCtrl"].get_or(false) << 10);
			shortcutKeycode += (effectInfo["ShortcutWithShift"].get_or(false) << 9);
			shortcutKeycode += (effectInfo["ShortcutWithAlt"].get_or(false) << 8);

			effectData.ShortcutKeycode = shortcutKeycode;
		}
	}
	try
	{
		effectData.ShortcutKeycode = settingOverrides["ShortcutKeycode"];
	}
	catch (nlohmann::json::exception &)
	{
	}

	try
	{
		std::string name = StringTrim(settingOverrides["CustomName"]);
		if (!name.empty())
			effectData.CustomName = name;
	}
	catch (nlohmann::json::exception &)
	{
	}

	// Exclude temporary effects from choices pool
	effectData.SetAttribute(EffectAttributes::IsTemporary, flags & ParseScriptFlag_IsTemporary);

	m_RegisteredEffects.emplace(effectId, LuaScript(scriptName, lua, flags & ParseScriptFlag_IsTemporary));
	g_EnabledEffects.emplace(effectId, effectData);
	g_RegisteredEffects.emplace_back(effectId);

	if (flags & ParseScriptFlag_IsTemporary)
	{
		// Immediately dispatch it too
		if (ComponentExists<EffectDispatcher>())
			GetComponent<EffectDispatcher>()->DispatchEffect(effectId, EffectDispatcher::DispatchEffectFlag_NoAddToLog);
	}
	else
	{
		LUA_SCRIPT_LOG(scriptName, "Registered effect \"" << effectName << "\" with id \"" << effectId << "\"");
	}

	return ParseScriptReturnReason::Success;
}

void LuaScripts::RemoveScriptEntry(const std::string &effectId)
{
	m_RegisteredEffects.erase(effectId);
	g_EnabledEffects.erase(effectId);

	auto result = std::find(g_RegisteredEffects.begin(), g_RegisteredEffects.end(), effectId);
	if (result != g_RegisteredEffects.end())
		g_RegisteredEffects.erase(result);
}

void LuaScripts::LuaScript::Execute(const char *funcName) const
{
	auto func = Lua[funcName];
	if (!func.valid())
		return;

	auto result = func();
	if (!result.valid())
	{
		const sol::error &error = result;

		LuaPrint(ScriptName, error.what());
	}
}

void LuaScripts::Execute(const std::string &effectId, ExecuteFuncType funcType)
{
	auto &script = m_RegisteredEffects.at(effectId);

	if (!script.LateStateSetup)
	{
		SetupLateState(script.Lua, script.ScriptName);
		script.LateStateSetup = true;
	}

	// All three are called for both timed and non-timed effects
	switch (funcType)
	{
	case ExecuteFuncType::Start:
		script.Execute("OnStart");
		break;
	case ExecuteFuncType::Stop:
		script.Execute("OnStop");

		if (script.IsTemporary)
			RemoveScriptEntry(effectId);

		break;
	case ExecuteFuncType::Tick:
		script.Execute("OnTick");
		break;
	}
}

void LuaScripts::RegisterScriptRawTemporary(std::string scriptName, std::string script)
{
	ParseScript(scriptName, script, ParseScriptFlag_IsTemporary);
}