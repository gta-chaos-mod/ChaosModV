#include <stdafx.h>

#include "Main.h"

static std::unique_ptr<DebugMenu> ms_pDebugMenu;
static std::unique_ptr<TwitchVoting> ms_pTwitchVoting;
static std::unique_ptr<Failsafe> ms_pFailsafe;
static std::unique_ptr<SplashTexts> ms_pSplashTexts;

static bool ms_bClearAllEffects = false;

static bool ms_bClearEffectsShortcutEnabled = false;

static bool ms_bToggleModShortcutEnabled = false;

static bool ms_bDisableMod = false;

static bool ms_bEnablePauseTimerShortcut = false;

static bool ms_bHaveLateHooksRan = false;

static _NODISCARD std::array<BYTE, 3> ParseConfigColorString(const std::string& szColorText)
{
	// Format: #ARGB
	std::array<BYTE, 3> rgColors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		 Util::TryParse<BYTE>(szColorText.substr(i, 2), rgColors[j++], 16);
	}

	return rgColors;
}

static void ParseEffectsFile()
{
	g_EnabledEffects.clear();
	g_dictCurrentEffectGroupMemberCount = g_dictAllEffectGroupMemberCount;

	EffectConfig::ReadConfig("chaosmod/effects.ini", g_EnabledEffects);
}

static void Reset()
{
	// Check if this isn't the first time this is being run
	if (g_pEffectDispatcher)
	{
		LOG("Mod has been disabled using shortcut!");
	}

	g_pEffectDispatcher.reset();

	ms_pDebugMenu.reset();

	ms_pTwitchVoting.reset();

	ms_pFailsafe.reset();

	ClearEntityPool();
}

