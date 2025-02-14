#include <stdafx.h>

#include "Components/CrossingChallenge.h"
#include "Components/EffectDispatchTimer.h"
#include "Main.h"
#include "Memory/WeaponPool.h"
#include "Util/OptionsManager.h"
#include "Util/ScriptText.h"

void CrossingChallenge::SetStartParams()
{
	Ped player = PLAYER_PED_ID();

	if (m_StartVehicleHash)
	{
		LoadModel(m_StartVehicleHash);
		Vehicle vehicle = CREATE_VEHICLE(m_StartVehicleHash, m_StartLocation.x, m_StartLocation.y, m_StartLocation.z,
		                                 m_StartHeading, true, false, false);
		SET_PED_INTO_VEHICLE(player, vehicle, -1);
	}

	SET_ENTITY_HEADING(player, m_StartHeading);
	SET_GAMEPLAY_CAM_RELATIVE_HEADING(m_StartCameraHeading);
	_SET_WEATHER_TYPE_TRANSITION(m_StartWeatherType1, m_StartWeatherType2, m_StartWeatherPercent);
	SET_CLOCK_TIME(m_ClockHours, m_ClockMinutes, m_ClockSeconds);
	REMOVE_ALL_PED_WEAPONS(player, false);
	for (const auto &weapon : m_StartWeapons)
		GIVE_WEAPON_TO_PED(player, weapon.hash, weapon.ammo, false, false);

	CLEAR_PLAYER_WANTED_LEVEL(PLAYER_ID());
}

void CrossingChallenge::ControlRespawn()
{
	Ped player = PLAYER_PED_ID();
	IGNORE_NEXT_RESTART(true);
	PAUSE_DEATH_ARREST_RESTART(true);
	if (IS_ENTITY_DEAD(player, false))
	{
		Vehicle vehicle = 0;

		if (IS_PED_IN_ANY_VEHICLE(player, false))
		{
			SET_PED_TO_RAGDOLL(player, 2000, 2000, 0, true, true, false);
			vehicle = GET_VEHICLE_PED_IS_IN(player, false);
		}

		while (!IS_SCREEN_FADED_OUT())
		{
			// block main process
			if (ComponentExists<EffectDispatcher>())
				GetComponent<EffectDispatcher>()->DrawEffectTexts();
			if (ComponentExists<EffectDispatchTimer>())
				GetComponent<EffectDispatchTimer>()->SetTimerPaused(true);
			ShowProgress();
			WAIT(0);
		}
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("respawn_controller");
		SET_TIME_SCALE(1.f);
		ANIMPOSTFX_STOP_ALL();
		NETWORK_REQUEST_CONTROL_OF_ENTITY(player);
		NETWORK_RESURRECT_LOCAL_PLAYER(m_StartLocation.x, m_StartLocation.y, m_StartLocation.z, m_StartHeading, false,
		                               false, false);
		WAIT(2000);
		if (vehicle)
		{
			SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);
			DELETE_ENTITY(&vehicle);
		}

		if (ComponentExists<EffectDispatcher>())
			GetComponent<EffectDispatcher>()->Reset();

		if (ComponentExists<EffectDispatchTimer>())
		{
			GetComponent<EffectDispatchTimer>()->ResetTimer();
			GetComponent<EffectDispatchTimer>()->SetTimerPaused(false);
		}
		ClearEntityPool();

		SetStartParams();

		DO_SCREEN_FADE_IN(2500);
		FORCE_GAME_STATE_PLAYING();
		RESET_PLAYER_ARREST_STATE(player);
		DISPLAY_HUD(true);
		FREEZE_ENTITY_POSITION(player, false);

		m_TickCount    = 0;
		m_EffectsCount = 0;
		m_TimerStarted = false;
	}
}

static bool LoadScaleform(int &handle, bool &loading, const std::string &name)
{
	if (!handle || (!loading && !HAS_SCALEFORM_MOVIE_LOADED(handle)))
	{
		handle  = REQUEST_SCALEFORM_MOVIE(name.c_str());
		loading = true;
	}

	return HAS_SCALEFORM_MOVIE_LOADED(handle);
}

