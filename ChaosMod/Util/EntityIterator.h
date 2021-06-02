#pragma once

#include "Natives.h"

#include "../Memory/Memory.h"

// Thanks to menyoo for most of these!!

class VehiclePool
{
public:
	UINT64* m_pullPoolAddress;
	UINT32 m_ulSize;
	char _Padding2[36];
	UINT32* m_pulBitArray;
	char _Padding3[40];
	UINT32 m_ulItemCount;

	inline bool IsValid(UINT32 i)
	{
		return (m_pulBitArray[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline UINT64 GetAddress(UINT32 i)
	{
		return m_pullPoolAddress[i];
	}
};

class GenericPool
{
public:
	UINT64 m_ullPoolStartAddress;
	BYTE* m_ucByteArray;
	UINT32 m_ulSize;
	UINT32 m_ulItemSize;

	inline bool IsValid(UINT32 i)
	{
		return Mask(i) != 0;
	}

	inline UINT64 GetAddress(UINT32 i)
	{
		return Mask(i) & (m_ullPoolStartAddress + i * m_ulItemSize);
	}

private:
	inline long long Mask(UINT32 i)
	{
		long long num1 = m_ucByteArray[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};


enum class EReturnedEntityType
{
	Peds,
	Vehs,
	Props
};

inline std::vector<Entity> GetAllOfEntityType(EReturnedEntityType entityType)
{
	static GenericPool* pPedPool;
	static VehiclePool* pVehPool;
	static GenericPool* pPropPool;

	static int(*_addEntityToPoolFunc)(__int64);

	std::vector<Entity> rgEntities;

	static bool c_bDidInit = false;
	if (!c_bDidInit)
	{
		Handle handle;

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 41 0F BF C8 0F BF 40 10");
		if (!handle.IsValid())
		{
			return rgEntities;
		}

		pPedPool = handle.At(2).Into().Value<GenericPool*>();

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? F3 0F 59 F6 48 8B 08");
		if (!handle.IsValid())
		{
			return rgEntities;
		}

		pVehPool = *handle.At(2).Into().Value<VehiclePool**>();

		handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 8B 78 10 85 FF");
		if (!handle.IsValid())
		{
			return rgEntities;
		}

		pPropPool = handle.At(2).Into().Value<GenericPool*>();

		handle = Memory::FindPattern("48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8").Addr();
		if (!handle.IsValid())
		{
			return rgEntities;
		}

		_addEntityToPoolFunc = handle.At(-0x68).Get<int(__int64)>();

		c_bDidInit = true;
	}

	switch (entityType)
	{
	case EReturnedEntityType::Peds:
		for (int i = 0; i < pPedPool->m_ulSize; i++)
		{
			if (pPedPool->IsValid(i))
			{
				__int64 ullAddr = pPedPool->GetAddress(i);

				if (ullAddr)
				{
					int iHandle = _addEntityToPoolFunc(ullAddr);

					if (DOES_ENTITY_EXIST(iHandle))
					{
						rgEntities.push_back(iHandle);
					}
				}
			}
		}

		break;
	case EReturnedEntityType::Vehs:
		for (int i = 0; i < pVehPool->m_ulSize; i++)
		{
			if (pVehPool->IsValid(i))
			{
				__int64 ullAddr = pVehPool->GetAddress(i);

				if (ullAddr)
				{
					int iHandle = _addEntityToPoolFunc(ullAddr);

					if (DOES_ENTITY_EXIST(iHandle))
					{
						rgEntities.push_back(iHandle);
					}
				}
			}
		}

		break;
	case EReturnedEntityType::Props:
		for (int i = 0; i < pPropPool->m_ulSize; i++)
		{
			if (pPropPool->IsValid(i))
			{
				__int64 ullAddr = pPropPool->GetAddress(i);

				if (ullAddr)
				{
					int iHandle = _addEntityToPoolFunc(ullAddr);

					if (DOES_ENTITY_EXIST(iHandle))
					{
						rgEntities.push_back(iHandle);
					}
				}
			}
		}

		break;
	}

	return rgEntities;
}

inline const std::vector<Ped>& GetAllPeds()
{
	static std::vector<Ped> c_rgPeds;

	static int c_ilastFrame = 0;
	int iCurFrame = GET_FRAME_COUNT();
	if (c_ilastFrame < iCurFrame)
	{
		c_ilastFrame = iCurFrame;

		c_rgPeds = GetAllOfEntityType(EReturnedEntityType::Peds);
	}

	return c_rgPeds;
}

inline const std::vector<Vehicle>& GetAllVehs()
{
	static std::vector<Vehicle> rgVehs;

	static int c_iLastFrame = 0;
	int iCurFrame = GET_FRAME_COUNT();
	if (c_iLastFrame < iCurFrame)
	{
		c_iLastFrame = iCurFrame;

		rgVehs = GetAllOfEntityType(EReturnedEntityType::Vehs);
	}

	return rgVehs;
}

inline const std::vector<Object>& GetAllProps()
{
	static std::vector<Object> rgProps;

	static int iLastFrame = 0;
	int iCurFrame = GET_FRAME_COUNT();
	if (iLastFrame < iCurFrame)
	{
		iLastFrame = iCurFrame;

		rgProps = GetAllOfEntityType(EReturnedEntityType::Props);
	}

	return rgProps;
}