static void Init()
{
	static std::streambuf* c_pOldStreamBuf;
	if (DoesFileExist("chaosmod\\.enableconsole"))
	{
		if (GetConsoleWindow())
		{
			system("cls");
		}
		else
		{
			LOG("Creating log console");

			AllocConsole();

			SetConsoleTitle("Chaos Mod");
			DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

			c_pOldStreamBuf = std::cout.rdbuf();

			g_ConsoleOut = std::ofstream("CONOUT$");
			std::cout.rdbuf(g_ConsoleOut.rdbuf());

			std::cout.clear();

			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			DWORD ulConMode;
			GetConsoleMode(handle, &ulConMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ulConMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}
	}
	else if (GetConsoleWindow())
	{
		LOG("Destroying log console");

		std::cout.rdbuf(c_pOldStreamBuf);

		g_ConsoleOut.close();

		FreeConsole();
	}

	LOG("Parsing config files");
	ParseEffectsFile();

	g_OptionsManager.Reset();

	ms_bClearEffectsShortcutEnabled = g_OptionsManager.GetConfigValue<bool>("EnableClearEffectsShortcut", OPTION_DEFAULT_SHORTCUT_CLEAR_EFFECTS);
	ms_bToggleModShortcutEnabled = g_OptionsManager.GetConfigValue<bool>("EnableToggleModShortcut", OPTION_DEFAULT_SHORTCUT_TOGGLE_MOD);
	ms_bEnablePauseTimerShortcut = g_OptionsManager.GetConfigValue<bool>("EnablePauseTimerShortcut", OPTION_DEFAULT_SHORTCUT_PAUSE_TIMER);

	g_bEnableGroupWeighting = g_OptionsManager.GetConfigValue<bool>("EnableGroupWeightingAdjustments", OPTION_DEFAULT_GROUP_WEIGHTING);

	const auto& rgTimerColor = ParseConfigColorString(g_OptionsManager.GetConfigValue<std::string>("EffectTimerColor", OPTION_DEFAULT_BAR_COLOR));
	const auto& rgTextColor = ParseConfigColorString(g_OptionsManager.GetConfigValue<std::string>("EffectTextColor", OPTION_DEFAULT_TEXT_COLOR));
	const auto& rgEffectTimerColor = ParseConfigColorString(g_OptionsManager.GetConfigValue<std::string>("EffectTimedTimerColor", OPTION_DEFAULT_TIMED_COLOR));

	LOG("Running custom scripts");
	LuaScripts::Load();

	g_Random.SetSeed(g_OptionsManager.GetConfigValue<int>("Seed", 0));

	LOG("Initializing effects dispatcher");
	g_pEffectDispatcher = std::make_unique<EffectDispatcher>(rgTimerColor, rgTextColor, rgEffectTimerColor);

	ms_pDebugMenu = std::make_unique<DebugMenu>();

	LOG("Initializing Twitch voting");
	ms_pTwitchVoting = std::make_unique<TwitchVoting>(rgTextColor);

	LOG("Initializing Failsafe");
	ms_pFailsafe = std::make_unique<Failsafe>();

	LOG("Completed Init!");

	if (ms_pTwitchVoting->IsEnabled())
	{
		ms_pSplashTexts->ShowTwitchVotingSplash();
	}
}

static void MainRun()
{
	if (!ms_bHaveLateHooksRan)
	{
		ms_bHaveLateHooksRan = true;

		Memory::RunLateHooks();
	}

	g_MainThread = GetCurrentFiber();

	EffectThreads::ClearThreads();

	Reset();

	ms_pSplashTexts = std::make_unique<SplashTexts>();
	ms_pSplashTexts->ShowInitSplash();

	ms_bDisableMod = g_OptionsManager.GetConfigValue<bool>("DisableStartup", OPTION_DEFAULT_DISABLE_STARTUP);

	Init();

	while (true)
	{
		WAIT(0);

		if (!EffectThreads::IsAnyThreadRunningOnStart())
		{
			static bool c_bJustReenabled = false;
			if (ms_bDisableMod && !c_bJustReenabled)
			{
				if (!c_bJustReenabled)
				{
					c_bJustReenabled = true;

					Reset();
				}

				continue;
			}
			else
			{
				if (c_bJustReenabled)
				{
					if (EffectThreads::IsAnyThreadRunning())
					{
						EffectThreads::RunThreads();

						continue;
					}
					else if (!ms_bDisableMod)
					{
						c_bJustReenabled = false;

						// Clear log
						g_Log = std::ofstream("chaosmod/chaoslog.txt");

						// Restart the main part of the mod completely
						Init();
					}
					else
					{
						continue;
					}
				}
			}

			if (ms_bClearAllEffects)
			{
				ms_bClearAllEffects = false;

				g_pEffectDispatcher->Reset();

				ClearEntityPool();
			}
		}
		else if (IS_SCREEN_FADED_OUT())
		{
			SET_TIME_SCALE(1.f); // Prevent potential softlock for certain effects

			WAIT(100);

			continue;
		}

		for (Component* pComponent : g_rgComponents)
		{
			pComponent->Run();
		}
	}
}

namespace Main
{
	void Run()
	{
		__try
		{
			MainRun();
		}
		__except (CrashHandler(GetExceptionInformation()))
		{

		}
	}

	void OnKeyboardInput(DWORD ulKey, WORD usRepeats, BYTE ucScanCode, BOOL bIsExtended, BOOL bIsWithAlt, BOOL bWasDownBefore, BOOL bIsUpNow)
	{
		static bool c_bIsCtrlPressed = false;

		if (ulKey == VK_CONTROL)
		{
			c_bIsCtrlPressed = !bIsUpNow;
		}
		else if (c_bIsCtrlPressed && !bWasDownBefore)
		{
			if (ulKey == VK_OEM_MINUS && ms_bClearEffectsShortcutEnabled)
			{
				ms_bClearAllEffects = true;

				if (ms_pSplashTexts)
				{
					ms_pSplashTexts->ShowClearEffectsSplash();
				}
			}
			else if (ulKey == VK_OEM_PERIOD && ms_bEnablePauseTimerShortcut && g_pEffectDispatcher)
			{
				g_pEffectDispatcher->m_bPauseTimer = !g_pEffectDispatcher->m_bPauseTimer;
			}
			else if (ulKey == VK_OEM_COMMA && ms_pDebugMenu && ms_pDebugMenu->IsEnabled())
			{
				ms_pDebugMenu->SetVisible(!ms_pDebugMenu->IsVisible());
			}
			else if (ulKey == 0x4C && ms_bToggleModShortcutEnabled) // L
			{
				ms_bDisableMod = !ms_bDisableMod;
			}
		}

		if (ms_pDebugMenu)
		{
			ms_pDebugMenu->HandleInput(ulKey, bWasDownBefore);
		}
	}
}