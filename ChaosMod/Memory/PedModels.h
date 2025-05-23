#pragma once

#include "game.h"

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

			DWORD64 archetypeMap_Map;
			WORD archetypeMap_Slots;
			DWORD64 archetypePool_ElemSize;
			DWORD64 archetypePool_PoolPtr;
			DWORD archetypePool_Size;
			DWORD64 archetypePool_AllocatedMap_Bits;

			if (IsLegacy())
			{
				handle = FindPattern("41 F7 F0 48 8B 05 ? ? ? 00 4C");
				if (!handle.IsValid())
					return pedModels;

				archetypeMap_Map   = handle.At(5).Into().Value<DWORD64>();
				archetypeMap_Slots = handle.At(-28).Into().Value<WORD>();

				handle             = FindPattern("73 17 4C 0F AF 05");
				if (!handle.IsValid())
					return pedModels;

				archetypePool_ElemSize = handle.At(5).Into().Value<DWORD64>();
				archetypePool_PoolPtr  = handle.At(12).Into().Value<DWORD64>();

				handle                 = FindPattern("3B 05 ? ? ? 00 7D 35");
				if (!handle.IsValid())
					return pedModels;

				archetypePool_Size              = handle.At(1).Into().Value<DWORD>();
				archetypePool_AllocatedMap_Bits = handle.At(18).Into().Value<DWORD64>();
			}
			else
			{
				handle = Memory::FindPattern("74 7C 49 89 D0 4C 8B 1D");

				if (!handle.IsValid())
					return pedModels;

				archetypeMap_Map                = handle.At(7).Into().Value<DWORD64>();
				archetypeMap_Slots              = handle.At(-8).Into().Value<WORD>();

				handle                          = Memory::FindPattern("7D 36 48 8B 05");

				archetypePool_Size              = handle.At(-5).Into().Value<DWORD>();
				archetypePool_AllocatedMap_Bits = handle.At(4).Into().Value<DWORD64>();

				archetypePool_PoolPtr           = handle.At(27).Into().Value<DWORD64>();
				archetypePool_ElemSize          = handle.At(43).Into().Value<DWORD64>();

				DEBUG_LOG(archetypeMap_Map << " " << archetypeMap_Slots << " " << archetypePool_Size << " "
				                           << archetypePool_AllocatedMap_Bits << " " << archetypePool_PoolPtr << " "
				                           << archetypePool_ElemSize);
			}

			for (WORD i = 0; i < archetypeMap_Slots; i++)
			{
				auto model = *reinterpret_cast<Hash **>(archetypeMap_Map + 8 * i);
				if (!model)
					continue;

				// These will crash the game, avoid at all costs !!!!
				static const Hash badModels[] = { 0x2D7030F3, 0x3F039CBA, 0x856CFB02 };

				if (std::find(std::begin(badModels), std::end(badModels), *model) == std::end(badModels))
				{
					__int64 v2 = 0;

					DWORD64 v5 = *reinterpret_cast<WORD *>(model + 4);
					LONG v6;
					// IDA copy paste ftw
					if (static_cast<DWORD>(v5) < archetypePool_Size
					    && (v6 = *reinterpret_cast<DWORD *>(archetypePool_AllocatedMap_Bits + 4 * (v5 >> 5)),
					        _bittest(&v6, v5 & 0x1F)))
					{
						v2 = *reinterpret_cast<__int64 *>(
						    archetypePool_PoolPtr
						    + archetypePool_ElemSize * *reinterpret_cast<WORD *>(reinterpret_cast<__int64>(model) + 4));
					}

					if (v2 && (*reinterpret_cast<BYTE *>(v2 + 157) & 31) == 6) // is a ped model
						pedModels.push_back(*model);
				}
			}
		}

		return pedModels;
	}
}