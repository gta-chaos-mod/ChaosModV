#pragma once

#include <scripthookv/inc/main.h>

#include "game.h"

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
		DWORD m_ScriptHash;
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
		struct scrThreadLegacyPre2699
		{
			void *vft;
			DWORD m_ThreadId;
			DWORD m_ScriptHash;
			DWORD dwSomething2;
			DWORD m_IP;
			char pad[184];
			char m_Name[32];
			char pad3[100];
			char chSomething3;
			char pad4[3];
		};

		struct scrThreadLegacy
		{
			void *vft;
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
		};

		struct scrThreadEnhanced
		{
			void *vft;
			DWORD m_ThreadId;
			char pad0[4];
			DWORD m_ScriptHash;
			DWORD dwSomething2;
			DWORD m_IP;
			char pad1[184];
			char pad_Enhanced[128];
			char m_Name[32];
			char pad3[100];
			char chSomething3;
			char pad4[3];
		};

	  public:
		DWORD GetThreadId() const
		{
			if (IsLegacy())
				return reinterpret_cast<const scrThreadLegacy *>(this)->m_ThreadId;
			else
				return reinterpret_cast<const scrThreadEnhanced *>(this)->m_ThreadId;
		}

		DWORD GetHash() const
		{
			if (IsLegacy())
				return reinterpret_cast<const scrThreadLegacy *>(this)->m_ScriptHash;
			else
				return reinterpret_cast<const scrThreadEnhanced *>(this)->m_ScriptHash;
		}

		const char *GetName() const
		{
			if (getGameVersion() < eGameVersion::VER_1_0_2699_0_STEAM)
				return reinterpret_cast<const scrThreadLegacyPre2699 *>(this)->m_Name;
			else if (IsLegacy())
				return reinterpret_cast<const scrThreadLegacy *>(this)->m_Name;
			else
				return reinterpret_cast<const scrThreadEnhanced *>(this)->m_Name;
		}
	};
}