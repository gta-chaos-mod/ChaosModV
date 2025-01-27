#include <stdafx.h>

#include "Main.h"

#include "Components/CrossingChallenge.h"
#include "Components/DebugMenu.h"
#include "Components/DebugSocket.h"
#include "Components/EffectDispatchTimer.h"
#include "Components/EffectDispatcher.h"
#include "Components/EffectShortcuts.h"
#include "Components/EffectSound/EffectSoundManagers.h"
#include "Components/Failsafe.h"
#include "Components/HelpTextQueue.h"
#include "Components/KeyStates.h"
#include "Components/LuaScripts.h"
#include "Components/MetaModifiers.h"
#include "Components/SplashTexts.h"
#include "Components/Voting.h"
#include "Components/Workshop.h"
#include "Effects/EffectConfig.h"
#include "Effects/EnabledEffects.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"
#include "Memory/Misc.h"
#include "Util/File.h"
#include "Util/OptionsManager.h"
#include "Util/PoolSpawner.h"
#include "Util/Text.h"

static struct
{
	bool ClearAllEffects             = false;
	bool ClearEffectsShortcutEnabled = false;
	bool ToggleModShortcutEnabled    = false;
	bool ToggleModState              = false;
	bool DisableMod                  = false;
	bool PauseTimerShortcutEnabled   = false;
	bool HaveLateHooksRan            = false;
	bool AntiSoftlockShortcutEnabled = false;
	bool RunAntiSoftlock             = false;
} ms_Flags;

static std::array<BYTE, 3> ParseConfigColorString(const std::string &colorText)
{
	// Format: #ARGB
	std::array<BYTE, 3> colors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
		Util::TryParse<BYTE>(colorText.substr(i, 2), colors[j++], 16);

	return colors;
}

static void ParseEffectsFile()
{
	g_EnabledEffects.clear();

	EffectConfig::ReadConfig("chaosmod/configs/effects.ini", g_EnabledEffects, { "chaosmod/effects.ini" });
}

static void Init()
{
	// Attempt to print game build number
	// We're doing it here as the build number isn't available when the mod is attached to the game process
	static auto printedGameBuild = []()
	{
		auto gameBuild = Memory::GetGameBuild();
		if (gameBuild.empty())
			gameBuild = "Unknown";

		LOG("Game Build: " << gameBuild);

		return true;
	}();

	static std::streambuf *oldStreamBuf;
	if (DoesFeatureFlagExist("enableconsole"))
	{
		if (GetConsoleWindow())
		{
			system("cls");
		}
		else
		{
			LOG("Creating log console");

			AllocConsole();

			SetConsoleTitle(L"ChaosModV");
			DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

			oldStreamBuf = std::cout.rdbuf();

			g_ConsoleOut = std::ofstream("CONOUT$");
			std::cout.rdbuf(g_ConsoleOut.rdbuf());

			std::cout.clear();

			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			DWORD conMode;
			GetConsoleMode(handle, &conMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), conMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
	}
	else if (GetConsoleWindow())
	{
		LOG("Destroying log console");

		std::cout.rdbuf(oldStreamBuf);

		g_ConsoleOut.close();

		FreeConsole();
	}

	LOG("Parsing config files");
	ParseEffectsFile();

	g_OptionsManager.Reset();

	ms_Flags.ClearEffectsShortcutEnabled =
	    g_OptionsManager.GetConfigValue({ "EnableClearEffectsShortcut" }, OPTION_DEFAULT_SHORTCUT_CLEAR_EFFECTS);
	ms_Flags.ToggleModShortcutEnabled =
	    g_OptionsManager.GetConfigValue({ "EnableToggleModShortcut" }, OPTION_DEFAULT_SHORTCUT_TOGGLE_MOD);
	ms_Flags.PauseTimerShortcutEnabled =
	    g_OptionsManager.GetConfigValue({ "EnablePauseTimerShortcut" }, OPTION_DEFAULT_SHORTCUT_PAUSE_TIMER);
	ms_Flags.AntiSoftlockShortcutEnabled =
	    g_OptionsManager.GetConfigValue({ "EnableAntiSoftlockShortcut" }, OPTION_DEFAULT_SHORTCUT_ANTI_SOFTLOCK);

	g_EnableGroupWeighting =
	    g_OptionsManager.GetConfigValue({ "EnableGroupWeightingAdjustments" }, OPTION_DEFAULT_GROUP_WEIGHTING);

	const auto &timerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>({ "EffectTimerColor" }, OPTION_DEFAULT_BAR_COLOR));
	const auto &textColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>({ "EffectTextColor" }, OPTION_DEFAULT_TEXT_COLOR));
	const auto &effectTimerColor = ParseConfigColorString(
	    g_OptionsManager.GetConfigValue<std::string>({ "EffectTimedTimerColor" }, OPTION_DEFAULT_TIMED_COLOR));

	g_Random.SetSeed(g_OptionsManager.GetConfigValue({ "Seed" }, 0));
	g_RandomNoDeterm.SetSeed(GetTickCount64());

	std::set<std::string> blacklistedComponentNames;
	if (DoesFeatureFlagExist("blacklistedcomponents"))
	{
		std::ifstream file("chaosmod\\.blacklistedcomponents");
		if (!file.fail())
		{
			std::string line;
			line.resize(64);
			while (file.getline(line.data(), 64))
				blacklistedComponentNames.insert(StringTrim(line.substr(0, line.find("\n"))));
		}
	}

