#include <stdafx.h>

#include "Main.h"

static std::unique_ptr<DebugMenu> ms_pDebugMenu;
static std::unique_ptr<TwitchVoting> ms_pTwitchVoting;
static std::unique_ptr<Failsafe> ms_pFailsafe;
static std::unique_ptr<SplashTexts> ms_pSplashTexts;

static bool ms_bClearAllEffects = false;

static bool ms_bClearEffectsShortcutEnabled = false;

static bool ms_bDisableDrawTimerBar = false;
static bool ms_bDisableDrawEffectTexts = false;

static bool ms_bToggleModShortcutEnabled = false;

static bool ms_bDisableMod = false;

static bool ms_bEnablePauseTimerShortcut = false;

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
	g_CurrentEffectGroupMemberCount = g_AllEffectGroupMemberCount;

	OptionsFile effectsFile("chaosmod/effects.ini");

	for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
	{
		EffectType effectType = static_cast<EffectType>(i);
		const EffectInfo& effectInfo = g_EffectsMap.at(effectType);

		// Default EffectData values
		// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting
		std::vector<int> rgValues { true, static_cast<int>(EEffectTimedType::Unk), -1, 5, false, false };
		// HACK: Store EffectCustomName seperately
		std::string szValueEffectName;

		std::string szValue = effectsFile.ReadValueString(effectInfo.Id);

		if (!szValue.empty())
		{
			size_t ullSplitIndex = szValue.find(",");
			for (int j = 0; ; j++)
			{
				if (j == 6)
				{
					// HACK for EffectCustomName :(
					if (szValue != "0")
					{
						szValueEffectName = szValue;
					}

					break;
				}
				else
				{
					const std::string& szSplit = szValue.substr(0, ullSplitIndex);

					Util::TryParse<int>(szSplit, rgValues[j]);
				}

				if (ullSplitIndex == szValue.npos)
				{
					break;
				}

				szValue = szValue.substr(ullSplitIndex + 1);

				ullSplitIndex = szValue.find(",");
			}
		}

		if (!rgValues[0]) // enabled == false
		{
			if (effectInfo.EffectGroupType != EffectGroupType::None)
			{
				g_CurrentEffectGroupMemberCount[effectInfo.EffectGroupType]--;
			}

			continue;
		}

		EffectData effectData;
		if (!effectInfo.IsTimed)
		{
			effectData.TimedType = EEffectTimedType::NotTimed;
		}
		else if (rgValues[4])
		{
			effectData.TimedType = EEffectTimedType::Permanent;
		}
		else if (rgValues[2] > -1)
		{
			effectData.TimedType = EEffectTimedType::Custom;
			effectData.CustomTime = rgValues[2];
		}
		else
		{
			effectData.TimedType = static_cast<EEffectTimedType>(static_cast<EEffectTimedType>(rgValues[1]) == EEffectTimedType::Unk ? effectInfo.IsShortDuration : rgValues[1]);
		}
		
		effectData.WeightMult = rgValues[3];
		effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
		effectData.ExcludedFromVoting = rgValues[5];
		effectData.IsMeta = effectInfo.ExecutionType == EffectExecutionType::META;
		effectData.Name = effectInfo.Name;
		if (!szValueEffectName.empty())
		{
			effectData.HasCustomName = true;
			effectData.CustomName = szValueEffectName;
		}
		effectData.Id = effectInfo.Id;
		
		for (EffectType effectType : effectInfo.IncompatibleWith)
		{
			effectData.IncompatibleIds.push_back(g_EffectsMap.at(effectType).Id);
		}

		effectData.EffectGroupType = effectInfo.EffectGroupType;

		g_EnabledEffects.emplace(effectType, effectData);
	}
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

	ms_pSplashTexts.reset();

	ClearEntityPool();
}

static void Loop()
{
	g_MainThread = GetCurrentFiber();

	EffectThreads::ClearThreads();

	Reset();

	Main::Init();

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
						Main::Init();
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

		if (!ms_bDisableDrawTimerBar && !g_MetaInfo.m_bShouldHideChaosUI && !g_MetaInfo.m_bDisableChaos)
		{
			g_pEffectDispatcher->DrawTimerBar();
		}
		if (!ms_bDisableDrawEffectTexts)
		{
			g_pEffectDispatcher->DrawEffectTexts();
		}
	}
}

namespace Main
{
	void Init()
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

		ms_bDisableDrawTimerBar = g_OptionsManager.GetConfigValue<bool>("DisableTimerBarDraw", OPTION_DEFAULT_NO_EFFECT_BAR);
		ms_bDisableDrawEffectTexts = g_OptionsManager.GetConfigValue<bool>("DisableEffectTextDraw", OPTION_DEFAULT_NO_TEXT_DRAW);

		g_EnableGroupWeighting = g_OptionsManager.GetConfigValue<bool>("EnableGroupWeightingAdjustments", OPTION_DEFAULT_GROUP_WEIGHTING);

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

		ms_pSplashTexts = std::make_unique<SplashTexts>();
		if (ms_pTwitchVoting->IsEnabled())
		{
			ms_pSplashTexts->ShowTwitchVotingSplash();
		}

		LOG("Completed Init!");
	}

	void RunLoop()
	{
		__try
		{
			Loop();
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

		if (ms_pDebugMenu && ms_pDebugMenu->IsEnabled() && ms_pDebugMenu->IsVisible())
		{
			ms_pDebugMenu->HandleInput(ulKey, bWasDownBefore);
		}
	}
}