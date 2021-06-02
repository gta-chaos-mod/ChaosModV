#pragma once

using DWORD = unsigned long;

namespace rage
{
	class scrThread
	{
	public:
		DWORD m_dwThreadId;
		DWORD m_dwProgramId;
		DWORD dwSomething2;
		DWORD m_dwIP;
		char pad[184];
		char m_szName[32];
		char pad3[100];
		char chSomething3;
		char pad4[3];

		virtual ~scrThread() = 0;

		virtual DWORD* Reset(int a2, const void* a3, int a4) = 0;

		virtual __int64 Run() = 0;

		virtual __int64 Update() = 0;

		virtual __int64 Kill() = 0;
	};
}

static_assert(sizeof(rage::scrThread) == 344);