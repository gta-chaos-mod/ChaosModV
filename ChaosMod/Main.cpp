#include "stdafx.h"

#include "Main.h"
#include "EffectDispatcher.h"
#include "Memory.h"
#include "TwitchVoting.h"
#include "DebugMenu.h"
#include "OptionsFile.h"

std::array<int, 3> ParseColor(const std::string& colorText)
{
	std::array<int, 3> colors;

	int j = 0;
	for (int i = 3; i < 9; i += 2)
	{
		colors[j++] = std::strtol(colorText.substr(i, 2).c_str(), nullptr, 16);
	}

	return colors;
}

// TODO: Parse twitch.ini with appropriate options
OptionsFileStatus ParseConfigFile(int& effectSpawnTime, int& effectTimedDur, int& seed, int& effectTimedShortDur, bool& enableClearEffectsShortcut,
	bool& disableEffectsTwiceInRow, bool& disableTimerDrawing, bool& disableEffectTextDrawing, std::array<int, 3>& timerColor, std::array<int, 3>& textColor,
	std::array<int, 3>& effectTimerColor, bool& enableTwitchVoting, int& twitchVotingNoVoteChance, int& twitchSecsBeforeVoting, bool& enableTwitchVoterIndicator,
	bool& enableToggleModShortcut, bool& enableTwitchVoteablesOnscreen)
{
	OptionsFile configFile("chaosmod/config.ini");

	effectSpawnTime = configFile.ReadValueInt("NewEffectSpawnTime", 30);
	effectTimedDur = configFile.ReadValueInt("EffectTimedDur", 90);
	seed = configFile.ReadValueInt("Seed", 0);
	effectTimedShortDur = configFile.ReadValueInt("EffectTimedShortDur", 30);
	enableTwitchVoting = configFile.ReadValueInt("EnableTwitchVoting", false);
	twitchVotingNoVoteChance = configFile.ReadValueInt("TwitchVotingNoVoteChance", 50);
	twitchSecsBeforeVoting = configFile.ReadValueInt("TwitchVotingSecsBeforeVoting", 0);
	enableTwitchVoterIndicator = configFile.ReadValueInt("TwitchVotingVoterIndicator", false);
	enableTwitchVoteablesOnscreen = configFile.ReadValueInt("TwitchVotingShowVoteablesOnscreen", false);
	enableClearEffectsShortcut = configFile.ReadValueInt("EnableClearEffectsShortcut", true);
	disableEffectsTwiceInRow = configFile.ReadValueInt("DisableEffectTwiceInRow", false);
	disableTimerDrawing = configFile.ReadValueInt("DisableTimerBarDraw", false);
	disableEffectTextDrawing = configFile.ReadValueInt("DisableEffectTextDraw", false);
	enableToggleModShortcut = configFile.ReadValueInt("EnableToggleModShortcut", true);
	timerColor = ParseColor(configFile.ReadValue("EffectTimerColor", "40,40,255"));
	textColor = ParseColor(configFile.ReadValue("EffectTextColor", "255,255,255"));
	effectTimerColor = ParseColor(configFile.ReadValue("EffectTimedTimerColor", "180,180,180"));

	return configFile.GetStatus();
}

// TODO: Convert to new config system
int ParseEffectsFile(std::map<EffectType, std::array<int, 4>>& enabledEffects)
{
	static constexpr const char* FILE_PATH = "chaosmod/effects.ini";

	struct stat temp;
	if (stat(FILE_PATH, &temp) == -1)
	{
		return -1;
	}

	std::ifstream config(FILE_PATH);

	if (config.fail())
	{
		return -2;
	}

	// Fill with all effecttypes first
	for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
	{
		enabledEffects.emplace(std::make_pair<EffectType, std::array<int, 4>>(static_cast<EffectType>(i), { g_effectsMap.at(static_cast<EffectType>(i)).IsShortDuration, -1, 5, 0 }));
	}

	// Remove disabled effecttypes
	char buffer[128];
	while (config.getline(buffer, 128))
	{
		std::string line(buffer);
		std::string key = line.substr(0, line.find("="));
		if (line == key)
		{
			continue;
		}

		std::string value = line.substr(line.find("=") + 1);
		std::array<int, 5> values { 1, -1, -1, 5, 0 };

		int splitIndex = value.find(",");
		if (splitIndex == value.npos)
		{
			values[0] = std::stoi(value);
		}
		else
		{
			for (int i = 0; ; i++)
			{
				values[i] = std::stoi(value.substr(0, splitIndex));
				value = value.substr(splitIndex + 1);

				if (splitIndex == value.npos)
				{
					break;
				}

				splitIndex = value.find(",");
			}
		}

		// Map id to EffectType
		for (const auto pair : g_effectsMap)
		{
			if (pair.second.Id == key)
			{
				auto result = enabledEffects.find(pair.first);
				if (result != enabledEffects.end())
				{
					if (!values[0])
					{
						enabledEffects.erase(result);
						break;
					}

					EffectInfo effectInfo = g_effectsMap.at(result->first);

					result->second[0] = values[1] == -1 ? effectInfo.IsShortDuration : values[1];
					result->second[1] = values[2];
					result->second[2] = values[3];
					result->second[3] = values[4];

					static std::ofstream log("chaosmod/enabledeffectslog.txt");
					log << effectInfo.Name << " " << result->second[0] << " " << result->second[1] << " " << result->second[2] << " " << result->second[3] << std::endl;
				}
				break;
			}
		}
	}

	return 0;
}