#define INIT_COMPONENT_BASE(componentName, logName, baseComponentType, componentType, ...) \
	do                                                                                     \
	{                                                                                      \
		if (blacklistedComponentNames.contains(componentName))                             \
		{                                                                                  \
			LOG(componentName << " component has been blacklisted from running!");         \
			UninitComponent<baseComponentType>();                                          \
		}                                                                                  \
		else                                                                               \
		{                                                                                  \
			LOG("Initializing " << logName << " component");                               \
			InitComponent<baseComponentType, componentType>(__VA_ARGS__);                  \
		}                                                                                  \
	} while (0)

#define INIT_COMPONENT(componentName, logName, componentType, ...) \
	INIT_COMPONENT_BASE(componentName, logName, componentType, componentType, __VA_ARGS__)

	INIT_COMPONENT("SplashTexts", "mod splash texts handler", SplashTexts);

	INIT_COMPONENT("Workshop", "workshop", Workshop);

	if (DoesFeatureFlagExist("uselegacyeffectsoundmanager"))
	{
		INIT_COMPONENT_BASE("EffectSoundManager", "effect sound system (legacy MCI)", EffectSoundManager,
		                    EffectSoundMCI);
	}
	else
	{
		INIT_COMPONENT_BASE("EffectSoundManager", "effect sound system", EffectSoundManager, EffectSound3D);
	}

	INIT_COMPONENT("MetaModifiers", "meta modifier states", MetaModifiers);

	INIT_COMPONENT("LuaScripts", "Lua scripts", LuaScripts);

	INIT_COMPONENT("EffectDispatcher", "effects dispatcher", EffectDispatcher, textColor, effectTimerColor);

	INIT_COMPONENT("EffectDispatchTimer", "effects dispatch timer", EffectDispatchTimer, timerColor);

	INIT_COMPONENT("DebugMenu", "debug menu", DebugMenu);

	INIT_COMPONENT("EffectShortcuts", "effect shortcuts handler", EffectShortcuts);

	INIT_COMPONENT("KeyStates", "key state handler", KeyStates);

	INIT_COMPONENT("Voting", "voting", Voting, textColor);

	INIT_COMPONENT("Failsafe", "Failsafe", Failsafe);

	INIT_COMPONENT("HelpTextQueue", "script help text queue", HelpTextQueue);

	INIT_COMPONENT("CrossingChallenge", "Crossing Challenge", CrossingChallenge);

#ifdef WITH_DEBUG_PANEL_SUPPORT
	if (DoesFeatureFlagExist("enabledebugsocket"))
		INIT_COMPONENT("DebugSocket", "Debug Websocket", DebugSocket);
#endif

#undef INIT_COMPONENT

	LOG("Completed init");
}