bool CrossingChallenge::CheckEndReached() const
{
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	return GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, 0, m_EndLocation.x, m_EndLocation.y, 0, false)
	     < m_EndRadius;
}

void CrossingChallenge::ShowPassedScaleform()
{
	if (!LoadScaleform(m_PassedScaleformHandle, m_PassedScaleformLoading, "MP_BIG_MESSAGE_FREEMODE")
	    || !IS_MISSION_COMPLETE_READY_FOR_UI())
	{
		return;
	}
	else if (m_PassedScaleformLoading)
	{
		BEGIN_SCALEFORM_MOVIE_METHOD(m_PassedScaleformHandle, "SHOW_SHARD_RANKUP_MP_MESSAGE");
		BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("YES!!!");
		END_TEXT_COMMAND_SCALEFORM_STRING();
		BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");

		int time = m_TickCount / 1000;
		std::ostringstream oss;
		oss << "You win! Congratulations!!!!\nEffect count: " << m_EffectsCount << " Time: " << time / 60 << ":"
		    << std::setw(2) << std::setfill('0') << time % 60;

		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(oss.str().c_str());
		END_TEXT_COMMAND_SCALEFORM_STRING();
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(14); // HUD_COLOUR_YELLOWDARK
		END_SCALEFORM_MOVIE_METHOD();

		m_PassedScaleformLoading = false;
	}

	if (!m_PassedTransitionStarted && GET_GAME_TIMER() - m_PassedScaleformTick > 5000)
	{
		CALL_SCALEFORM_MOVIE_METHOD(m_PassedScaleformHandle, "TRANSITION_OUT");
		m_PassedTransitionStarted = true;
	}

	DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_PassedScaleformHandle, 255, 255, 255, 255, 0);

	if (GET_GAME_TIMER() - m_PassedScaleformTick > 6000)
		m_PassedState = 2;
}

void CrossingChallenge::ControlPassed()
{
	switch (m_PassedState)
	{
	case 0:
		if (CheckEndReached())
		{
			m_PassedState         = 1;
			m_PassedScaleformTick = GET_GAME_TIMER();
			PLAY_MISSION_COMPLETE_AUDIO("MICHAEL_SMALL_01");
			SET_ENTITY_INVINCIBLE(PLAYER_PED_ID(), true);
			if (ComponentExists<EffectDispatcher>())
				GetComponent<EffectDispatcher>()->ClearEffects();
			if (ComponentExists<EffectDispatchTimer>())
				GetComponent<EffectDispatchTimer>()->SetTimerEnabled(false);
		}
		else
			break;
		[[fallthrough]];
	case 1:
		while (m_PassedState == 1)
		{
			ShowPassedScaleform();
			ShowProgress();
			// block main process
			WAIT(0);
		}
		[[fallthrough]];
	case 2:
		SET_ENTITY_INVINCIBLE(PLAYER_PED_ID(), false);
		Main::Stop();
		break;
	}
}

static std::string GetWeaponOptionName(Hash weapon)
{
	std::ostringstream oss;

	oss << "StartWeapon_" << weapon;

	return oss.str();
}

void CrossingChallenge::SaveConfig()
{
	m_ConfigFile.SetValue("StartEnabled", m_StartEnabled);
	m_ConfigFile.SetValue("StartLocationX", m_StartLocation.x);
	m_ConfigFile.SetValue("StartLocationY", m_StartLocation.y);
	m_ConfigFile.SetValue("StartLocationZ", m_StartLocation.z);
	m_ConfigFile.SetValue("StartVehicle", m_StartVehicleHash);
	m_ConfigFile.SetValue("StartHeading", m_StartHeading);
	m_ConfigFile.SetValue("StartCameraHeading", m_StartCameraHeading);
	m_ConfigFile.SetValue("StartWeather1", m_StartWeatherType1);
	m_ConfigFile.SetValue("StartWeather2", m_StartWeatherType2);
	m_ConfigFile.SetValue("StartWeatherPercent", m_StartWeatherPercent);
	m_ConfigFile.SetValue("StartHours", m_ClockHours);
	m_ConfigFile.SetValue("StartMinutes", m_ClockMinutes);
	m_ConfigFile.SetValue("StartSeconds", m_ClockSeconds);

	for (const auto &weapon : m_StartWeapons)
		m_ConfigFile.SetValue(GetWeaponOptionName(weapon.hash), weapon.ammo);

	m_ConfigFile.SetValue("EndEnabled", m_EndEnabled);
	m_ConfigFile.SetValue("EndLocationX", m_EndLocation.x);
	m_ConfigFile.SetValue("EndLocationY", m_EndLocation.y);
	m_ConfigFile.SetValue("EndLocationZ", m_EndLocation.z);
	m_ConfigFile.SetValue("EndRadius", m_EndRadius);

	m_ConfigFile.WriteFile();
}

