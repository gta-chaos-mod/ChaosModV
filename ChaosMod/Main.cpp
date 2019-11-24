#include "stdafx.h"
#include "Main.h"
#include "EffectDispatcher.h"
#include "Memory.h"

Main* m_main = nullptr;
EffectDispatcher* m_effectDispatcher = nullptr;

std::unique_ptr<DebugMenu> m_debugMenu;
bool m_clearAllEffects = false;
bool m_pauseTimer = false;
bool m_clearEffectsShortcutEnabled = false;
int m_clearEffectsTextTime = 0;

int ParseConfigFile(int& effectSpawnTime, int& effectTimedDur, int& seed, int& effectTimedShortDur, bool& enableClearEffectsShortcut,
	bool& disableEffectsTwiceInRow, std::array<int, 3>& timerColor, std::array<int, 3>& textColor, std::array<int, 3>& effectTimerColor)
{
	static constexpr const char* FILE_PATH = "chaosmod/config.ini";

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
		try
		{
			int _value = std::stoi(value);

			if (key == "NewEffectSpawnTime")
			{
				effectSpawnTime = _value > 0 ? _value : 1;
			}
			else if (key == "EffectTimedDur")
			{
				effectTimedDur = _value > 0 ? _value : 1;
			}
			else if (key == "Seed")
			{
				seed = _value;
			}
			else if (key == "EffectTimedShortDur")
			{
				effectTimedShortDur = _value > 0 ? _value : 1;
			}
			else if (key == "EnableClearEffectsShortcut")
			{
				enableClearEffectsShortcut = _value;
			}
			else if (key == "DisableEffectTwiceInRow")
			{
				disableEffectsTwiceInRow = _value;
			}
		}
		catch (std::invalid_argument)
		{
			// For now it's probably a hex color
			// Also the WPF color picker stores colors in argb instead of rgba apparently, lol
			std::array<int, 3> colors;

			int j = 0;
			for (int i = 3; i < 9; i += 2)
			{
				colors[j++] = std::strtol(value.substr(i, 2).c_str(), nullptr, 16);
			}

			if (key == "EffectTimerColor")
			{
				timerColor = colors;
			}
			else if (key == "EffectTextColor")
			{
				textColor = colors;
			}
			else if (key == "EffectTimedTimerColor")
			{
				effectTimerColor = colors;
			}
		}
	}

	return 0;
}

int ParseEffectsFile(std::map<EffectType, std::array<int, 3>>& enabledEffects)
{
	static constexpr const char* FILE_PATH = "chaosmod/effects.ini";
	std::vector<std::string> goneThroughIds;

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
		enabledEffects.emplace(std::make_pair<EffectType, std::array<int, 3>>((EffectType)i, {}));
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
		std::array<int, 4> values {1, -1, -1, 5 };

		int splitIndex = value.find(",");
		if (splitIndex == value.npos)
		{
			values[0] = std::stoi(value);
		}
		else
		{
			for (int i = 0; i < 4; i++)
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

		if (std::find(goneThroughIds.begin(), goneThroughIds.end(), key) != goneThroughIds.end())
		{
			std::ostringstream oss;
			oss << "Multiple entries for id " << key << " in " << FILE_PATH << " found! Try regenerating the file maybe?";

			MessageBox(NULL, oss.str().c_str(), "ChaosModV Error", MB_OK | MB_ICONERROR);

			return -3;
		}

		goneThroughIds.push_back(key);

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

					static std::ofstream log("chaosmod/enabledeffectslog.txt");
					log << effectInfo.Name << " " << result->second[0] << " " << result->second[1] << " " << result->second[2] << std::endl;
				}
				break;
			}
		}
	}

	return 0;
}

bool Main::Init()
{
	if (m_main && m_effectDispatcher)
	{
		return false;
	}

	if (!m_main)
	{
		m_main = new Main();
	}

	int effectSpawnTime = 30;
	int effectTimedDur = 90;
	int seed = -1;
	int effectTimedShortDur = 30;
	bool disableEffectsTwiceInRow = false;
	std::array<int, 3> timerColor = { 40, 40, 255 };
	std::array<int, 3> textColor = { 255, 255, 255 };
	std::array<int, 3> effectTimerColor = { 180, 180, 180 };
	std::map<EffectType, std::array<int, 3>> enabledEffects;

	int result;
	if ((result = ParseConfigFile(effectSpawnTime, effectTimedDur, seed, effectTimedShortDur, m_clearEffectsShortcutEnabled, disableEffectsTwiceInRow,
		timerColor, textColor, effectTimerColor)) || (result = ParseEffectsFile(enabledEffects)))
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

	Random::SetSeed(seed);

	if (!m_effectDispatcher)
	{
		m_effectDispatcher = new EffectDispatcher(effectSpawnTime, effectTimedDur, enabledEffects, effectTimedShortDur, disableEffectsTwiceInRow,
			timerColor, textColor, effectTimerColor);
	}

#ifdef _DEBUG
	std::vector<EffectType> enabledEffectTypes;
	for (auto pair : enabledEffects)
	{
		enabledEffectTypes.push_back(pair.first);
	}

	m_debugMenu = std::make_unique<DebugMenu>(enabledEffectTypes, m_effectDispatcher);
#endif

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}

void Main::Loop()
{
	m_effectDispatcher->Reset();

	while (true)
	{
		WAIT(0);

		if (IS_SCREEN_FADED_OUT())
		{
			continue;
		}

		if (m_clearAllEffects)
		{
			m_clearAllEffects = false;

			m_effectDispatcher->Reset();
		}

		if (m_clearEffectsTextTime > 0)
		{
			BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Effects Cleared!");
			SET_TEXT_SCALE(.8f, .8f);
			SET_TEXT_COLOUR(255, 100, 100, 255);
			SET_TEXT_CENTRE(true);
			END_TEXT_COMMAND_DISPLAY_TEXT(.86f, .86f, 0);

			static DWORD64 lastTick = GetTickCount64();
			DWORD64 curTick = GetTickCount64();
			
			m_clearEffectsTextTime -= curTick - lastTick;

			lastTick = curTick;
		}

		if (!m_pauseTimer)
		{
			m_effectDispatcher->UpdateTimer();
		}

		m_effectDispatcher->UpdateEffects();
		m_effectDispatcher->Draw();

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

void Main::Stop()
{
	if (!m_main && !m_effectDispatcher)
	{
		return;
	}

	if (m_main)
	{
		delete m_main;
		m_main = nullptr;
	}

	if (m_effectDispatcher)
	{
		delete m_main;
		m_effectDispatcher = nullptr;
	}

	MH_DisableHook(MH_ALL_HOOKS);
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
	}

#ifdef _DEBUG
	if (m_debugMenu->IsVisible())
	{
		m_debugMenu->HandleInput(key, wasDownBefore);
	}
#endif
}