static void MainRun()
{
	if (!ms_Flags.HaveLateHooksRan)
	{
		ms_Flags.HaveLateHooksRan = true;

		Memory::RunLateHooks();
	}

	g_MainThread            = GetCurrentFiber();

	ms_Flags.ToggleModState = g_OptionsManager.GetConfigValue({ "DisableStartup" }, OPTION_DEFAULT_DISABLE_STARTUP);

	if (!g_Components.empty())
	{
		for (auto component : g_Components)
			component->OnModPauseCleanup();

		g_Components.clear();
	}
	ClearEntityPool();

	Init();

	bool isDisabled = false;

	while (true)
	{
		WAIT(0);

		// This will run regardless if mod is disabled
		if (ms_Flags.RunAntiSoftlock)
		{
			ms_Flags.RunAntiSoftlock = false;
			if (IS_SCREEN_FADED_OUT())
			{
				DO_SCREEN_FADE_IN(0);
				SET_ENTITY_HEALTH(PLAYER_PED_ID(), 0, 0);
			}
		}

		if (ms_Flags.ToggleModState || ms_Flags.DisableMod)
		{
			if (!isDisabled)
			{
				isDisabled = true;

				LOG("Mod has been disabled");

				if (ComponentExists<EffectDispatcher>())
				{
					GetComponent<EffectDispatchTimer>()->SetTimerEnabled(false);
					GetComponent<EffectDispatcher>()->Reset(
					    EffectDispatcher::ClearEffectsFlag_NoRestartPermanentEffects);
					while (GetComponent<EffectDispatcher>()->IsClearingEffects())
					{
						GetComponent<EffectDispatcher>()->OnRun();
						WAIT(0);
					}
				}

				ClearEntityPool();

				for (auto component : g_Components)
					component->OnModPauseCleanup();
			}
			else if (ms_Flags.ToggleModState)
			{
				isDisabled = false;

				if (DoesFeatureFlagExist("clearlogfileonreset"))
				{
					// Clear log
					g_Log = std::ofstream(CHAOS_LOG_FILE);
				}

				LOG("Mod has been re-enabled");

				// Restart the main part of the mod completely
				Init();
			}

			ms_Flags.ToggleModState = false;
			ms_Flags.DisableMod     = false;
		}

		if (isDisabled)
			continue;

		if (ms_Flags.ClearAllEffects)
		{
			ms_Flags.ClearAllEffects = false;

			if (ComponentExists<EffectDispatcher>())
			{
				GetComponent<EffectDispatchTimer>()->ResetTimer();
				GetComponent<EffectDispatcher>()->Reset();
				while (GetComponent<EffectDispatcher>()->IsClearingEffects())
				{
					GetComponent<EffectDispatcher>()->OnRun();
					WAIT(0);
				}
			}

			ClearEntityPool();
		}

		if (IS_SCREEN_FADED_OUT())
		{
			// Prevent potential softlock for certain effects
			SET_TIME_SCALE(1.f);
			Hooks::DisableScriptThreadBlock();
			WAIT(100);

			continue;
		}

		for (auto component : g_Components)
			component->OnRun();
		}
	}

namespace Main
{
	void OnRun()
	{
		SetUnhandledExceptionFilter(CrashHandler);

		MainRun();
	}

	void OnCleanup()
	{
		for (auto component : g_Components)
			component->OnModPauseCleanup();
	}

	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore,
	                     BOOL isUpNow)
	{
		static bool isCtrlPressed  = false;
		static bool isShiftPressed = false;

		if (key == VK_CONTROL)
		{
			isCtrlPressed = !isUpNow;
		}
		else if (key == VK_SHIFT)
		{
			isShiftPressed = !isUpNow;
		}
		else if (isCtrlPressed && !wasDownBefore)
		{
			if (key == VK_OEM_MINUS)
			{
				if (ms_Flags.ClearEffectsShortcutEnabled)
				{
					ms_Flags.ClearAllEffects = true;

					if (ComponentExists<SplashTexts>())
						GetComponent<SplashTexts>()->ShowClearEffectsSplash();
				}
			}
			else if (key == VK_OEM_PERIOD)
			{
				if (ms_Flags.PauseTimerShortcutEnabled && ComponentExists<EffectDispatchTimer>())
					GetComponent<EffectDispatchTimer>()->SetTimerEnabled(
					    !GetComponent<EffectDispatchTimer>()->IsTimerEnabled());
			}
			else if (key == VK_OEM_COMMA)
			{
				if (ComponentExists<DebugMenu>() && GetComponent<DebugMenu>()->IsEnabled())
					GetComponent<DebugMenu>()->SetVisible(!GetComponent<DebugMenu>()->IsVisible());
			}
			else if (key == 0x4B) // K
			{
				if (ms_Flags.AntiSoftlockShortcutEnabled && isShiftPressed)
					ms_Flags.RunAntiSoftlock = true;
			}
			else if (key == 0x4C) // L
			{
				if (ms_Flags.ToggleModShortcutEnabled)
					ms_Flags.ToggleModState = true;
			}
		}

		for (auto component : g_Components)
			component->OnKeyInput(key, wasDownBefore, isUpNow, isCtrlPressed, isShiftPressed, isWithAlt);
	}

	void Stop()
	{
		ms_Flags.DisableMod = true;
	}
}