void CrossingChallenge::CaptureStart()
{
	Ped player      = PLAYER_PED_ID();

	m_StartEnabled  = true;
	m_StartLocation = GET_ENTITY_COORDS(player, false);
	if (IS_PED_IN_ANY_VEHICLE(player, false))
		m_StartVehicleHash = GET_ENTITY_MODEL(GET_VEHICLE_PED_IS_IN(player, false));
	else
		m_StartVehicleHash = 0;
	m_StartHeading       = GET_ENTITY_HEADING(player);
	m_StartCameraHeading = GET_GAMEPLAY_CAM_RELATIVE_HEADING();
	_GET_WEATHER_TYPE_TRANSITION(&m_StartWeatherType1, &m_StartWeatherType2, &m_StartWeatherPercent);
	m_ClockHours   = GET_CLOCK_HOURS();
	m_ClockMinutes = GET_CLOCK_MINUTES();
	m_ClockSeconds = GET_CLOCK_SECONDS();

	m_StartWeapons.clear();

	for (Hash weapon : Memory::GetAllWeapons())
		if (HAS_PED_GOT_WEAPON(player, weapon, false))
			m_StartWeapons.emplace_back(WeaponInfo { weapon, GET_AMMO_IN_PED_WEAPON(player, weapon) });

	SaveConfig();
}

void CrossingChallenge::CaptureEnd()
{
	m_EndEnabled  = true;
	m_EndLocation = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	m_EndRadius   = 10.f;

	SaveConfig();
}

CrossingChallenge::CrossingChallenge()
{
	m_Enabled = g_OptionsManager.GetConfigValue({ "EnableCrossingChallenge" }, false);

	if (!m_Enabled)
		return;

	m_StartEnabled = m_ConfigFile.ReadValue({ "StartEnabled" }, false);
	if (m_StartEnabled)
	{
		m_StartLocation       = Vector3(m_ConfigFile.ReadValue({ "StartLocationX" }, 0.f),
		                                m_ConfigFile.ReadValue({ "StartLocationY" }, 0.f),
		                                m_ConfigFile.ReadValue({ "StartLocationZ" }, 0.f));
		m_StartVehicleHash    = m_ConfigFile.ReadValue({ "StartVehicle" }, 0);
		m_StartHeading        = m_ConfigFile.ReadValue({ "StartHeading" }, 0.f);
		m_StartCameraHeading  = m_ConfigFile.ReadValue({ "StartCameraHeading" }, 0.f);
		m_StartWeatherType1   = m_ConfigFile.ReadValue({ "StartWeather1" }, 0);
		m_StartWeatherType2   = m_ConfigFile.ReadValue({ "StartWeather2" }, 0);
		m_StartWeatherPercent = m_ConfigFile.ReadValue({ "StartWeatherPercent" }, 0.f);
		m_ClockHours          = m_ConfigFile.ReadValue({ "StartHours" }, 0);
		m_ClockMinutes        = m_ConfigFile.ReadValue({ "StartMinutes" }, 0);
		m_ClockSeconds        = m_ConfigFile.ReadValue({ "StartSeconds" }, 0);

		for (Hash hash : Memory::GetAllWeapons())
		{
			int ammo = m_ConfigFile.ReadValue({ GetWeaponOptionName(hash) }, -1);
			if (ammo >= 0)
				m_StartWeapons.emplace_back(WeaponInfo { hash, ammo });
		}
	}

	m_EndEnabled = m_ConfigFile.ReadValue({ "EndEnabled" }, false);
	if (m_EndEnabled)
	{
		m_EndLocation =
		    Vector3(m_ConfigFile.ReadValue({ "EndLocationX" }, 0.f), m_ConfigFile.ReadValue({ "EndLocationY" }, 0.f),
		            m_ConfigFile.ReadValue({ "EndLocationZ" }, 0.f));
		m_EndRadius = m_ConfigFile.ReadValue({ "EndRadius" }, 0.f);
	}

	if (ComponentExists<EffectDispatcher>())
		m_OnPreDispatchEffectListener.Register(GetComponent<EffectDispatcher>()->OnPreDispatchEffect,
		                                       [&](const EffectIdentifier &id)
		                                       {
			                                       IncrementEffects();
			                                       return true;
		                                       });
}

