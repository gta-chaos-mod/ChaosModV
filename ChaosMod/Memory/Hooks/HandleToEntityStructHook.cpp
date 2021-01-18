#include <stdafx.h>

static Entity m_origHandle = 0;
static Entity m_proxyHandle = 0;

__int64(*_OG_HandleToEntityStruct)(Entity entity);
__int64 _HK_HandleToEntityStruct(Entity entity)
{
	if (entity <= 0)
	{
		return 0;
	}

	if (entity == m_origHandle)
	{
		return _OG_HandleToEntityStruct(m_proxyHandle);
	}

	return _OG_HandleToEntityStruct(entity);
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
		m_origHandle = origHandle;
		m_proxyHandle = newHandle;
	}
}