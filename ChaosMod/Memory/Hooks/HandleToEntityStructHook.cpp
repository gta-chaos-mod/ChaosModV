#include <stdafx.h>

static std::unordered_map<Entity, Entity> vehicleMap;

__int64(*_OG_HandleToEntityStruct)(Entity entity);
__int64 _HK_HandleToEntityStruct(Entity entity)
{
	if (entity <= 0)
	{
		return 0;
	}
	Entity vehToContinue = entity;
	while (vehicleMap.count(vehToContinue) > 0)
	{
		vehToContinue = vehicleMap[vehToContinue];
    	if (vehToContinue <= 0) 
        {
     		return 0;
    	}
	}
	return _OG_HandleToEntityStruct(vehToContinue);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("83 F9 FF 74 31 4C 8B 0D");
	if (!handle.IsValid())
	{
		return false;
	}	

	Memory::AddHook(handle.Get<void>(), _HK_HandleToEntityStruct, &_OG_HandleToEntityStruct);

	return true;
}

static RegisterHook registerHook(OnHook, "_HandleToEntityStruct");

namespace Hooks
{
	void ProxyEntityHandle(Entity origHandle, Entity newHandle)
	{
		vehicleMap.emplace(origHandle, newHandle);
		// CleanUp
		bool found = false;
		do 
		{
			found = false;
			for (std::unordered_map<Entity, Entity>::iterator it = vehicleMap.begin(); it != vehicleMap.end(); )
			{
				if (!DOES_ENTITY_EXIST(it->second))
				{
					it = vehicleMap.erase(it);
					found = true;
				}
				else
				{
					it++;
				}
			}
		} 
		while (found);
	}
}
