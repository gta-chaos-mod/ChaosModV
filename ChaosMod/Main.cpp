#include "stdafx.h"

#include "Main.h"

std::array<int, 3> ParseColor(const std::string& colorText)
{
	// Format: #ARGB

	std::array<int, 3> colors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		 TryParse<int>(colorText.substr(i, 2), colors[j++], 16);
	}

	return colors;
}

static void ParseEffectsFile()
{
	g_enabledEffects.clear();

	OptionsFile effectsFile("chaosmod/effects.ini");

	for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
	{
		EffectType effectType = static_cast<EffectType>(i);
		const EffectInfo& effectInfo = g_effectsMap.at(effectType);

		// Default EffectData values
		// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting
		std::vector<int> values { true, static_cast<int>(EffectTimedType::TIMED_UNK), -1, 5, false, false };
		// HACK: Store EffectCustomName seperately
		std::string valueEffectName;

		std::string value = effectsFile.ReadValueString(effectInfo.Id);

		if (!value.empty())
		{
			int splitIndex = value.find(",");
			for (int j = 0; ; j++)
			{
				if (j == 6)
				{
					// HACK for EffectCustomName :(
					if (value != "0")
					{
						valueEffectName = value;
					}

					break;
				}
				else
				{
					const std::string& split = value.substr(0, splitIndex);

					TryParse<int>(split, values[j]);
				}

				if (splitIndex == value.npos)
				{
					break;
				}

				value = value.substr(splitIndex + 1);

				splitIndex = value.find(",");
			}
		}

		if (!values[0]) // enabled == false?
		{
			continue;
		}

		EffectData effectData;
		if (!effectInfo.IsTimed)
		{
			effectData.TimedType = EffectTimedType::TIMED_NOTTIMED;
		}
		else if (values[4])
		{
			effectData.TimedType = EffectTimedType::TIMED_PERMANENT;
		}
		else if (values[2] > -1)
		{
			effectData.TimedType = EffectTimedType::TIMED_CUSTOM;
			effectData.CustomTime = values[2];
		}
		else
		{
			effectData.TimedType = static_cast<EffectTimedType>(static_cast<EffectTimedType>(values[1]) == EffectTimedType::TIMED_UNK ? effectInfo.IsShortDuration : values[1]);
		}
		
		effectData.WeightMult = values[3];
		effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
		effectData.ExcludedFromVoting = values[5];
		effectData.IsMeta = effectInfo.ExecutionType == EffectExecutionType::META;
		effectData.Name = effectInfo.Name;
		if (!valueEffectName.empty())
		{
			effectData.HasCustomName = true;
			effectData.CustomName = valueEffectName;
		}
		effectData.Id = effectInfo.Id;
		
		for (EffectType effectType : effectInfo.IncompatibleWith)
		{
			effectData.IncompatibleIds.push_back(g_effectsMap.at(effectType).Id);
		}

		effectData.EffectGroup = effectInfo.EffectGroup;

		g_enabledEffects.emplace(effectType, effectData);
	}
}

