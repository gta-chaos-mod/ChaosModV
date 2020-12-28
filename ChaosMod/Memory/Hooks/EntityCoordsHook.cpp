#include <stdafx.h>

static bool m_enabledHook = false;
static Vector3 m_prevPos;

Vector3(*OG_GET_ENTITY_COORDS)(Entity entity, bool alive);
Vector3 HK_GET_ENTITY_COORDS(Entity entity, bool alive)
{
	if (m_enabledHook)
	{
		Ped playerPed = PLAYER_PED_ID();
		Vehicle playerVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false) : 0;

		if (entity == playerPed || entity == playerVeh)
		{
			return m_prevPos;
		}
	}

	return OG_GET_ENTITY_COORDS(entity, alive);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("E9 ? ? ? ? C3 90 90 E9 2C 65 E8 48");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_GET_ENTITY_COORDS, &OG_GET_ENTITY_COORDS);

	return true;
}

static RegisterHook registerHook(OnHook, "GET_ENTITY_COORDS");

namespace Hooks
{
	void EnableFakeTpHook(const Vector3& prevPos)
	{
		m_enabledHook = true;
		m_prevPos = prevPos;
	}

	void DisableFakeTpHook()
	{
		m_enabledHook = false;
	}
}