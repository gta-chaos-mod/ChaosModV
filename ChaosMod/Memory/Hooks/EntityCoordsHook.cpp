#include <stdafx.h>

static bool m_enabledHook = false;
static std::unordered_map<Entity, Vector3> m_savedEntityCoords;

Vector3(*OG_GET_ENTITY_COORDS)(Entity entity, bool alive);
Vector3 HK_GET_ENTITY_COORDS(Entity entity, bool alive)
{
	if (m_enabledHook)
	{
		auto result = m_savedEntityCoords.find(entity);

		if (result != m_savedEntityCoords.end())
		{
			return result->second;
		}
	}

	return OG_GET_ENTITY_COORDS(entity, alive);
}

static bool OnHook()
{
	Handle handle;

	// Not really a fan of this sig either but the best I could do :shrug:
	/*handle = Memory::FindPattern("E9 ? ? ? ? C3 90 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ? 90");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_GET_ENTITY_COORDS, &OG_GET_ENTITY_COORDS);*/

	return true;
}

static RegisterHook registerHook(OnHook, "GET_ENTITY_COORDS");

namespace Hooks
{
	void EnableFakeTpHook()
	{
		m_enabledHook = true;
		
		for (Ped ped : GetAllPeds())
		{
			m_savedEntityCoords.emplace(ped, GET_ENTITY_COORDS(ped, false));
		}

		for (Vehicle veh : GetAllVehs())
		{
			m_savedEntityCoords.emplace(veh, GET_ENTITY_COORDS(veh, false));
		}
	}

	void DisableFakeTpHook()
	{
		m_enabledHook = false;

		m_savedEntityCoords.clear();
	}
}