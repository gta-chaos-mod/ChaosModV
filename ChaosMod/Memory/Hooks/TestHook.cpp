#include <stdafx.h>

#ifdef _DEBUG

__int64 (*OG_sub_7FF788D32A60)(unsigned int a1);
__int64 HK_sub_7FF788D32A60(unsigned int a1)
{
	DEBUG_LOG("======");
	DEBUG_LOG(a1);

	DWORD64 result = OG_sub_7FF788D32A60(a1);

	DEBUG_LOG(std::hex << std::uppercase << result);
	DEBUG_LOG(Memory::GetTypeName(result));
	DEBUG_LOG("======");

	return result;
}

static bool OnHook()
{
	Handle handle;

	//

	/*handle = Memory::FindPattern("E8 ? ? ? ? 48 85 FF 74 47");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_sub_7FF788D32A60, &OG_sub_7FF788D32A60);*/

	return true;
}

static RegisterHook registerHook(OnHook, "TestHook");

#endif