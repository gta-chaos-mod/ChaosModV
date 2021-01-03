#include "stdafx.h"

#include "Main.h"

std::array<int, 3> ParseColor(const std::string& colorText)
{
	// Format: #ARGB

	std::array<int, 3> colors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		 TryParseInt(colorText.substr(i, 2), colors[j++], 16);
	}

	return colors;
}

void ParseConfigFile(int& effectSpawnTime, int& effectTimedDur, int& seed, int& effectTimedShortDur, bool& enableClearEffectsShortcut, bool& disableEffectsTwiceInRow,
	bool& disableTimerDrawing, bool& disableEffectTextDrawing, bool& enableToggleModShortcut, bool& enableDebugMenu, bool& enablePauseTimerShortcut, std::array<int, 3>& timerColor,
	std::array<int, 3>& textColor, std::array<int, 3>& effectTimerColor)
{
	OptionsFile configFile("chaosmod/config.ini");

	effectSpawnTime = configFile.ReadValueInt("NewEffectSpawnTime", 30);
	effectTimedDur = configFile.ReadValueInt("EffectTimedDur", 90);
	seed = configFile.ReadValueInt("Seed", 0);
	effectTimedShortDur = configFile.ReadValueInt("EffectTimedShortDur", 30);
	enableClearEffectsShortcut = configFile.ReadValueInt("EnableClearEffectsShortcut", true);
	disableEffectsTwiceInRow = configFile.ReadValueInt("DisableEffectTwiceInRow", false);
	disableTimerDrawing = configFile.ReadValueInt("DisableTimerBarDraw", false);
	disableEffectTextDrawing = configFile.ReadValueInt("DisableEffectTextDraw", false);
	enableToggleModShortcut = configFile.ReadValueInt("EnableToggleModShortcut", true);
	enableDebugMenu = configFile.ReadValueInt("EnableDebugMenu", false);
	enablePauseTimerShortcut = configFile.ReadValueInt("EnablePauseTimerShortcut", false);
	timerColor = ParseColor(configFile.ReadValue("EffectTimerColor", "#FF4040FF"));
	textColor = ParseColor(configFile.ReadValue("EffectTextColor", "#FFFFFFFF"));
	effectTimerColor = ParseColor(configFile.ReadValue("EffectTimedTimerColor", "#FFB4B4B4"));
}

void ParseTwitchFile(bool& enableTwitchVoting, int& twitchSecsBeforeVoting, TwitchOverlayMode& twitchOverlayMode, bool& enableTwitchChanceSystem,
	bool& enableVotingChanceSystemRetainChance, bool& enableTwitchRandomEffectVoteable)
{
	OptionsFile twitchFile("chaosmod/twitch.ini");

	enableTwitchVoting = twitchFile.ReadValueInt("EnableTwitchVoting", false);
	twitchSecsBeforeVoting = twitchFile.ReadValueInt("TwitchVotingSecsBeforeVoting", 0);
	twitchOverlayMode = static_cast<TwitchOverlayMode>(twitchFile.ReadValueInt("TwitchVotingOverlayMode", 0));
	enableTwitchChanceSystem = twitchFile.ReadValueInt("TwitchVotingChanceSystem", false);
	enableVotingChanceSystemRetainChance = twitchFile.ReadValueInt("TwitchVotingChanceSystemRetainChance", true);
	enableTwitchRandomEffectVoteable = twitchFile.ReadValueInt("TwitchRandomEffectVoteableEnable", true);
}

void ParseEffectsFile()
{
	std::map<EffectType, EffectData> enabledEffects;

	OptionsFile effectsFile("chaosmod/effects.ini");

	for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
	{
		EffectType effectType = static_cast<EffectType>(i);
		const EffectInfo& effectInfo = g_effectsMap.at(effectType);

		// Default EffectData values
		// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting
		std::vector<int> values { true, static_cast<int>(EffectTimedType::TIMED_DEFAULT), -1, 5, false, false };
		// HACK: Store EffectCustomName seperately
		std::string valueEffectName = effectInfo.Name;

		std::string value = effectsFile.ReadValue(effectInfo.Id);

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

					TryParseInt(split, values[j]);
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
		effectData.TimedType = static_cast<EffectTimedType>(static_cast<EffectTimedType>(values[1]) == EffectTimedType::TIMED_DEFAULT ? effectInfo.IsShortDuration : values[1]);
		effectData.CustomTime = values[2];
		effectData.WeightMult = values[3];
		effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
		effectData.Permanent = values[4];
		effectData.ExcludedFromVoting = values[5];
		effectData.Name = valueEffectName;

		enabledEffects.emplace(effectType, effectData);

		static std::ofstream log("chaosmod/enabledeffectslog.txt");
		log << effectInfo.Name << std::endl;
	}

	g_enabledEffects = enabledEffects;
}

