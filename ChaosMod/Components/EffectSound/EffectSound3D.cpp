#include <stdafx.h>

#include "Util/Camera.h"

#include "EffectSound3D.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#define SOUND_RANGE_MULT .1f
#define SOUND_VEL_MULT .5f

EffectSound3D::EffectSound3D()
{
	ma_engine_init(nullptr, &m_maEngine);
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
	FreeSounds();

	ma_engine_uninit(&m_maEngine);
}

void EffectSound3D::OnRun()
{
	if (m_Sounds.empty())
		return;

	auto playerPed       = PLAYER_PED_ID();

	auto adjPlayerCoords = GET_ENTITY_COORDS(playerPed, true) * SOUND_RANGE_MULT;
	ma_engine_listener_set_position(&m_maEngine, 0, adjPlayerCoords.x, adjPlayerCoords.y, adjPlayerCoords.z);

	auto camRot = Util::DegToRadian(GET_GAMEPLAY_CAM_ROT(2));
	ma_engine_listener_set_direction(&m_maEngine, 0, camRot.x, camRot.y, camRot.z);

	auto playerVeh        = GET_VEHICLE_PED_IS_IN(playerPed, false);
	auto playerIsInAnyVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	auto adjPlayerVel     = GET_ENTITY_VELOCITY(playerIsInAnyVeh ? playerVeh : playerPed) * SOUND_VEL_MULT;
	ma_engine_listener_set_velocity(&m_maEngine, 0, adjPlayerVel.x, adjPlayerVel.y, adjPlayerVel.z);

	for (auto it = m_Sounds.begin(); it != m_Sounds.end();)
	{
		auto &[soundId, sound] = *it;

		auto uninitSound       = [&]()
		{
			ma_sound_stop(&sound.Handle);
			ma_sound_uninit(&sound.Handle);
			it = m_Sounds.erase(it);
		};

		if (ma_sound_at_end(&sound.Handle))
		{
			uninitSound();
			continue;
		}

		switch (sound.PlayOptions.PlayType)
		{
		case EffectSoundPlayType::FollowPlayer:
			ma_sound_set_position(&sound.Handle, adjPlayerCoords.x, adjPlayerCoords.y, adjPlayerCoords.z);
			break;
		case EffectSoundPlayType::FollowEntity:
		{
			if (!sound.PlayOptions.Entity || !DOES_ENTITY_EXIST(sound.PlayOptions.Entity))
			{
				uninitSound();
				continue;
			}

			auto adjEntityCoords = GET_ENTITY_COORDS(sound.PlayOptions.Entity, true) * SOUND_RANGE_MULT;
			ma_sound_set_position(&sound.Handle, adjEntityCoords.x, adjEntityCoords.y, adjEntityCoords.z);

			auto entityVeh        = GET_VEHICLE_PED_IS_IN(sound.PlayOptions.Entity, false);
			auto entityIsInAnyVeh = IS_PED_IN_ANY_VEHICLE(sound.PlayOptions.Entity, false);
			auto adjEntityVel =
			    GET_ENTITY_VELOCITY(entityIsInAnyVeh ? entityVeh : sound.PlayOptions.Entity) * SOUND_VEL_MULT;
			ma_engine_listener_set_velocity(&m_maEngine, 0, adjEntityVel.x, adjEntityVel.y, adjEntityVel.z);
		}
		break;
		case EffectSoundPlayType::AtCoords:
		{
			auto adjCoords = sound.PlayOptions.Coords * SOUND_RANGE_MULT;
			ma_sound_set_position(&sound.Handle, adjCoords.x, adjCoords.y, adjCoords.z);
		}
		break;
		default:
			LOG("ERROR: Unhandled sound play type!");
			uninitSound();
			continue;
		}

		it++;
	}
}

DWORD64 EffectSound3D::HandleSound(const std::string &soundFile)
{
	auto &sound = m_Sounds[m_SoundIdCounter];
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