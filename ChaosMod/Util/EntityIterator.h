#pragma once

#include "nativesNoNamespaces.h"
#include "../Memory/Memory.h"

// Thanks to menyoo for most of these!!

class VehiclePool
{
public:
	UINT64* PoolAddress;
	UINT32 Size;
	char _Padding2[36];
	UINT32* BitArray;
	char _Padding3[40];
	UINT32 ItemCount;

	inline bool IsValid(UINT32 i)
	{
		return (BitArray[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline UINT64 GetAddress(UINT32 i)
	{
		return PoolAddress[i];
	}
};

class GenericPool
{
public:
	UINT64 PoolStartAddress;
	BYTE* ByteArray;
	UINT32 Size;
	UINT32 ItemSize;

	inline bool IsValid(UINT32 i)
	{
		return Mask(i) != 0;
	}

	inline UINT64 GetAddress(UINT32 i)
	{
		return Mask(i) & (PoolStartAddress + i * ItemSize);
	}

private:
	inline long long Mask(UINT32 i)
	{
		long long num1 = ByteArray[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};


enum class ReturnedEntityType
{
	PEDS,
	VEHS,
	PROPS
};

inline std::vector<Entity> GetAllOfEntityType(ReturnedEntityType entityType)
{
	static GenericPool* pedPool;
	static VehiclePool* vehPool;
	static GenericPool* propPool;

	static int(*_addEntityToPoolFunc)(__int64);

	std::vector<Entity> entities;

	static bool didInit = false;
	if (!didInit)
	{
		Handle handle;

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 41 0F BF C8 0F BF 40 10");
		if (!handle.IsValid())
		{
			return entities;
		}

		pedPool = handle.At(2).Into().Value<GenericPool*>();

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? F3 0F 59 F6 48 8B 08");
		if (!handle.IsValid())
		{
			return entities;
		}

		vehPool = *handle.At(2).Into().Value<VehiclePool**>();

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 8B 78 10 85 FF");
		if (!handle.IsValid())
		{
			return entities;
		}

		propPool = handle.At(2).Into().Value<GenericPool*>();

		handle = Memory::FindPattern("48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8").Addr();
		if (!handle.IsValid())
		{
			return entities;
		}

		_addEntityToPoolFunc = handle.At(-0x68).Get<int(__int64)>();

		didInit = true;
	}

	switch (entityType)
	{
	case ReturnedEntityType::PEDS:
		for (int i = 0; i < pedPool->Size; i++)
		{
			if (pedPool->IsValid(i))
			{
				__int64 addr = pedPool->GetAddress(i);

				if (addr)
				{
					int handle = _addEntityToPoolFunc(addr);

					if (DOES_ENTITY_EXIST(handle))
					{
						entities.push_back(handle);
					}
				}
			}
		}

		break;
	case ReturnedEntityType::VEHS:
		for (int i = 0; i < vehPool->Size; i++)
		{
			if (vehPool->IsValid(i))
			{
				__int64 addr = vehPool->GetAddress(i);

				if (addr)
				{
					int handle = _addEntityToPoolFunc(addr);

					if (DOES_ENTITY_EXIST(handle))
					{
						entities.push_back(handle);
					}
				}
			}
		}

		break;
	case ReturnedEntityType::PROPS:
		for (int i = 0; i < propPool->Size; i++)
		{
			if (propPool->IsValid(i))
			{
				__int64 addr = propPool->GetAddress(i);

				if (addr)
				{
					int handle = _addEntityToPoolFunc(addr);

					if (DOES_ENTITY_EXIST(handle))
					{
						entities.push_back(handle);
					}
				}
			}
		}

		break;
	}

	return entities;
}

inline const std::vector<Ped>& GetAllPeds()
{
	static std::vector<Ped> peds;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		peds = GetAllOfEntityType(ReturnedEntityType::PEDS);
	}

	return peds;
}

inline const std::vector<Vehicle>& GetAllVehs()
{
	static std::vector<Vehicle> vehs;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		vehs = GetAllOfEntityType(ReturnedEntityType::VEHS);
	}

	return vehs;
}

inline const std::vector<Object>& GetAllProps()
{
	static std::vector<Object> props;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		props = GetAllOfEntityType(ReturnedEntityType::PROPS);
	}

	return props;
}