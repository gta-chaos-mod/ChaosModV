#pragma once

#include "Memory.h"
#include "Handle.h"

#include "../Util/Macros.h"

#include <vector>

using DWORD64 = unsigned long long;
using WORD = unsigned short;
using BYTE = unsigned char;

using Hash = unsigned long;

namespace Memory
{
	inline void SetSkyDisabled(bool bState)
	{
		static BYTE* c_pucPatchByte = nullptr;

		if (!c_pucPatchByte)
		{
			Handle handle = FindPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 48 83 C4 58");
			if (!handle.IsValid())
			{
				return;
			}

			c_pucPatchByte = handle.Into().Get<BYTE>();
		}

		Write<BYTE>(c_pucPatchByte, bState ? _INSTR_RET : _INSTR_DEC);
	}

	/*inline const std::vector<Hash>& GetTVPlaylists()
	{
		static std::vector<Hash> c_rgTVPlaylists;

		if (c_rgTVPlaylists.empty())
		{
			DWORD64 prgTVPlaylists = 0;

			Handle handle = FindPattern("48 8B 05 ? ? ? ? 89 6C C8 18");
			if (handle.IsValid())
			{
				prgTVPlaylists = handle.At(2).Into().Addr();

				for (WORD wBinkIdx = 0; wBinkIdx < *reinterpret_cast<WORD*>(prgTVPlaylists + 40); wBinkIdx++)
				{
					c_rgTVPlaylists.push_back(*reinterpret_cast<Hash*>(*reinterpret_cast<DWORD64*>(prgTVPlaylists + 32) + 24 * wBinkIdx));
				}
			}
		}

		return c_rgTVPlaylists;
	}

	inline void SetTVChannelPlaylist(int iChannel, DWORD64 a2, int iHour)
	{
		static DWORD64 c_pTVChannels = 0;
		static __int64(*c_pUpdateTVChannel)(DWORD64 a1, int iHour) = nullptr;

		if (!c_pTVChannels || !c_pUpdateTVChannel)
		{
			Handle handle = FindPattern("48 8D 05 ? ? ? ? 48 8D 34 D0");
			if (handle.IsValid())
			{
				c_pTVChannels = handle.At(2).Into().Addr();

				handle = FindPattern("E8 ? ? ? ? C6 43 10 01");
				if (handle.IsValid())
				{
					c_pUpdateTVChannel = handle.Into().Get<__int64(DWORD64, int)>();
				}
			}
		}

		if (c_pTVChannels && c_pUpdateTVChannel)
		{
			DWORD64 a = c_pTVChannels + 24 * iChannel;

			LOG(std::uppercase << std::hex << a << " | " << a2);

			*reinterpret_cast<DWORD64*>(a) = a2;

			c_pUpdateTVChannel(a, iHour);

			//*reinterpret_cast<BYTE*>(a + 16) = 1;
		}
	}*/
}