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
		void *m_pPageMap;
		DWORD64 *m_pCodeBlocks;
		DWORD m_nGlobalsSignature;
		DWORD m_nCodeSize;
		DWORD m_nParameterCount;
		DWORD m_nStaticCount;
		DWORD m_nGlobalCount;
		DWORD m_nNativesCount;
		void *m_pStaticsPointer;
		void *m_pGlobalsPointer;
		void **m_pNativesPointer;
		DWORD64 field_0x48;
		DWORD64 field_0x50;
		DWORD64 m_nScriptHash;
		int field_0x5c;
		void *m_pScriptNamePointer;
		DWORD64 *m_pStringBlocks;
		DWORD m_nStringSize;
		int field_0x74;
		int field_0x78;
		int field_0x7c;
	};

	class scrThread
	{
	  private:
		DWORD m_dwThreadId;
		DWORD m_dwScriptHash;
		DWORD dwSomething2;
		DWORD m_dwIP;
		char pad[184];
		char pad_2699[4];
		char m_szName[32];
		char pad3[100];
		char chSomething3;
		char pad4[3];

		virtual ~scrThread()                                 = 0;

		virtual DWORD *Reset(int a2, const void *a3, int a4) = 0;

		virtual __int64 OnRun()                              = 0;

		virtual __int64 Update()                             = 0;

		virtual __int64 Kill()                               = 0;

	public:
		const char *GetName() const
		{
			return getGameVersion() < eGameVersion::VER_1_0_2699_0_STEAM ? pad_2699 : m_szName;
		}

		DWORD GetHash() const
		{
			return m_dwScriptHash;
		}
	};
}

static_assert(sizeof(rage::scrThread) == 352);