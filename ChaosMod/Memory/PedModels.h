#pragma once

#include "Handle.h"
#include "Memory.h"

#include <vector>

using DWORD64 = unsigned long long;
using WORD    = unsigned short;
using BYTE    = unsigned char;

using Hash    = unsigned long;

namespace Memory
{
	inline const std::vector<Hash> &GetAllPedModels()
	{
		static std::vector<Hash> pedModels;

		if (pedModels.empty())
		{
			Handle handle;

			handle = FindPattern("41 F7 F0 48 8B 05 ? ? ? 00 4C");
			if (!handle.IsValid())
			{
				return pedModels;
			}

			DWORD64 qword_7FF69DB37F30 = handle.At(5).Into().Value<DWORD64>();

			WORD word_7FF69DB37F38     = handle.At(-28).Into().Value<WORD>();

			handle                     = FindPattern("73 17 4C 0F AF 05");
			if (!handle.IsValid())
			{
				return pedModels;
			}

			DWORD64 qword_7FF69DB37EE8 = handle.At(5).Into().Value<DWORD64>();
			DWORD64 qword_7FF69DB37ED0 = handle.At(12).Into().Value<DWORD64>();

			handle                     = FindPattern("3B 05 ? ? ? 00 7D 35");
			if (!handle.IsValid())
			{
				return pedModels;
			}

			DWORD dword_7FF69DB37ED8   = handle.At(1).Into().Value<DWORD>();
			DWORD64 qword_7FF69DB37F00 = handle.At(18).Into().Value<DWORD64>();

			for (WORD i = 0; i < word_7FF69DB37F38; i++)
			{
				auto model = *reinterpret_cast<Hash **>(qword_7FF69DB37F30 + 8 * i);
				if (!model)
				{
					continue;
				}

				// These will crash the game, avoid at all costs !!!!
				static const Hash badModels[] = { 0x2D7030F3, 0x3F039CBA, 0x856CFB02 };

				if (std::find(std::begin(badModels), std::end(badModels), *model) == std::end(badModels))
				{
					__int64 v2 = 0;

					DWORD64 v5 = *reinterpret_cast<WORD *>(model + 4);
					LONG v6;
					// IDA copy paste ftw
					if (static_cast<int>(v5) < dword_7FF69DB37ED8
					    && (v6 = *reinterpret_cast<DWORD *>(qword_7FF69DB37F00 + 4 * (v5 >> 5)),
					        _bittest(&v6, v5 & 0x1F)))
					{
						v2 = *reinterpret_cast<__int64 *>(
						    qword_7FF69DB37ED0
						    + qword_7FF69DB37EE8 * *reinterpret_cast<WORD *>(reinterpret_cast<__int64>(model) + 4));
					}

					if (v2 && (*reinterpret_cast<BYTE *>(v2 + 157) & 31) == 6) // is a ped model
					{
						pedModels.push_back(*model);
					}
				}
			}
		}

		return pedModels;
	}
}