/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

#include "Memory/Weapon.h"
#include "Memory/WeaponPool.h"

#include "Util/PedVoices.h"

static const uint32_t replaceAudio = 0U;

std::unordered_map<Hash, uint32_t> weaponAudios;

static void OnStop()
{
	for (auto const &[wep, audio] : weaponAudios)
	{
		Memory::SetWeaponAudio(wep, audio);
	}

	weaponAudios.clear();

	for (Ped ped : GetAllPeds())
	{
		Hash curPedWep;
		GET_CURRENT_PED_WEAPON(ped, &curPedWep, false);
		if (curPedWep != 0xA2719263)
		{
			SET_CURRENT_PED_WEAPON(ped, 0xA2719263, true);
			SET_CURRENT_PED_WEAPON(ped, curPedWep, true);
		}
	}
}

static void OnTick()
{
	static DWORD64 lastTick = GetTickCount64();
	DWORD64 curTick         = GetTickCount64();

	for (Ped ped : GetAllPeds())
	{
		Hash curPedWep;
		GET_CURRENT_PED_WEAPON(ped, &curPedWep, false);

		if (!weaponAudios.contains(curPedWep))
		{
			weaponAudios.insert({ curPedWep, Memory::GetWeaponAudio(curPedWep) });
			Memory::SetWeaponAudio(curPedWep, replaceAudio);
			SET_CURRENT_PED_WEAPON(ped, 0xA2719263, true);
			SET_CURRENT_PED_WEAPON(ped, curPedWep, true);
		}

		if (IS_PED_SHOOTING(ped) && curTick - lastTick > 2000)
		{
			lastTick              = curTick;
			const char *voiceName = Util::GetRandomPedVoiceName();
			Vector3 soundCoords   = GET_ENTITY_COORDS(ped, false);

			PLAY_AMBIENT_SPEECH_FROM_POSITION_NATIVE("GENERIC_INSULT_HIGH", voiceName, soundCoords.x, soundCoords.y,
			                                         soundCoords.z, "SPEECH_PARAMS_INTERRUPT_SHOUTED");
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "These Bullets Mean",
		.Id = "misc_insultguns",
		.IsTimed = true
	}
);