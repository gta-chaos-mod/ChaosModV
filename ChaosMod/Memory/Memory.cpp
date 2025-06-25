#include <stdafx.h>

#include "Memory.h"

#include "Effects/EffectThreads.h"

#include "Memory/Hooks/Hook.h"

#include "Util/Script.h"
#include "Util/Text.h"

static DWORD64 ms_BaseAddr;
static DWORD64 ms_EndAddr;

static std::set<std::string> ms_BlacklistedHookNames;

namespace Memory
{
	void Init()
	{
		MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo));

		ms_BaseAddr = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
		ms_EndAddr  = ms_BaseAddr + moduleInfo.SizeOfImage;

		MH_Initialize();

		if (DoesFeatureFlagExist("skipintro"))
		{
			// Splash screen
			Handle handle = FindPattern("E8 ? ? ? ? 8B CF 40 88 2D", "0F 85 A9 00 00 00 48 8D 54");
			if (!handle.IsValid())
			{
				LOG("SkipIntro: Failed to patch splash screen!");
			}
			else
			{
				if (IsLegacy())
					Write<BYTE>(handle.Into().At(0x21).Into().Get<BYTE>(), 0x0, 36);
				else
				{
					// jz -> jmp
					Write<BYTE>(handle.At(-0xD).Get<BYTE>(), 0xE9);
					Write<BYTE>(handle.At(-0xC).Get<BYTE>(), 0x86);
					Write<BYTE>(handle.At(-0xB).Get<BYTE>(), 0x01);
					Write<BYTE>(handle.At(-0xA).Get<BYTE>(), 0x00);
					// extra nop just in case
					Write<BYTE>(handle.At(-0x8).Get<BYTE>(), 0x90);
				}
				LOG("SkipIntro: Patched splash screen");
			}

			// Legal screen
			handle = FindPattern("E8 ? ? ? ? EB 0D B1 01", "00 E9 ? 05 00 00 E8 ? 07");
			if (!handle.IsValid())
			{
				LOG("SkipIntro: Failed to patch legal screen!");
			}
			else
			{
				if (IsLegacy())
				{
					handle = handle.Into();
					Write<BYTE>(handle.Get<BYTE>(), 0xC3);
					Write<BYTE>(handle.At(0x9).Into().At(0x3).Get<BYTE>(), 0x2);
				}
				else
					Write<BYTE>(handle.At(-0xF).Get<BYTE>(), 0x90, 6);

				LOG("SkipIntro: Patched legal screen");
			}
		}

		if (DoesFeatureFlagExist("skipdlcs"))
		{
			Handle handle = FindPattern("84 C0 74 2C 48 8D 15 ? ? ? ? 48 8D 0D ? ? ? ? 45 33 C9 41 B0 01",
			                            "E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 89 D9 E8 ? ? ? ? EB 03");
			if (!handle.IsValid())
			{
				LOG("SkipDLCs: Failed to patch DLC loading!");
			}
			else
			{
				if (IsEnhanced())
					Write<BYTE>(handle.Get<BYTE>(), 0x90, 5);
				else
					Write<BYTE>(handle.At(24).Get<BYTE>(), 0x90, 24);

				LOG("SkipDLCs: Patched DLC loading");
			}
		}

		if (DoesFeatureFlagExist("blacklistedhooks"))
		{
			std::ifstream file("chaosmod\\.blacklistedhooks");
			if (!file.fail())
			{
				std::string line;
				line.resize(64);
				while (file.getline(line.data(), 64))
					ms_BlacklistedHookNames.insert(StringTrim(line.substr(0, line.find("\n"))));
			}
		}

		LOG("Running hooks");
		std::thread(
		    []()
		    {
			    for (const auto &registeredHook : Memory::GetRegisteredHooks())
			    {
				    if (registeredHook->IsLateHook())
					    continue;

				    const auto &hookName = registeredHook->GetName();

				    if (ms_BlacklistedHookNames.contains(hookName))
				    {
					    LOG(hookName << " hook has been blacklisted from running!");
					    continue;
				    }

				    LOG("Running " << hookName << " hook");

				    if (!registeredHook->RunHook())
					    LOG(hookName << " hook failed!");
			    }

			    MH_EnableHook(MH_ALL_HOOKS);
		    })
		    .detach();
	}

	void Uninit()
	{
		LOG("Running hook cleanups");
		for (const auto &registeredHook : Memory::GetRegisteredHooks())
		{
			const auto &hookName = registeredHook->GetName();

			if (ms_BlacklistedHookNames.contains(hookName))
				continue;

			LOG("Running " << hookName << " hook cleanup");

			registeredHook->RunCleanup();
		}

		MH_DisableHook(MH_ALL_HOOKS);

		MH_Uninitialize();
	}

	void RunLateHooks()
	{
		LOG("Running late hooks");

		std::thread(
		    []()
		    {
			    for (const auto &registeredHook : Memory::GetRegisteredHooks())
			    {
				    if (!registeredHook->IsLateHook())
					    continue;

				    const auto &hookName = registeredHook->GetName();

				    if (ms_BlacklistedHookNames.contains(hookName))
				    {
					    LOG(hookName << " hook has been blacklisted from running!");
					    continue;
				    }

				    LOG("Running " << hookName << " hook");

				    if (!registeredHook->RunHook())
					    LOG(hookName << " hook failed!");
			    }

			    MH_EnableHook(MH_ALL_HOOKS);
		    })
		    .detach();
	}

	Handle FindPattern(const std::string &pattern, const PatternScanRange &&scanRange)
	{
		DEBUG_LOG("Searching for pattern \""
		          << pattern
		          << (scanRange.StartAddr == 0 && scanRange.EndAddr == 0
		                  ? "\""
		                  : (std::stringstream()
		                     << "\" within address range 0x" << std::uppercase << std::hex << scanRange.StartAddr
		                     << std::setfill(' ') << " to 0x" << std::uppercase << std::hex << scanRange.EndAddr)
		                        .str()));

		if ((scanRange.StartAddr != 0 || scanRange.EndAddr != 0) && scanRange.StartAddr >= scanRange.EndAddr)
		{
			LOG("startAddr is equal / bigger than endAddr???");
			return Handle();
		}

		auto scanPattern = [&]() -> Handle
		{
			auto copy = pattern;
			for (size_t pos = copy.find("??"); pos != std::string::npos; pos = copy.find("??", pos + 1))
				copy.replace(pos, 2, "?");

			auto thePattern = scanRange.StartAddr == 0 && scanRange.EndAddr == 0
			                    ? hook::pattern(copy)
			                    : hook::pattern(scanRange.StartAddr, scanRange.EndAddr, copy);
			if (!thePattern.size())
				return {};

			auto resultAddr = reinterpret_cast<uintptr_t>(thePattern.get_first());
			DEBUG_LOG("Found pattern \"" << pattern << "\" at address 0x" << std::uppercase << std::hex << resultAddr
			                             << std::dec);
			return resultAddr;
		};

		if (EffectThreads::IsThreadAnEffectThread())
		{
			Handle handle;

			auto future = std::async(std::launch::async, [&]() { handle = scanPattern(); });

			using namespace std::chrono_literals;
			while (future.wait_for(0ms) != std::future_status::ready)
				WAIT(0);

			return handle;
		}

		return scanPattern();
	}

	Handle FindPattern(const std::string &legacyPattern, const std::string &enhancedPattern)
	{
		return IsEnhanced() ? FindPattern(enhancedPattern) : FindPattern(legacyPattern);
	}

	const char *GetTypeName(__int64 vftAddr)
	{
		if (vftAddr)
		{
			auto vftable = *reinterpret_cast<__int64 *>(vftAddr);
			if (vftable)
			{
				auto rtti = *reinterpret_cast<__int64 *>(vftable - 8);
				if (rtti)
				{
					auto rva = *reinterpret_cast<DWORD *>(rtti + 12);
					if (rva)
					{
						auto typeDesc = ms_BaseAddr + rva;
						if (typeDesc)
							return reinterpret_cast<const char *>(typeDesc + 16);
					}
				}
			}
		}

		return "UNK";
	}

	DWORD64 *GetGlobalPtr(int globalId)
	{
		static auto globalPtr = []() -> DWORD64 **
		{
			auto handle = FindPattern("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11", "48 8D 3D ? ? ? ? 31 ED 4C 8D 25");
			if (!handle.IsValid())
				return nullptr;

			return handle.At(2).Into().Get<DWORD64 *>();
		}();

		static auto fallbackToSHV = []() -> bool
		{
			bool fallbackToSHV = !globalPtr;

			if (fallbackToSHV)
			{
				LOG("Warning: _globalPtr not found, falling back to SHV's getGlobalPtr");
			}
			// HACK: Check for the presence some arbitrary module specific to FiveM
			// Also check if player is in a mp session if so to check for FiveM sp
			else if (GetModuleHandle(L"gta-net-five.dll"))
			{
				bool modeDetermined = false;

				auto handle         = FindPattern("48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 09 48 8D 15");
				if (handle.IsValid())
				{
					auto _networkObj = handle.At(2).Into().Get<DWORD64>();
					handle           = FindPattern("83 B9 ? ? 00 00 05 0F 85 ? ? ? ? E9");
					if (handle.IsValid())
					{
						fallbackToSHV  = *reinterpret_cast<DWORD *>(*_networkObj + handle.At(2).Value<WORD>()) == 5;
						modeDetermined = true;
					}
				}

				if (!modeDetermined)
				{
					LOG("Warning: FiveM detected but could not determine mode, switching to fallback for GetGlobalPtr");
					fallbackToSHV = true;
				}
			}

			if (fallbackToSHV)
				LOG("Warning: FiveM (non-sp) detected, features such as Failsafe will not work!");

			return fallbackToSHV;
		}();

		return fallbackToSHV ? getGlobalPtr(globalId) : (&globalPtr[globalId >> 18 & 0x3F][globalId & 0x3FFFF]);
	}

	std::string GetGameBuild()
	{
		static auto gameBuild = []() -> std::string
		{
			auto handle = Memory::FindPattern("80 3D ? ? ? ? 00 0F 57 C0 48",
			                                  "48 8D 0D ? ? ? ? 48 8D 15 ? ? ? ? 4C 8D 44 24 2C E8");
			if (!handle.IsValid())
				return {};

			std::string buildStr = handle.At(IsLegacy() ? 1 : 2).Into().At(IsLegacy() ? 1 : 0).Get<char>();
			if (buildStr.empty())
				return {};

			auto splitIndex = buildStr.find("-dev");
			if (splitIndex == buildStr.npos)
				return {};

			return buildStr.substr(0, splitIndex);
		}();

		return gameBuild;
	}
}