static void DrawTriangle(Vector3 A, Vector3 B, Vector3 C, int r, int g, int b, int a)
{
	DRAW_POLY(A.x, A.y, A.z, B.x, B.y, B.z, C.x, C.y, C.z, r, g, b, a);
	DRAW_POLY(B.x, B.y, B.z, A.x, A.y, A.z, C.x, C.y, C.z, r, g, b, a);
}

static void DrawCylinder(Vector3 center, float radius, float height, int r, int g, int b, int a, int polygons = 100)
{
	Vector3 uc = center + Vector3(0, 0, height / 2);
	Vector3 dc = center - Vector3(0, 0, height / 2);

	for (int i = 0; i < polygons; i++)
	{
		Vector3 delta1 = Vector3(SIN(360.f * i / polygons), COS(360.f * i / polygons), 0);
		Vector3 delta2 = Vector3(SIN(360.f * (i + 1) / polygons), COS(360.f * (i + 1) / polygons), 0);

		Vector3 u1 = uc + delta1 * radius, u2 = uc + delta2 * radius, d1 = dc + delta1 * radius,
		        d2 = dc + delta2 * radius;

		DrawTriangle(u1, u2, uc, r, g, b, a);
		DrawTriangle(d1, d2, dc, r, g, b, a);
		DrawTriangle(d2, d1, u1, r, g, b, a);
		DrawTriangle(d2, u2, u1, r, g, b, a);
	}
}

bool CrossingChallenge::IsEndValid() const
{
	return !m_EndEnabled || !m_StartEnabled
	    || GET_DISTANCE_BETWEEN_COORDS(m_StartLocation.x, m_StartLocation.y, 0, m_EndLocation.x, m_EndLocation.y, 0,
	                                   false)
	           > m_EndRadius + 5.f;
}

void CrossingChallenge::ShowEndCylinder() const
{
	if (IsEndValid())
		DrawCylinder(m_EndLocation, m_EndRadius, 50.f, 255, 255, 0, 120);
	else
		DrawCylinder(m_EndLocation, m_EndRadius, 50.f, 255, 0, 0, 120);
}

void CrossingChallenge::IncreaseEndRadius()
{
	m_EndRadius += 1.f;
	SaveConfig();
}

void CrossingChallenge::DecreaseEndRadius()
{
	if (m_EndRadius >= 2.f)
		m_EndRadius -= 1.f;
	SaveConfig();
}

void CrossingChallenge::ShowHint(const std::string &text)
{
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
	END_TEXT_COMMAND_DISPLAY_HELP(0, 1, 1, -1);
	m_HelpMessageTick = GET_GAME_TIMER();
}

static void AddButton(int scaleform, int i, const std::string &control, const std::string &text)
{
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_DATA_SLOT");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(i);
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(control.c_str());
	BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
	END_TEXT_COMMAND_SCALEFORM_STRING();
	END_SCALEFORM_MOVIE_METHOD();
}

