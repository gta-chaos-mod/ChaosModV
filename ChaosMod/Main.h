#pragma once

using DWORD = unsigned long;
using WORD  = unsigned short;
using BYTE  = unsigned char;
using BOOL  = int; // wtf?

class IDXGISwapChain;

namespace Main
{
	void OnRun();
	void OnCleanup();
	void OnKeyboardInput(DWORD ulKey, WORD usRepeats, BYTE ucScanCode, BOOL bIsExtended, BOOL bIsWithAlt,
	                     BOOL bWasDownBefore, BOOL bIsUpNow);
}