#pragma once

#include <list>

#include "Component.h"
#include "Util/OptionsFile.h"
#include "Util/Text.h"

#define SPLASH_TEXT_DUR_SECS 10

class CrossingChallenge : public Component
{
  private:
	struct WeaponInfo
	{
		Hash hash;
		int ammo;
	};

	OptionsFile m_ConfigFile { "chaosmod/configs/crossing.ini", { "chaosmod/crossing.ini" } };

	bool m_Enabled              = false;

	int m_HelpMessageTick       = -1;

	bool m_CaptureStartFlag     = false;
	bool m_CaptureEndFlag       = false;

	int m_StartedState          = 0;

	bool m_WaitingConfirm       = false;

	bool m_StartEnabled         = false;
	Vector3 m_StartLocation     = Vector3();
	Hash m_StartVehicleHash     = 0;
	float m_StartHeading        = 0.f;
	float m_StartCameraHeading  = 0.f;
	Hash m_StartWeatherType1    = 0;
	Hash m_StartWeatherType2    = 0;
	float m_StartWeatherPercent = 0.f;
	int m_ClockHours            = 0;
	int m_ClockMinutes          = 0;
	int m_ClockSeconds          = 0;
	std::vector<WeaponInfo> m_StartWeapons;

	bool m_EndEnabled              = false;
	Vector3 m_EndLocation          = Vector3();
	float m_EndRadius              = 0.f;

	int m_ButtonsScaleformHandle   = 0;
	bool m_ButtonsScaleformLoading = false;

	int m_PassedScaleformHandle    = 0;
	bool m_PassedScaleformLoading  = false;
	int m_PassedScaleformTick      = 0;
	bool m_PassedTransitionStarted = false;

	Blip m_StartBlip               = 0;
	Blip m_EndBlip                 = 0;

	DWORD m_TickCount              = 0;
	DWORD m_LastTick               = 0;
	int m_EffectsCount             = 0;
	bool m_TimerStarted            = 0;

	void ShowHint(const std::string &text);

	void SetStartParams();
	void ControlRespawn();

	int m_PassedState = 0;

	bool CheckEndReached() const;
	void ShowPassedScaleform();
	void ControlPassed();

	void SaveConfig();

	void CaptureStart();
	void CaptureEnd();
	void IncreaseEndRadius();
	void DecreaseEndRadius();
	bool IsEndValid() const;
	void ShowEndCylinder() const;
	void ShowHelpButtons();
	void ShowBlips();

	void ShowProgress() const;

  public:
	CrossingChallenge();

	virtual void OnRun() override;
	virtual void OnModPauseCleanup() override;
	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed) override;
	inline void IncrementEffects()
	{
		m_EffectsCount++;
	}

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};