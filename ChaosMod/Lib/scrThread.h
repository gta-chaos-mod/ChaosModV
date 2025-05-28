#pragma once

#include <scripthookv/inc/main.h>

using DWORD   = unsigned long;
using DWORD64 = unsigned long long;

namespace rage
{
	// Thanks to rainbomizer for this def!
	struct scrProgram
	{
		static const uint32_t PAGE_SIZE = 0x4000;

		void *vft;
		void *m_PageMap;
		DWORD64 *m_CodeBlocks;
		DWORD m_GlobalsSignature;
		DWORD m_CodeSize;
		DWORD m_ParameterCount;
		DWORD m_StaticCount;
		DWORD m_GlobalCount;
		DWORD m_NativesCount;
		void *m_StaticsPointer;
		void *m_GlobalsPointer;
		void **m_NativesPointer;
		DWORD64 field_0x48;
		DWORD64 field_0x50;
		DWORD64 m_ScriptHash;
		int field_0x5c;
		void *m_ScriptNamePointer;
		DWORD64 *m_StringBlocks;
		DWORD m_StringSize;
		int field_0x74;
		int field_0x78;
		int field_0x7c;
	};

	class scrThread
	{
	  private:
		DWORD m_ThreadId;
		DWORD m_ScriptHash;
		DWORD dwSomething2;
		DWORD m_IP;
		char pad[184];
		char pad_2699[4];
		char m_Name[32];
		char pad3[100];
		char chSomething3;
		char pad4[3];

		virtual ~scrThread()                                 = 0;

		virtual DWORD *Reset(int a2, const void *a3, int a4) = 0;

		virtual __int64 OnRun()                              = 0;

		virtual __int64 Update()                             = 0;

		virtual __int64 Kill()                               = 0;

	  public:
		DWORD GetThreadId() const
		{
			return m_ThreadId;
		}

		DWORD GetHash() const
		{
			return m_ScriptHash;
		}

		const char *GetName() const
		{
			return getGameVersion() < eGameVersion::VER_1_0_2699_0_STEAM ? pad_2699 : m_Name;
		}
	};
}

static_assert(sizeof(rage::scrThread) == 352);