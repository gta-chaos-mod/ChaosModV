#pragma once

#include "Natives.h"

#include "Memory/Memory.h"

#include <vector>

#define _NODISCARD [[nodiscard]]

// Thanks to menyoo for most of these!!

// Pool Interator class to iterate over pools. Has just enough operators defined to be able to be used in a for loop,
// not suitable for any other iterating.
template <typename T> class PoolIterator
{
  public:
	T *Pool       = nullptr;
	int32_t Index = 0;

	explicit PoolIterator(T *pool, int32_t index = 0)
	{
		this->Pool  = pool;
		this->Index = index;
	}

	PoolIterator &operator++()
	{
		for (Index++; Index < Pool->m_ulSize; Index++)
		{
			if (Pool->IsValid(Index))
			{
				return *this;
			}
		}

		Index = Pool->m_ulSize;
		return *this;
	}

	Entity operator*()
	{
		static int (*_addEntityToPoolFunc)(__int64) = []
		{
			Handle handle = Memory::FindPattern("48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8").Addr();
			return handle.At(-0x68).Get<int(__int64)>();
		}();

		__int64 ullAddr = Pool->GetAddress(Index);
		int iHandle     = _addEntityToPoolFunc(ullAddr);
		return iHandle;
	}

	bool operator!=(const PoolIterator &other) const
	{
		return this->Index != other.Index;
	}
};

// Common functions for VehiclePool and GenericPool
template <typename T> class PoolUtils
{
  public:
	inline auto ToArray()
	{
		std::vector<Entity> arr;
		for (auto entity : *static_cast<T *>(this))
		{
			arr.push_back(entity);
		}

		return arr;
	}

	auto begin()
	{
		return ++PoolIterator<T>(static_cast<T *>(this), -1);
	}

	auto end()
	{
		return ++PoolIterator<T>(static_cast<T *>(this), static_cast<T *>(this)->m_ulSize);
	}
};

class VehiclePool : public PoolUtils<VehiclePool>
{
  public:
	UINT64 *m_pullPoolAddress;
	UINT32 m_ulSize;
	char _Padding2[36];
	UINT32 *m_pulBitArray;
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

class GenericPool : public PoolUtils<GenericPool>
{
  public:
	UINT64 m_ullPoolStartAddress;
	BYTE *m_ucByteArray;
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

_NODISCARD inline auto &GetAllPeds()
{
	static GenericPool *pPedPool = []
	{
		Handle handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 41 0F BF C8 0F BF 40 10");
		return handle.At(2).Into().Value<GenericPool *>();
	}();

	return *pPedPool;
}

_NODISCARD inline auto &GetAllVehs()
{
	static VehiclePool *pVehPool = []
	{
		Handle handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? F3 0F 59 F6 48 8B 08");
		return *handle.At(2).Into().Value<VehiclePool **>();
	}();

	return *pVehPool;
}

_NODISCARD inline auto &GetAllProps()
{
	static GenericPool *pPropPool = []
	{
		Handle handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 8B 78 10 85 FF");
		return handle.At(2).Into().Value<GenericPool *>();
	}();

	return *pPropPool;
}

_NODISCARD inline auto GetAllPedsArray()
{
	return GetAllPeds().ToArray();
}

_NODISCARD inline auto GetAllVehsArray()
{
	return GetAllVehs().ToArray();
}

_NODISCARD inline auto GetAllPropsArray()
{
	return GetAllProps().ToArray();
}