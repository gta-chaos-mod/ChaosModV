#include <stdafx.h>

#include "Components/EffectDispatchTimer.h"
#include "Components/EffectDispatcher.h"
#include "Components/MetaModifiers.h"

#include "Util/OptionsManager.h"

EffectDispatchTimer::EffectDispatchTimer(const std::array<BYTE, 3> &timerColor) : Component()
{
	m_TimerColor      = timerColor;

	m_DrawTimerBar    = !g_OptionsManager.GetConfigValue({ "DisableTimerBarDraw" }, OPTION_DEFAULT_NO_EFFECT_BAR);
	m_EffectSpawnTime = g_OptionsManager.GetConfigValue({ "NewEffectSpawnTime" }, OPTION_DEFAULT_EFFECT_SPAWN_TIME);

	m_DistanceChaosState.EnableDistanceBasedEffectDispatch = g_OptionsManager.GetConfigValue(
	    { "EnableDistanceBasedEffectDispatch" }, OPTION_DEFAULT_DISTANCE_BASED_DISPATCH_ENABLED);
	m_DistanceChaosState.DistanceToActivateEffect =
	    g_OptionsManager.GetConfigValue<float>({ "DistanceToActivateEffect" }, OPTION_DEFAULT_EFFECT_SPAWN_DISTANCE);
	m_DistanceChaosState.DistanceType = static_cast<DistanceChaosState::TravelledDistanceType>(
	    g_OptionsManager.GetConfigValue({ "DistanceType" }, OPTION_DEFAULT_DISTANCE_TYPE));
}

void EffectDispatchTimer::UpdateTimer(int deltaTime)
{
	if (!m_EnableTimer || (ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->DisableChaos))
		return;

	int effectSpawnTime = ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->TimeToDispatchEffect > 0
	                        ? GetComponent<MetaModifiers>()->TimeToDispatchEffect
	                        : m_EffectSpawnTime;

	m_TimerPercentage += deltaTime
	                   * (!ComponentExists<MetaModifiers>() ? 1.f : GetComponent<MetaModifiers>()->TimerSpeedModifier)
	                   / effectSpawnTime / 1000.f;

	if (m_TimerPercentage >= 1.f && m_DispatchEffectsOnTimer && ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->DispatchRandomEffect();

		if (ComponentExists<MetaModifiers>())
			for (std::uint8_t i = 0; i < GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch; i++)
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();

		m_TimerPercentage = 0.f;
	}
}

void EffectDispatchTimer::UpdateTravelledDistance()
{
	auto player   = PLAYER_PED_ID();
	auto position = GET_ENTITY_COORDS(player, false);

	if (!m_EnableTimer || (ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->DisableChaos))
	{
		m_DistanceChaosState.SavedPosition = position;
		return;
	}

	if (IS_ENTITY_DEAD(player, false))
	{
		m_DistanceChaosState.DeadFlag = true;
		return;
	}

	if (m_DistanceChaosState.DeadFlag)
	{
		m_DistanceChaosState.DeadFlag      = false;
		m_DistanceChaosState.SavedPosition = GET_ENTITY_COORDS(player, false);
		return;
	}

	int effectSpawnDistance =
	    ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->DistanceToDispatchEffect > 0
	        ? GetComponent<MetaModifiers>()->DistanceToDispatchEffect
	        : m_DistanceChaosState.DistanceToActivateEffect;

	auto distance =
	    GET_DISTANCE_BETWEEN_COORDS(position.x, position.y, position.z, m_DistanceChaosState.SavedPosition.x,
	                                m_DistanceChaosState.SavedPosition.y, m_DistanceChaosState.SavedPosition.z, true);

	if (m_DistanceChaosState.DistanceType == DistanceChaosState::TravelledDistanceType::Displacement)
	{
		m_TimerPercentage =
		    (distance * (ComponentExists<MetaModifiers>() ? GetComponent<MetaModifiers>()->TimerSpeedModifier : 1.f))
		    / effectSpawnDistance;

		if (m_TimerPercentage >= 1.f)
		{
			if (m_DispatchEffectsOnTimer && ComponentExists<EffectDispatcher>())
			{
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();

				if (ComponentExists<MetaModifiers>())
					for (std::uint8_t i = 0; i < GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch; i++)
						GetComponent<EffectDispatcher>()->DispatchRandomEffect();
			}

			m_DistanceChaosState.SavedPosition = position;
		}
	}
	else if (m_DistanceChaosState.DistanceType == DistanceChaosState::TravelledDistanceType::Distance)
	{
		m_DistanceChaosState.SavedPosition = position;
		m_TimerPercentage +=
		    (distance * (ComponentExists<MetaModifiers>() ? GetComponent<MetaModifiers>()->TimerSpeedModifier : 1.f))
		    / effectSpawnDistance;

		if (m_TimerPercentage >= 1.f && m_DispatchEffectsOnTimer && ComponentExists<EffectDispatcher>())
		{
			GetComponent<EffectDispatcher>()->DispatchRandomEffect();

			if (ComponentExists<MetaModifiers>())
				for (std::uint8_t i = 0; i < GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch; i++)
					GetComponent<EffectDispatcher>()->DispatchRandomEffect();

			m_TimerPercentage = 0;
		}
	}
}