void CrossingChallenge::ShowHelpButtons()
{
	if (!LoadScaleform(m_ButtonsScaleformHandle, m_ButtonsScaleformLoading, "INSTRUCTIONAL_BUTTONS"))
		return;
	else
		m_ButtonsScaleformLoading = false;

	CALL_SCALEFORM_MOVIE_METHOD(m_ButtonsScaleformHandle, "CLEAR_ALL");

	int i = 0;

	if (IsEndValid())
	{
		if (m_WaitingConfirm)
			AddButton(m_ButtonsScaleformHandle, i++, "b_1003", "Start?");
		else
			AddButton(m_ButtonsScaleformHandle, i++, "b_2000", "Confirm and start");
	}
	if (m_EndEnabled)
	{
		AddButton(m_ButtonsScaleformHandle, i++, "b_1009", "Increase finish radius");
		AddButton(m_ButtonsScaleformHandle, i++, "b_1010", "Decrease finish radius");
	}
	AddButton(m_ButtonsScaleformHandle, i++, "t_Q", "Capture finish point");
	AddButton(m_ButtonsScaleformHandle, i++, "t_E", "Capture starting point");

	CALL_SCALEFORM_MOVIE_METHOD_WITH_NUMBER(m_ButtonsScaleformHandle, "SET_BACKGROUND_COLOUR", 0, 0, 0, 80, -1.0);

	CALL_SCALEFORM_MOVIE_METHOD(m_ButtonsScaleformHandle, "DRAW_INSTRUCTIONAL_BUTTONS");

	DRAW_SCALEFORM_MOVIE_FULLSCREEN(m_ButtonsScaleformHandle, 255, 255, 255, 255, 0);
}

static void HandleBlip(Blip &blip, Vector3 coords, int color, const std::string &name)
{
	if (!blip || !DOES_BLIP_EXIST(blip))
	{
		blip = ADD_BLIP_FOR_COORD(coords.x, coords.y, coords.z);
		SET_BLIP_COLOUR(blip, color);
		SET_BLIP_DISPLAY(blip, 2);
		BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(name.c_str());
		END_TEXT_COMMAND_SET_BLIP_NAME(blip);
	}
	else
	{
		SET_BLIP_COORDS(blip, coords.x, coords.y, coords.z);
	}
}

void CrossingChallenge::ShowBlips()
{
	if (m_StartEnabled)
		HandleBlip(m_StartBlip, m_StartLocation, 2, "Start");
	if (m_EndEnabled)
		HandleBlip(m_EndBlip, m_EndLocation, 5, "Finish");
}

void CrossingChallenge::ShowProgress() const
{
	std::ostringstream oss;

	int time = m_TickCount / 1000;

	DRAW_RECT(.105f, .145f, .19f, .05f, 0, 0, 0, 80, false);

	oss << time / 60 << ":" << std::setw(2) << std::setfill('0') << time % 60;

	DrawScreenText(oss.str(), { .15f, .12f }, .7f, { 50, 198, 90 }, true, ScreenTextAdjust::Left);

	oss.str("");

	oss << "Effects: " << m_EffectsCount;

	DrawScreenText(oss.str(), { .015f, .12f }, .7f, { 255, 255, 255 }, true, ScreenTextAdjust::Left);
}

