#include <stdafx.h>

#include "EffectSound3D.h"

#include "Memory/Hooks/AudioPitchHook.h"
#include "Util/Camera.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#define MAX_ACTIVE_SOUNDS 10

EffectSound3D::EffectSound3D()
{
	ma_engine_init(nullptr, &m_maEngine);

	m_PauseSoundsThread = std::thread(
	    [&]()
	    {
		    while (!m_IsStopping)
		    {
			    Sleep(100);

			    if (IS_PAUSE_MENU_ACTIVE())
			    {
				    std::lock_guard lock(m_SoundsMutex);
				    for (auto &[soundId, sound] : m_Sounds)
					    ma_sound_stop(&sound.Handle);
			    }
		    }
	    });
}

void EffectSound3D::FreeSounds()
{
	for (auto &[soundId, sound] : m_Sounds)
	{
		ma_sound_stop(&sound.Handle);
		ma_sound_uninit(&sound.Handle);
	}
}

void EffectSound3D::OnModPauseCleanup()
{
	m_IsStopping = true;
	m_PauseSoundsThread.join();

	FreeSounds();

	ma_engine_uninit(&m_maEngine);
}

void EffectSound3D::OnRun()
{
	if (m_Sounds.empty())
		return;

	auto playerPed    = PLAYER_PED_ID();

	auto adjCamCoords = GET_GAMEPLAY_CAM_COORD();
	ma_engine_listener_set_position(&m_maEngine, 0, adjCamCoords.x, adjCamCoords.y, adjCamCoords.z);

	float camHeading = GET_GAMEPLAY_CAM_RELATIVE_HEADING();
	float camPitch   = GET_GAMEPLAY_CAM_RELATIVE_PITCH();
	Vector3 camRot   = { std::cos(Util::DegToRadian(camHeading)) * std::cos(Util::DegToRadian(camPitch)),
		                 std::sin(Util::DegToRadian(camPitch)),
		                 std::sin(Util::DegToRadian(camHeading)) * std::cos(Util::DegToRadian(camPitch)) };
	ma_engine_listener_set_direction(&m_maEngine, 0, camRot.x, camRot.y, camRot.z);

	auto playerVeh        = GET_VEHICLE_PED_IS_IN(playerPed, false);
	auto playerIsInAnyVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	auto adjPlayerVel     = GET_ENTITY_VELOCITY(playerIsInAnyVeh ? playerVeh : playerPed);
	ma_engine_listener_set_velocity(&m_maEngine, 0, adjPlayerVel.x, adjPlayerVel.y, adjPlayerVel.z);

	int activeSounds = 0;
	std::lock_guard lock(m_SoundsMutex);
	// Reverse order to ensure the first sounds are removed if MAX_ACTIVE_SOUNDS has been reached
	for (auto it = m_Sounds.rbegin(); it != m_Sounds.rend();)
	{
		auto &[soundId, sound] = *it;

		auto uninitSound       = [&]()
		{
			ma_sound_stop(&sound.Handle);
			ma_sound_uninit(&sound.Handle);
			it = static_cast<decltype(it)>(m_Sounds.erase(std::next(it).base()));
			activeSounds--;
		};

		if (ma_sound_at_end(&sound.Handle))
		{
			uninitSound();
			continue;
		}

		if (!ma_sound_is_playing(&sound.Handle))
			ma_sound_start(&sound.Handle);

		ma_sound_set_attenuation_model(&sound.Handle, ma_attenuation_model_linear);
		ma_sound_set_max_distance(&sound.Handle, 125.f);
		ma_sound_set_pitch(&sound.Handle,
		                   1.f + (!Hooks::GetTargetAudioPitch() ? 0.f : Hooks::GetTargetAudioPitch() * .0001f));
		ma_sound_set_looping(&sound.Handle, sound.PlayOptions.PlayType == EffectSoundPlayType::FollowEntity
		                                        && sound.PlayOptions.PlayFlags & EffectSoundPlayFlags_Looping);

		switch (sound.PlayOptions.PlayType)
		{
		case EffectSoundPlayType::FollowPlayer:
			ma_sound_set_position(&sound.Handle, adjCamCoords.x, adjCamCoords.y, adjCamCoords.z);
			break;
		case EffectSoundPlayType::FollowEntity:
		{
			if (!sound.PlayOptions.Entity || !DOES_ENTITY_EXIST(sound.PlayOptions.Entity)
			    || (!(sound.PlayOptions.PlayFlags & EffectSoundPlayFlags_DontStopOnEntityDeath)
			        && IS_ENTITY_DEAD(sound.PlayOptions.Entity, false)))
			{
				uninitSound();
				continue;
			}

			auto adjEntityCoords = GET_ENTITY_COORDS(sound.PlayOptions.Entity, true);
			ma_sound_set_position(&sound.Handle, adjEntityCoords.x, adjEntityCoords.y, adjEntityCoords.z);

			auto entityVeh        = GET_VEHICLE_PED_IS_IN(sound.PlayOptions.Entity, false);
			auto entityIsInAnyVeh = IS_PED_IN_ANY_VEHICLE(sound.PlayOptions.Entity, false);
			auto adjEntityVel     = GET_ENTITY_VELOCITY(entityIsInAnyVeh ? entityVeh : sound.PlayOptions.Entity);
			ma_engine_listener_set_velocity(&m_maEngine, 0, adjEntityVel.x, adjEntityVel.y, adjEntityVel.z);
		}
		break;
		case EffectSoundPlayType::AtCoords:
		{
			auto adjCoords = sound.PlayOptions.Coords;
			ma_sound_set_position(&sound.Handle, adjCoords.x, adjCoords.y, adjCoords.z);
		}
		break;
		default:
			LOG("ERROR: Unhandled sound play type!");
			uninitSound();
			continue;
		}

		if (++activeSounds > MAX_ACTIVE_SOUNDS)
		{
			uninitSound();
			continue;
		}

		it++;
	}
}

DWORD64 EffectSound3D::HandleSound(const std::string &soundFile)
{
	auto &sound = m_Sounds[m_SoundIdCounter];
	DEBUG_LOG("Playing sound for \"" << soundFile << "\" with ID " << m_SoundIdCounter);
	if (ma_sound_init_from_file(&m_maEngine, soundFile.c_str(), MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &sound.Handle)
	    != MA_SUCCESS)
	{
		LOG("ERROR: Couldn't play sound \"" << soundFile << "\"!");
		return 0;
	}
	ma_sound_set_volume(&sound.Handle, 1.f);
	ma_sound_start(&sound.Handle);

	return m_SoundIdCounter++;
}

void EffectSound3D::SetSoundOptions(DWORD64 soundId, const EffectSoundPlayOptions &soundPlayOptions)
{
	if (!soundId || !m_Sounds.contains(soundId))
		return;

	m_Sounds.at(soundId).PlayOptions = soundPlayOptions;
}