#include <stdafx.h>

#ifdef _DEBUG


static bool OnHook()
{
	return true;
}

static RegisterHook registerHook(OnHook, "TestHook");

#endif