bool Main::Init()
{
	int effectSpawnTime = 30;
	int effectTimedDur = 90;
	int seed = -1;
	int effectTimedShortDur = 30;
	bool disableEffectsTwiceInRow = false;
	std::array<int, 3> timerColor = { 40, 40, 255 };
	std::array<int, 3> textColor = { 255, 255, 255 };
	std::array<int, 3> effectTimerColor = { 180, 180, 180 };
	std::map<EffectType, std::array<int, 4>> enabledEffects;
	bool enableTwitchVoting = false;
	int twitchVotingNoVoteChance = 50;
	int twitchSecsBeforeChatVoting = 0;
	bool enableTwitchVoterIndicator = false;
	bool enableTwitchVoteablesOnscreen = false;

	struct stat temp;
	bool enableTwitchPollVoting = stat("chaosmod/.twitchpoll", &temp) != -1;

	int result;
	if ((result = ParseConfigFile(effectSpawnTime, effectTimedDur, seed, effectTimedShortDur, m_clearEffectsShortcutEnabled, disableEffectsTwiceInRow,
		m_disableDrawTimerBar, m_disableDrawEffectTexts, timerColor, textColor, effectTimerColor, enableTwitchVoting, twitchVotingNoVoteChance, twitchSecsBeforeChatVoting,
		enableTwitchVoterIndicator, m_toggleModShortcutEnabled, enableTwitchVoteablesOnscreen)) || (result = ParseEffectsFile(enabledEffects)))
	{
		switch (result)
		{
		case -1:
			MessageBox(NULL,
				"Config File was either not found or is invalid. Please make sure you've run the config tool at least once, otherwise try regenerating it.",
				"ChaosModV Error", MB_OK | MB_ICONERROR);
			break;
		case -2:
			MessageBox(NULL,
				"Config File couldn't be opened for reading. Make sure there's no process running with a handle on chaosmod/config.ini or chaosmod/effects.ini",
				"ChaosModV Error", MB_OK | MB_ICONERROR);
			break;
		}
		return false;
	}

	if (enableTwitchVoting && stat("chaosmod/.twitchmode", &temp) == -1)
	{
		enableTwitchVoting = false;
	}

	Random::SetSeed(seed);

	g_effectDispatcher = std::make_unique<EffectDispatcher>(effectSpawnTime, effectTimedDur, enabledEffects, effectTimedShortDur, disableEffectsTwiceInRow,
		timerColor, textColor, effectTimerColor, enableTwitchVoteablesOnscreen);

#ifdef _DEBUG
	std::vector<EffectType> enabledEffectTypes;
	for (auto pair : enabledEffects)
	{
		enabledEffectTypes.push_back(pair.first);
	}

	m_debugMenu = std::make_unique<DebugMenu>(enabledEffectTypes);
#endif

	m_twitchVoting = std::make_unique<TwitchVoting>(enableTwitchVoting, twitchVotingNoVoteChance, twitchSecsBeforeChatVoting, enableTwitchPollVoting, enableTwitchVoterIndicator,
		enableTwitchVoteablesOnscreen, enabledEffects);

	return true;
}

void Main::MainLoop()
{
	int twitchVotingWarningTextTime = 15000;

	DWORD64 lastTick = GetTickCount64();

	while (true)
	{
		WAIT(0);

		if (IS_SCREEN_FADED_OUT() || m_disableMod)
		{
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

		lastTick = curTick;

		if (!m_disableDrawTimerBar)
		{
			g_effectDispatcher->DrawTimerBar();
		}
		if (!m_disableDrawEffectTexts)
		{
			g_effectDispatcher->DrawEffectTexts();
		}

#ifdef _DEBUG
		if (m_debugMenu->IsVisible())
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
#endif
	}
}

void Main::RunEffectLoop()
{
	g_effectDispatcher->Reset();

	while (true)
	{
		WAIT(0);

		if (IS_SCREEN_FADED_OUT())
		{
			SET_TIME_SCALE(1.f);

			WAIT(100);

			continue;
		}

		static bool justReenabled = false;
		if (m_disableMod)
		{
			justReenabled = true;

			g_effectDispatcher->ClearEffects();
			g_effectDispatcher->ResetTimer();
		}
		else if (justReenabled)
		{
			justReenabled = false;

			g_effectDispatcher->Reset();
		}

		if (m_clearAllEffects)
		{
			m_clearAllEffects = false;

			g_effectDispatcher->Reset();
		}

		if (m_twitchVoting->IsEnabled())
		{
			m_twitchVoting->Tick();
		}

		if (!m_pauseTimer)
		{
			g_effectDispatcher->UpdateTimer();
		}

		g_effectDispatcher->UpdateEffects();
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
#ifdef _DEBUG
		else if (key == VK_OEM_PERIOD)
		{
			m_pauseTimer = !m_pauseTimer;
		}
		else if (key == VK_OEM_COMMA)
		{
			m_debugMenu->SetVisible(!m_debugMenu->IsVisible());
		}
#endif
		else if (key == 0x4C && m_toggleModShortcutEnabled) // L
		{
			m_disableMod = !m_disableMod;
		}
	}

#ifdef _DEBUG
	if (m_debugMenu->IsVisible())
	{
		m_debugMenu->HandleInput(key, wasDownBefore);
	}
#endif
}