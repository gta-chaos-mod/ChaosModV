#include "stdafx.h"
#include "Main.h"
#include "EffectDispatcher.h"
#include "Memory.h"

Main* m_main = nullptr;
EffectDispatcher* m_effectDispatcher = nullptr;

bool m_forceDispatchEffectNextFrame = false;

int ParseConfigFile(int* effectSpawnTime, int* effectTimedDur, bool* spawnTimedEffects)
{
	constexpr const char* filePath = "chaosmod/config.ini";

	struct stat temp;
	if (stat(filePath, &temp) == -1)
	{
		return -1;
	}

	std::ifstream config(filePath);

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
			*effectSpawnTime = value;
		}
		else if (key == "EffectTimedDur")
		{
			*effectTimedDur = value;
		}
		else if (key == "SpawnTimedEffects")
		{
			*spawnTimedEffects = value;
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

	int effectSpawnTime;
	int effectTimedDur;
	bool spawnTimedEffects;

	int result;
	if ((result = ParseConfigFile(&effectSpawnTime, &effectTimedDur, &spawnTimedEffects)))
	{
		switch (result)
		{
		case -1:
			MessageBox(NULL,
				"Config File was not found or is invalid. Please make sure you've run the config tool at least once, otherwise try regenerating it.",
				"ChaosModV Error", MB_OK | MB_ICONERROR);
			break;
		case -2:
			MessageBox(NULL,
				"Config File couldn't be opened for reading. Make sure there's no process running with a handle on chaosmod/config.ini",
				"ChaosModV Error", MB_OK | MB_ICONERROR);
			break;
		}
		return false;
	}

	if (!m_effectDispatcher)
	{
		m_effectDispatcher = new EffectDispatcher(effectSpawnTime, effectTimedDur, spawnTimedEffects);
	}

	MH_EnableHook(MH_ALL_HOOKS);

	return true;
}

void Main::Loop()
{
	m_effectDispatcher->Reset();

	while (true)
	{
		scriptWait(0);

		if (IS_SCREEN_FADED_OUT())
		{
			continue;
		}

		if (m_forceDispatchEffectNextFrame)
		{
			m_forceDispatchEffectNextFrame = false;

			m_effectDispatcher->ClearEffects();
			m_effectDispatcher->DispatchEffect((EffectType)((int)_EFFECT_ENUM_MAX - 1));
		}

		m_effectDispatcher->UpdateTimer();
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
	else if (key == VK_OEM_MINUS && CTRLpressed && !wasDownBefore)
	{
		m_forceDispatchEffectNextFrame = true;
	}
#endif
}