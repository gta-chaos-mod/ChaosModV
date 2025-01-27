#pragma once

#include "Components/Component.h"

class EffectDispatchTimer : public Component
{
	bool m_EnableTimer              = true;
	bool m_PauseTimer               = false;
	bool m_DispatchEffectsOnTimer   = true;
	bool m_DrawTimerBar             = true;
	std::uint16_t m_EffectSpawnTime = 0;
	std::array<std::uint8_t, 3> m_TimerColor;
	float m_TimerPercentage     = 0.f;
	float m_FakeTimerPercentage = 0.f;
	std::uint64_t m_Timer       = 0;

	struct DistanceChaosState
	{
		bool DeadFlag                          = true;
		bool EnableDistanceBasedEffectDispatch = false;
		float DistanceToActivateEffect         = 500.f;
		enum class TravelledDistanceType
		{
			Distance,
			Displacement
		} DistanceType        = TravelledDistanceType::Distance;
		Vector3 SavedPosition = { 0.f, 0.f, 0.f };
	} m_DistanceChaosState;

  public:
	EffectDispatchTimer(const std::array<BYTE, 3> &timerColor);

  private:
	void UpdateTimer(int deltaTime);
	void UpdateTravelledDistance();

  public:
	bool IsTimerEnabled() const;
	void SetTimerEnabled(bool state);

	int GetDefaultEffectSpawnTime() const;
	int GetDefaultEffectSpawnDistance() const;

	void ResetSavedPosition();

	std::uint64_t GetTimer() const;
	void ResetTimer();

	int GetRemainingTimerTime() const;
	bool ShouldDispatchEffectNow() const;

	void SetShouldDispatchEffects(bool state);

	void SetFakeTimerPercentage(float percentage);
	void ResetFakeTimerPercentage();

	bool IsTimerPaused() const;
	void SetTimerPaused(bool pause);

	bool IsUsingDistanceBasedDispatch() const;

	virtual void OnRun() override;
};