void CrossingChallenge::OnRun()
{
	if (!m_Enabled)
		return;

	if (m_HelpMessageTick != -1 && GET_GAME_TIMER() - m_HelpMessageTick > 5000)
	{
		CLEAR_ALL_HELP_MESSAGES();
		m_HelpMessageTick = -1;
	}

	if (m_CaptureStartFlag)
	{
		CaptureStart();
		ShowHint("Starting point captured");
		m_CaptureStartFlag = false;
	}
	if (m_CaptureEndFlag)
	{
		CaptureEnd();
		ShowHint("Finish point captured");
		m_CaptureEndFlag = false;
	}

	if (m_StartedState == 0)
	{
		SET_ENTITY_INVINCIBLE(PLAYER_PED_ID(), true);
		ShowHelpButtons();
		ShowBlips();

		if (m_EndEnabled)
			ShowEndCylinder();

		if (ComponentExists<EffectDispatchTimer>())
			GetComponent<EffectDispatchTimer>()->SetTimerEnabled(false);
	}
	else if (m_StartedState == 1)
	{
		if (m_StartEnabled)
		{
			DO_SCREEN_FADE_OUT(1000);
			NEW_LOAD_SCENE_START_SPHERE(m_StartLocation.x, m_StartLocation.y, m_StartLocation.z, 200.f, 0);
			WAIT(1000);
			SET_ENTITY_COORDS(PLAYER_PED_ID(), m_StartLocation.x, m_StartLocation.y, m_StartLocation.z, 1, 0, 0, 1);
			int startTick = GET_GAME_TIMER();
			while (!IS_NEW_LOAD_SCENE_LOADED() && GET_GAME_TIMER() - startTick < 10000)
			{
				SET_TIME_SCALE(1.f);
				WAIT(0);
			}

			SetStartParams();
			REMOVE_BLIP(&m_StartBlip);

			DO_SCREEN_FADE_IN(1000);
		}

		if (m_EndEnabled)
			SET_BLIP_ROUTE(m_EndBlip, true);

		SET_ENTITY_INVINCIBLE(PLAYER_PED_ID(), false);
		if (ComponentExists<EffectDispatchTimer>())
		{
			GetComponent<EffectDispatchTimer>()->ResetTimer();
			GetComponent<EffectDispatchTimer>()->SetTimerEnabled(true);
		}

		m_TickCount    = 0;
		m_EffectsCount = 0;
		m_TimerStarted = false;

		m_StartedState = 2;
	}
	else
	{
		if (!m_TimerStarted
		    && (IS_CONTROL_JUST_PRESSED(0, 30) || IS_CONTROL_JUST_PRESSED(0, 31) || IS_CONTROL_JUST_PRESSED(0, 32)
		        || IS_CONTROL_JUST_PRESSED(0, 33) || IS_CONTROL_JUST_PRESSED(0, 34) || IS_CONTROL_JUST_PRESSED(0, 35)))
		{
			m_TimerStarted = true;
		}

		if (m_StartEnabled)
		{
			ControlRespawn();
		}
		else
		{
			IGNORE_NEXT_RESTART(false);
			PAUSE_DEATH_ARREST_RESTART(false);
		}

		if (m_EndEnabled)
			ControlPassed();

		int deltaTicks = GetTickCount64() - m_LastTick;
		m_LastTick     = GetTickCount64();
		if (m_TimerStarted && deltaTicks < 1000)
			m_TickCount += deltaTicks;

		ShowProgress();
	}
}

void CrossingChallenge::OnModPauseCleanup()
{
	if (m_StartedState != 2)
		SET_ENTITY_INVINCIBLE(PLAYER_PED_ID(), false);

	m_StartedState            = 0;
	m_ButtonsScaleformHandle  = 0;
	m_ButtonsScaleformLoading = false;
	if (DOES_BLIP_EXIST(m_StartBlip))
		REMOVE_BLIP(&m_StartBlip);
	if (DOES_BLIP_EXIST(m_EndBlip))
		REMOVE_BLIP(&m_EndBlip);

	if (m_HelpMessageTick != -1)
	{
		CLEAR_ALL_HELP_MESSAGES();
		m_HelpMessageTick = -1;
	}
}

void CrossingChallenge::OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
                                   bool isAltPressed)
{
	if (m_StartedState == 0)
	{
		if (!repeated)
		{
			if (key == 0x45) // E
			{
				m_CaptureStartFlag = true;
			}
			else if (key == 0x51) // Q
			{
				m_CaptureEndFlag = true;
			}
			else if (key == 0x20) // Space
			{
				m_WaitingConfirm = true;
			}
			else if (key == 0x21) // PgUp
			{
				IncreaseEndRadius();
			}
			else if (key == 0x22) // PgDn
			{
				DecreaseEndRadius();
			}
			else if (key == 0x0D && m_WaitingConfirm) // Enter
			{
				m_WaitingConfirm = false;
				m_StartedState   = 1;
			}
		}

		if (key != 0x20) // Space
			m_WaitingConfirm = false;
	}
}