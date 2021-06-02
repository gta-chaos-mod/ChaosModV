#pragma once

#include "Memory.h"
#include "Handle.h"

#include <vector>

using DWORD64 = unsigned long long;
using WORD = unsigned short;
using BYTE = unsigned char;

using Hash = unsigned long;

namespace Memory
{
	inline std::vector<Hash> GetAllPedModels()
	{
		static std::vector<Hash> c_rgPedModels;

		if (c_rgPedModels.empty())
		{
			Handle handle;

			// TODO: Fix these patterns

			handle = FindPattern("48 8B 05 ? ? ? ? 4C 8B 14 D0 EB 09 41 3B 0A 74 54 4D 8B 52 08 4D 85 D2 75 F2 4D 8B D1 4D 85 D2 74 58 41 0F B7 02 4D 85 DB 74 03 41 89 03");
			if (!handle.IsValid())
			{
				return c_rgPedModels;
			}

			DWORD64 qword_7FF69DB37F30 = handle.At(2).Into().Value<DWORD64>();

			handle = FindPattern("0F B7 05 ? ? ? ? 45 33 C9 4C 8B DA 66 85 C0 0F 84 ? ? ? ? 44 0F B7 C0 33 D2 8B C1 41 F7 F0 48 8B 05 ? ? ? ? 4C 8B 14 D0 EB 09 41 3B 0A 74 54 4D 8B 52 08");
			if (!handle.IsValid())
			{
				return c_rgPedModels;
			}

			WORD word_7FF69DB37F38 = handle.At(2).Into().Value<WORD>();

			handle = FindPattern("4C 0F AF 05 ? ? ? ? 4C 03 05 ? ? ? ? EB 09 49 83 C2 04 EB B2 4D 8B C1 4D 85 C0 74 03 4D 8B 08 49 8B C1 C3");
			if (!handle.IsValid())
			{
				return c_rgPedModels;
			}

			DWORD64 qword_7FF69DB37EE8 = handle.At(3).Into().Value<DWORD64>();
			DWORD64 qword_7FF69DB37ED0 = handle.At(10).Into().Value<DWORD64>();

			handle = FindPattern("3B 05 ? ? ? ? 7D 35 4C 8B C0 83 E0 1F 8B D0 48 8B 05 ? ? ? ? 49 8B C8 48 C1 E9 05 8B 0C 88 0F A3 D1 73 17 4C 0F AF 05");
			if (!handle.IsValid())
			{
				return c_rgPedModels;
			}

			DWORD dword_7FF69DB37ED8 = handle.At(1).Into().Value<DWORD>();
			DWORD64 qword_7FF69DB37F00 = handle.At(18).Into().Value<DWORD64>();

			for (WORD i = 0; i < word_7FF69DB37F38; i++)
			{
				Hash* pModel = *reinterpret_cast<Hash**>(qword_7FF69DB37F30 + 8 * i);
				if (!pModel)
				{
					continue;
				}

				// These will crash the game, avoid at all costs !!!!
				static const Hash badModels[] = { 0x2D7030F3, 0x3F039CBA, 0x856CFB02 };

				if (std::find(std::begin(badModels), std::end(badModels), *pModel) == std::end(badModels))
				{
					__int64 v2 = 0;

					DWORD64 v5 = *reinterpret_cast<WORD*>(pModel + 4);
					LONG v6;
					// IDA copy paste ftw
					if (static_cast<int>(v5) < dword_7FF69DB37ED8 && (v6 = *reinterpret_cast<DWORD*>(qword_7FF69DB37F00 + 4 * (v5 >> 5)), _bittest(&v6, v5 & 0x1F)))
					{
						v2 = *reinterpret_cast<__int64*>(qword_7FF69DB37ED0 + qword_7FF69DB37EE8 * *reinterpret_cast<WORD*>(reinterpret_cast<__int64>(pModel) + 4));
					}

					if (v2 && (*reinterpret_cast<BYTE*>(v2 + 157) & 31) == 6) // is a ped model
					{
						c_rgPedModels.push_back(*pModel);
					}
				}
			}
		}

		return c_rgPedModels;
	}
}