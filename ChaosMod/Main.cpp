#include "stdafx.h"
#include "Main.h"
#include "EffectDispatcher.h"
#include "Memory.h"

Main* m_main = nullptr;
EffectDispatcher* m_effectDispatcher = nullptr;

bool m_forceDispatchLastEffectNextFrame;
bool m_pauseTimer;

int ParseConfigFile(int& effectSpawnTime, int& effectTimedDur, int& seed, int& effectTimedShortDur)
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

		int value = std::stoi(line.substr(line.find("=") + 1));

		if (key == "NewEffectSpawnTime")
		{
			effectSpawnTime = value > 0 ? value : 1;
		}
		else if (key == "EffectTimedDur")
		{
			effectTimedDur = value > 0 ? value : 1;
		}
		else if (key == "Seed")
		{
			seed = value;
		}
		else if (key == "EffectTimedShortDur")
		{
			effectTimedShortDur = value > 0 ? value : 1;
		}
	}

	return 0;
}

int ParseEffectsFile(std::vector<EffectType>& enabledEffects)
{
	static constexpr const char* FILE_PATH = "chaosmod/effects.ini";
	std::vector<int> goneThroughIds;

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
		enabledEffects.push_back((EffectType)i);
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

		int keyValue = std::stoi(key);
		int value = std::stoi(line.substr(line.find("=") + 1));

		if (std::find(goneThroughIds.begin(), goneThroughIds.end(), keyValue) != goneThroughIds.end())
		{
			std::ostringstream oss;
			oss << "Multiple entries for id " << keyValue << " in " << FILE_PATH << " found! Try regenerating the file maybe?";

			MessageBox(NULL, oss.str().c_str(), "ChaosModV Error", MB_OK | MB_ICONERROR);

			return -3;
		}

		goneThroughIds.push_back(keyValue);

		if (value)
		{
			continue;
		}

		// Map id to EffectType
		for (const auto pair : EffectsMap)
		{
			if (pair.second.Id == keyValue)
			{
				auto result = std::find(enabledEffects.begin(), enabledEffects.end(), pair.first);
				if (result != enabledEffects.end())
				{
					enabledEffects.erase(result);
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

	int effectSpawnTime = 180;
	int effectTimedDur = 60;
	int seed = -1;
	int effectTimedShortDur = 15;
	std::vector<EffectType> enabledEffects;

	int result;
	if ((result = ParseConfigFile(effectSpawnTime, effectTimedDur, seed, effectTimedShortDur))
		|| (result = ParseEffectsFile(enabledEffects)))
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
		m_effectDispatcher = new EffectDispatcher(effectSpawnTime, effectTimedDur, enabledEffects, effectTimedShortDur);
	}

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}

void Main::Loop()
{
	m_effectDispatcher->Reset();

	m_forceDispatchLastEffectNextFrame = false;
	m_pauseTimer = false;

	while (true)
	{
		scriptWait(0);

		if (IS_SCREEN_FADED_OUT())
		{
			continue;
		}

		if (m_forceDispatchLastEffectNextFrame)
		{
			m_forceDispatchLastEffectNextFrame = false;

			m_effectDispatcher->ClearEffects();
			m_effectDispatcher->DispatchEffect((EffectType)((int)_EFFECT_ENUM_MAX - 1));
		}

		if (!m_pauseTimer)
		{
			m_effectDispatcher->UpdateTimer();
		}

		m_effectDispatcher->UpdateEffects();
		m_effectDispatcher->Draw();
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
		delete m_effectDispatcher;
		m_effectDispatcher = nullptr;
	}

	MH_DisableHook(MH_ALL_HOOKS);
}

void Main::OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
#ifdef _DEBUG
	static bool CTRLpressed = false;
	if (key == VK_CONTROL)
	{
		CTRLpressed = !isUpNow;
	}
	else if (CTRLpressed && !wasDownBefore)
	{
		if (key == VK_OEM_MINUS)
		{
			m_forceDispatchLastEffectNextFrame = true;
		}
		else if (key == VK_OEM_PERIOD)
		{
			m_pauseTimer = !m_pauseTimer;
		}
	}
#endif
}