void Main::Init()
{
	int effectSpawnTime, effectTimedDur, seed, effectTimedShortDur, twitchSecsBeforeChatVoting;
	bool disableEffectsTwiceInRow, enableTwitchVoting, enableTwitchChanceSystem, enableVotingChanceSystemRetainChance, enableTwitchRandomEffectVoteable;
	std::array<int, 3> timerColor, textColor, effectTimerColor;
	TwitchOverlayMode twitchOverlayMode;

	ParseConfigFile(effectSpawnTime, effectTimedDur, seed, effectTimedShortDur, m_clearEffectsShortcutEnabled, disableEffectsTwiceInRow, m_disableDrawTimerBar,
		m_disableDrawEffectTexts, m_toggleModShortcutEnabled, m_enableDebugMenu, m_enablePauseTimerShortcut, timerColor, textColor, effectTimerColor);
	ParseTwitchFile(enableTwitchVoting, twitchSecsBeforeChatVoting, twitchOverlayMode, enableTwitchChanceSystem, enableVotingChanceSystemRetainChance,
		enableTwitchRandomEffectVoteable);
	ParseEffectsFile();

	g_random.SetSeed(seed);

	g_effectDispatcher = std::make_unique<EffectDispatcher>(effectSpawnTime, effectTimedDur, effectTimedShortDur, disableEffectsTwiceInRow, timerColor, textColor, effectTimerColor,
		enableTwitchVoting, twitchOverlayMode);

	if (m_enableDebugMenu)
	{
		std::vector<EffectType> enabledEffectTypes;
		for (const auto& pair : g_enabledEffects)
		{
			if (!pair.second.Permanent)
			{
				enabledEffectTypes.push_back(pair.first);
			}
		}

		m_debugMenu = std::make_unique<DebugMenu>(enabledEffectTypes);
	}

	struct stat temp;
	bool enableTwitchPollVoting = stat("chaosmod/.twitchpoll", &temp) != -1 && false; // disable polls for now
	m_twitchVoting = std::make_unique<TwitchVoting>(enableTwitchVoting, twitchSecsBeforeChatVoting, enableTwitchPollVoting, twitchOverlayMode, enableTwitchChanceSystem,
		enableVotingChanceSystemRetainChance, enableTwitchRandomEffectVoteable);
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

	Init(); // Restart the main part of the mod completely
}

void Main::Loop()
{
	int splashTextTime = 15000;
	int twitchVotingWarningTextTime = 15000;

	DWORD64 lastTick = GetTickCount64();

	g_mainThread = GetCurrentFiber();

	ThreadManager::ClearThreads();

	Reset();

	while (true)
	{
		WAIT(0);

		if (!ThreadManager::IsAnyThreadRunningOnStart())
		{
			static bool justReenabled = false;
			if (m_disableMod)
			{
				if (!justReenabled)
				{
					justReenabled = true;

					Reset();
				}

				continue;
			}
			else if (justReenabled)
			{
				justReenabled = false;
			}

			if (m_clearAllEffects)
			{
				m_clearAllEffects = false;

				g_effectDispatcher->Reset();

				ClearEntityPool();
			}

			if (m_debugMenu && m_debugMenu->IsVisible())
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
			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Effects Cleared!");
			SET_TEXT_SCALE(.8f, .8f);
			SET_TEXT_COLOUR(255, 100, 100, 255);
			SET_TEXT_CENTRE(true);
			END_TEXT_COMMAND_DISPLAY_TEXT(.86f, .86f, 0);
			
			m_clearEffectsTextTime -= curTick - lastTick;
		}

		if (splashTextTime > 0)
		{
			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Chaos Mod v1.8.2.1 by pongo1231\n\nSee credits.txt for list of contributors");
			SET_TEXT_SCALE(.65f, .65f);
			SET_TEXT_COLOUR(0, 255, 255, 255);
			SET_TEXT_CENTRE(true);
			END_TEXT_COMMAND_DISPLAY_TEXT(.2f, .3f, 0);

			splashTextTime -= curTick - lastTick;
		}

		if (m_twitchVoting->IsEnabled() && twitchVotingWarningTextTime > 0)
		{
			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Twitch Voting Enabled!");
			SET_TEXT_SCALE(.8f, .8f);
			SET_TEXT_COLOUR(255, 100, 100, 255);
			SET_TEXT_CENTRE(true);
			END_TEXT_COMMAND_DISPLAY_TEXT(.86f, .7f, 0);

			twitchVotingWarningTextTime -= curTick - lastTick;
		}

		g_effectDispatcher->UpdateEffects();

		if (m_twitchVoting->IsEnabled())
		{
			m_twitchVoting->Tick();
		}

		if (!m_pauseTimer)
		{
			g_effectDispatcher->UpdateTimer();
		}

		if (!m_disableDrawTimerBar)
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