void Main::Init()
{
	static std::streambuf* oldStreamBuf;
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

			oldStreamBuf = std::cout.rdbuf();

			g_consoleOut = std::ofstream("CONOUT$");
			std::cout.rdbuf(g_consoleOut.rdbuf());

			std::cout.clear();
		}
	}
	else if (GetConsoleWindow())
	{
		LOG("Destroying log console");

		std::cout.rdbuf(oldStreamBuf);

		g_consoleOut.close();

		FreeConsole();
	}

	LOG("Parsing config files");
	ParseEffectsFile();
	
	g_optionsManager.Reset();

	m_clearEffectsShortcutEnabled = g_optionsManager.GetConfigValue<bool>("EnableClearEffectsShortcut", OPTION_DEFAULT_SHORTCUT_CLEAR_EFFECTS);
	m_toggleModShortcutEnabled = g_optionsManager.GetConfigValue<bool>("EnableToggleModShortcut", OPTION_DEFAULT_SHORTCUT_TOGGLE_MOD);
	m_enablePauseTimerShortcut = g_optionsManager.GetConfigValue<bool>("EnablePauseTimerShortcut", OPTION_DEFAULT_SHORTCUT_PAUSE_TIMER);

	m_enableDebugMenu = g_optionsManager.GetConfigValue<bool>("EnableDebugMenu", OPTION_DEFAULT_DEBUG_MENU);

	m_disableDrawTimerBar = g_optionsManager.GetConfigValue<bool>("DisableTimerBarDraw", OPTION_DEFAULT_NO_EFFECT_BAR);
	m_disableDrawEffectTexts = g_optionsManager.GetConfigValue<bool>("DisableEffectTextDraw", OPTION_DEFAULT_NO_TEXT_DRAW);

	const std::array<int, 3>& timerColor = ParseColor(g_optionsManager.GetConfigValue<std::string>("EffectTimerColor", OPTION_DEFAULT_BAR_COLOR));
	const std::array<int, 3>& textColor = ParseColor(g_optionsManager.GetConfigValue<std::string>("EffectTextColor", OPTION_DEFAULT_TEXT_COLOR));
	const std::array<int, 3>& effectTimerColor = ParseColor(g_optionsManager.GetConfigValue<std::string>("EffectTimedTimerColor", OPTION_DEFAULT_TIMED_COLOR));

	LOG("Running custom scripts");
	LuaManager::Load();

	g_random.SetSeed(g_optionsManager.GetConfigValue<int>("Seed", 0));

	LOG("Initializing effects dispatcher");
	g_effectDispatcher = std::make_unique<EffectDispatcher>(timerColor, textColor, effectTimerColor);

	if (m_enableDebugMenu)
	{
		m_debugMenu = std::make_unique<DebugMenu>();
	}

	LOG("Initializing Twitch voting");
	m_twitchVoting = std::make_unique<TwitchVoting>();

	LOG("Completed Init!");
}

void Main::Reset()
{
	g_effectDispatcher.reset();

	if (m_enableDebugMenu)
	{
		m_debugMenu.reset();
	}

	m_twitchVoting.reset();

	ClearEntityPool();
}

void Main::RunLoop()
{
	__try
	{
		Loop();
	}
	__except (CrashHandler(GetExceptionInformation()))
	{

	}
}

