#include <stdafx.h>

#ifdef _DEBUG

__int64(*OG_FUN_7ff78845e500)(int a1);
__int64 HK_FUN_7ff78845e500(int a1)
{
	__int64 result = OG_FUN_7ff78845e500(a1);

	DEBUG_LOG(Memory::GetTypeName(result));

	return result;
}

static bool OnHook()
{
	Handle handle;

	//

	handle = Memory::FindPattern("40 53 48 83 ec 20 8b d9 e8 0b e0 f5 00 8b cb e8 2c 70 46 01");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Get<void>(), HK_FUN_7ff78845e500, &OG_FUN_7ff78845e500);

	return true;
}

static RegisterHook registerHook(OnHook, "TestHook");

#endif