bool EffectDispatchTimer::IsTimerEnabled() const
{
	return m_EnableTimer;
}

void EffectDispatchTimer::SetTimerEnabled(bool state)
{
	m_EnableTimer = state;
}

int EffectDispatchTimer::GetDefaultEffectSpawnTime() const
{
	return m_EffectSpawnTime;
}

int EffectDispatchTimer::GetDefaultEffectSpawnDistance() const
{
	return m_DistanceChaosState.DistanceToActivateEffect;
}

void EffectDispatchTimer::ResetSavedPosition()
{
	m_DistanceChaosState.SavedPosition = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
}

std::uint64_t EffectDispatchTimer::GetTimer() const
{
	return m_Timer;
}

void EffectDispatchTimer::ResetTimer()
{
	m_TimerPercentage                  = 0.f;
	m_Timer                            = GetTickCount64();
	m_DistanceChaosState.SavedPosition = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
}

int EffectDispatchTimer::GetRemainingTimerTime() const
{
	return static_cast<int>(
	    std::ceil(m_EffectSpawnTime
	              / (!ComponentExists<MetaModifiers>() ? 1.f : GetComponent<MetaModifiers>()->TimerSpeedModifier)
	              * (1.f - m_TimerPercentage)));
}

bool EffectDispatchTimer::ShouldDispatchEffectNow() const
{
	return GetRemainingTimerTime() <= 0;
}

void EffectDispatchTimer::SetShouldDispatchEffects(bool state)
{
	m_DispatchEffectsOnTimer = state;
}

void EffectDispatchTimer::SetFakeTimerPercentage(float percentage)
{
	m_FakeTimerPercentage = std::clamp(percentage, 0.f, 1.f);
}

void EffectDispatchTimer::ResetFakeTimerPercentage()
{
	m_FakeTimerPercentage = 0.f;
}

bool EffectDispatchTimer::IsTimerPaused() const
{
	return m_PauseTimer;
}

void EffectDispatchTimer::SetTimerPaused(bool pause)
{
	m_PauseTimer = pause;
}

bool EffectDispatchTimer::IsUsingDistanceBasedDispatch() const
{
	return m_DistanceChaosState.EnableDistanceBasedEffectDispatch;
}

void EffectDispatchTimer::OnRun()
{
	auto curTime = GetTickCount64();

	if (m_EnableTimer && m_DrawTimerBar
	    && (!ComponentExists<MetaModifiers>() || !GetComponent<MetaModifiers>()->HideChaosUI)
	    && (!ComponentExists<MetaModifiers>() || !GetComponent<MetaModifiers>()->DisableChaos))
	{
		float percentage = m_FakeTimerPercentage != 0.f ? m_FakeTimerPercentage : m_TimerPercentage;

		// Timer bar at the top
		DRAW_RECT(.5f, .01f, 1.f, .021f, 0, 0, 0, 127, false);

		if (ComponentExists<MetaModifiers>() && GetComponent<MetaModifiers>()->FlipChaosUI)
			DRAW_RECT(1.f - percentage * .5f, .01f, percentage, .018f, m_TimerColor[0], m_TimerColor[1],
			          m_TimerColor[2], 255, false);
		else
			DRAW_RECT(percentage * .5f, .01f, percentage, .018f, m_TimerColor[0], m_TimerColor[1], m_TimerColor[2], 255,
			          false);
	}

	int deltaTime = curTime - m_Timer;

	// The game was paused
	if (deltaTime > 1000)
		deltaTime = 0;

	if (!m_PauseTimer)
	{
		const TimerMode modeOverride =
		    ComponentExists<MetaModifiers>() ? GetComponent<MetaModifiers>()->TimerModeOverride : TimerMode::None;
		
		switch (modeOverride)
		{
		case TimerMode::Time:
			UpdateTimer(deltaTime);
			break;
		case TimerMode::Distance:
			UpdateTravelledDistance();
			break;
		default:
			if (m_DistanceChaosState.EnableDistanceBasedEffectDispatch)
				UpdateTravelledDistance();
			else
				UpdateTimer(deltaTime);
			break;
		}
	}

	m_Timer = curTime;
}