void Main::Loop()
{
	int splashTextTime = 15000;
	int twitchVotingWarningTextTime = 15000;

	DWORD64 lastTick = GetTickCount64();

	g_mainThread = GetCurrentFiber();

	ThreadManager::ClearThreads();

	FailsafeManager::Reset();

	Reset();

	Init();

	while (true)
	{
		WAIT(0);

		if (!ThreadManager::IsAnyThreadRunningOnStart())
		{
			static bool justReenabled = false;
			if (m_disableMod && !justReenabled)
			{
				if (!justReenabled)
				{
					justReenabled = true;

					Reset();
				}

				continue;
			}
			else
			{
				if (justReenabled)
				{
					if (ThreadManager::IsAnyThreadRunning())
					{
						ThreadManager::RunThreads();

						continue;
					}
					else if (!m_disableMod)
					{
						justReenabled = false;

						// Clear log
						g_log = std::ofstream("chaosmod/chaoslog.txt");

						// Restart the main part of the mod completely
						Init();
					}
					else
					{
						continue;
					}
				}
			}

			FailsafeManager::Run();

			if (m_clearAllEffects)
			{
				m_clearAllEffects = false;

				g_effectDispatcher->Reset();

				ClearEntityPool();
			}

			if (m_enableDebugMenu && m_debugMenu->IsVisible())
			{
				// Arrow Up
				DISABLE_CONTROL_ACTION(1, 27, true);
				DISABLE_CONTROL_ACTION(1, 127, true);
				DISABLE_CONTROL_ACTION(1, 188, true);
				DISABLE_CONTROL_ACTION(1, 300, true);
				// Arrow Down
				DISABLE_CONTROL_ACTION(1, 173, true);
				DISABLE_CONTROL_ACTION(1, 187, true);
				DISABLE_CONTROL_ACTION(1, 299, true);
				// Enter
				DISABLE_CONTROL_ACTION(1, 18, true);
				DISABLE_CONTROL_ACTION(1, 176, true);
				DISABLE_CONTROL_ACTION(1, 191, true);
				DISABLE_CONTROL_ACTION(1, 201, true);
				DISABLE_CONTROL_ACTION(1, 215, true);
				// Backspace
				DISABLE_CONTROL_ACTION(1, 177, true);
				DISABLE_CONTROL_ACTION(1, 194, true);
				DISABLE_CONTROL_ACTION(1, 202, true);

				m_debugMenu->Tick();
			}
		}
		else if (IS_SCREEN_FADED_OUT())
		{
			SET_TIME_SCALE(1.f); // Prevent potential softlock for certain effects

			WAIT(100);

			continue;
		}

		DWORD64 curTick = GetTickCount64();

		if (m_clearEffectsTextTime > 0)
		{
			DrawScreenText("Effects Cleared!", { .86f, .86f }, .8f, { 255, 100, 100 }, true);
			
			m_clearEffectsTextTime -= curTick - lastTick;
		}

		if (splashTextTime > 0)
		{
			DrawScreenText("Chaos Mod v1.9.1.1 by pongo1231\n\nSee credits.txt for list of contributors",
				{ .2f, .3f }, .65f, { 60, 245, 190 }, true);

#ifdef _DEBUG
			DrawScreenText("DEBUG BUILD!", { .2f, .5f }, .7f, { 255, 0, 0 }, true);
#endif

			splashTextTime -= curTick - lastTick;
		}

		if (m_twitchVoting->IsEnabled() && twitchVotingWarningTextTime > 0)
		{
			DrawScreenText("Twitch Voting Enabled!", { .86f, .7f }, .8f, { 255, 100, 100 }, true);

			twitchVotingWarningTextTime -= curTick - lastTick;
		}

		g_effectDispatcher->UpdateEffects();

		if (m_twitchVoting->IsEnabled())
		{
			m_twitchVoting->Tick();
		}

		if (!m_pauseTimer)
		{
			if (!g_metaInfo.DisableChaos)
			{
				g_effectDispatcher->UpdateTimer();
			}

			g_effectDispatcher->UpdateMetaEffects();
		}

		if (!m_disableDrawTimerBar && !g_metaInfo.ShouldHideChaosUI && !g_metaInfo.DisableChaos)
		{
			g_effectDispatcher->DrawTimerBar();
		}
		if (!m_disableDrawEffectTexts)
		{
			g_effectDispatcher->DrawEffectTexts();
		}

		lastTick = curTick;
	}
}

void Main::OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	static bool CTRLpressed = false;

	if (key == VK_CONTROL)
	{
		CTRLpressed = !isUpNow;
	}
	else if (CTRLpressed && !wasDownBefore)
	{
		if (key == VK_OEM_MINUS && m_clearEffectsShortcutEnabled)
		{
			m_clearAllEffects = true;
			m_clearEffectsTextTime = 5000;
		}
		else if (key == VK_OEM_PERIOD && m_enablePauseTimerShortcut)
		{
			m_pauseTimer = !m_pauseTimer;
		}
		else if (key == VK_OEM_COMMA && m_enableDebugMenu && m_debugMenu)
		{
			m_debugMenu->SetVisible(!m_debugMenu->IsVisible());
		}
		else if (key == 0x4C && m_toggleModShortcutEnabled) // L
		{
			m_disableMod = !m_disableMod;
		}
	}

	if (m_enableDebugMenu && m_debugMenu && m_debugMenu->IsVisible())
	{
		m_debugMenu->HandleInput(key, wasDownBefore);
	}
}
