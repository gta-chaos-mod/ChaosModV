#pragma once

using DWORD = unsigned long;
using WORD  = unsigned short;
using BYTE  = unsigned char;
using BOOL  = int; // wtf?

struct IDXGISwapChain;

namespace Main
{
	void OnRun();
	void OnCleanup();
	void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore,
	                     BOOL isUpNow);
	void Stop();
}