#pragma once
#include <Windows.h>

class Main
{
private:
	Main() {}

public:
	static void Init();
	static void Loop();
	static void Stop();
	static void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);
};