#pragma once

#include "Memory/Memory.h"

#include <scripthookv/inc/natives.h>
#include <scripthookv/inc/types.h>

#include <vector>

#include "Util/Logging.h"

// Thanks to menyoo for most of these!!

// Pool Interator class to iterate over pools. Has just enough operators defined to be able to be used in a for loop,
// not suitable for any other iterating.
template <typename T> class PoolIterator
{
  public:
	T *Pool            = nullptr;
	std::int32_t Index = 0;

	explicit PoolIterator(T *pool, int32_t index = 0)
	{
		this->Pool  = pool;
		this->Index = index;
	}

	PoolIterator &operator++()
	{
		for (Index++; Index < Pool->GetSize(); Index++)
			if (Pool->IsValid(Index))
				return *this;

		Index = Pool->GetSize();
		return *this;
	}

	Entity operator*()
	{
		static int (*_addEntityToPoolFunc)(__int64) = []
		{
			Handle handle =
			    Memory::FindPattern("48 F7 F9 49 8B 48 08 48 63 D0 C1 E0 08 0F B6 1C 11 03 D8", "48 8D 71 10 8B 15");
			return handle.At(IsLegacy() ? -0x68 : -0x9).Get<int(__int64)>();
		}();

		auto addr  = Pool->GetAddress(Index);
		int handle = _addEntityToPoolFunc(addr);
		return handle;
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
			arr.push_back(entity);

		return arr;
	}

	auto begin()
	{
		return ++PoolIterator<T>(static_cast<T *>(this), -1);
	}

	auto end()
	{
		return ++PoolIterator<T>(static_cast<T *>(this), static_cast<T *>(this)->GetSize());
	}

	virtual bool IsValid(UINT32 i)      = 0;
	virtual UINT64 GetAddress(UINT32 i) = 0;
	virtual UINT32 GetSize()            = 0;
};

class VehiclePool : public PoolUtils<VehiclePool>
{
  private:
	struct LegacyPool
	{
		UINT64 *m_PoolAddress;
		UINT32 m_Size;
		char _Padding2[36];
		UINT32 *m_BitArray;
		char _Padding3[40];
		UINT32 m_ItemCount;
	};

	struct EnhancedPool
	{
		void *vft;
		UINT64 *m_PoolAddress;
		UINT32 m_Size;
		char _Padding2[36];
		UINT32 *m_BitArray;
		char _Padding3[40];
		UINT32 m_ItemCount;
	};

	union Pool
	{
		LegacyPool lp;
		EnhancedPool ep;
	};

	Pool *pool;

  public:
	VehiclePool(DWORD64 address)
	{
		pool = *reinterpret_cast<Pool **>(address);
	}

	VehiclePool() : VehiclePool(0)
	{
	}

	inline bool IsValid(UINT32 i) override
	{
		if (IsEnhanced())
			return (pool->ep.m_BitArray[i >> 5] >> (i & 0x1F)) & 1;
		else
			return (pool->lp.m_BitArray[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline UINT64 GetAddress(UINT32 i) override
	{
		if (IsEnhanced())
			return pool->ep.m_PoolAddress[i];
		else
			return pool->lp.m_PoolAddress[i];
	}

	inline UINT32 GetSize() override
	{
		if (IsEnhanced())
			return pool->ep.m_Size;
		else
			return pool->lp.m_Size;
	}
};

class GenericPool : public PoolUtils<GenericPool>
{
  private:
	struct LegacyPool
	{
		UINT64 m_PoolStartAddress;
		BYTE *m_ByteArray;
		UINT32 m_Size;
		UINT32 m_ItemSize;
	};

	struct EnhancedPool
	{
		void *vft;
		UINT64 m_PoolStartAddress;
		BYTE *m_ByteArray;
		UINT32 m_Size;
		UINT32 m_ItemSize;
	};

	union Pool
	{
		LegacyPool lp;
		EnhancedPool ep;
	};

	Pool *pool;

  public:
	GenericPool(DWORD64 address)
	{
		pool = reinterpret_cast<Pool *>(address);
	}

	GenericPool() : GenericPool(0)
	{
	}

	inline bool IsValid(UINT32 i) override
	{
		return Mask(i) != 0;
	}

	inline UINT64 GetAddress(UINT32 i) override
	{
		if (IsEnhanced())
			return Mask(i) & (pool->ep.m_PoolStartAddress + i * pool->ep.m_ItemSize);
		else
			return Mask(i) & (pool->lp.m_PoolStartAddress + i * pool->lp.m_ItemSize);
	}

	inline UINT32 GetSize() override
	{
		if (IsEnhanced())
			return pool->ep.m_Size;
		else
			return pool->lp.m_Size;
	}

  private:
	inline long long Mask(UINT32 i)
	{
		long long num1;
		if (IsEnhanced())
			num1 = pool->ep.m_ByteArray[i] & 0x80;
		else
			num1 = pool->lp.m_ByteArray[i] & 0x80;

		return ~((num1 | -num1) >> 63);
	}
};

template <typename T> class EncryptedPointer
{
  public: // protected
	struct Encrypted
	{
		bool m_IsSet;
		DWORD64 m_First;
		DWORD64 m_Second;
	};

	Encrypted *val;

  public:
	virtual T GetPool() = 0;

	EncryptedPointer(DWORD64 address)
	{
		val = reinterpret_cast<Encrypted *>(address);
	}

	EncryptedPointer() : EncryptedPointer(0)
	{
	}
};

class PedPoolEncryptedPointer : public EncryptedPointer<GenericPool>
{
  public:
	GenericPool GetPool() override
	{
		if (val->m_IsSet)
		{
			DWORD64 temp = _rotl64(val->m_Second, 30);
			return GenericPool(~_rotl64(_rotl64(temp ^ val->m_First, 32), (temp & 0x1F) + 2));
		}

		return GenericPool();
	}

	PedPoolEncryptedPointer(DWORD64 address) : EncryptedPointer(address)
	{
	}

	PedPoolEncryptedPointer() : EncryptedPointer()
	{
	}
};

class ObjectPoolEncryptedPointer : public EncryptedPointer<GenericPool>
{
  public:
	GenericPool GetPool() override
	{
		if (val->m_IsSet)
		{
			DWORD64 temp = _rotl64(val->m_Second, 30);
			return GenericPool(~_rotl64(_rotl64(temp ^ val->m_First, (temp & 0x1F) + 3), 32));
		}

		return GenericPool();
	}

	ObjectPoolEncryptedPointer(DWORD64 address) : EncryptedPointer(address)
	{
	}

	ObjectPoolEncryptedPointer() : EncryptedPointer()
	{
	}
};

inline auto &GetAllPeds()
{
	static GenericPool pedPool = []
	{
		if (IsLegacy())
		{
			auto handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 41 0F BF C8 0F BF 40 10");
			return GenericPool(handle.At(2).Into().Value<UINT64>());
		}
		else
		{
			auto handle       = Memory::FindPattern("0F B6 05 ?? ?? ?? ?? A8 01 75 0F 45 31 F6");
			auto encryptedPtr = PedPoolEncryptedPointer(handle.At(2).Into().Addr());
			return encryptedPtr.GetPool();
		}
	}();

	return pedPool;
}

inline auto &GetAllVehs()
{
	static VehiclePool vehPool = []
	{
		if (IsLegacy())
		{
			auto handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? F3 0F 59 F6 48 8B 08");
			return VehiclePool(handle.At(2).Into().Value<UINT64>());
		}
		else
		{
			auto handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 48 8B 00 48 83 78 18 05");
			return VehiclePool(handle.At(2).Into().Value<UINT64>());
		}
	}();

	return vehPool;
}

inline auto &GetAllProps()
{
	static GenericPool propPool = []
	{
		if (IsLegacy())
		{
			auto handle = Memory::FindPattern("48 8B 05 ?? ?? ?? ?? 8B 78 10 85 FF");
			return GenericPool(handle.At(2).Into().Value<UINT64>());
		}
		else
		{
			auto handle       = Memory::FindPattern("0F B6 05 ?? ?? ?? ?? A8 01 0F 84 94 02 00 00");
			auto encryptedPtr = ObjectPoolEncryptedPointer(handle.At(2).Into().Addr());
			return encryptedPtr.GetPool();
		}
	}();

	return propPool;
}

inline auto GetAllPedsArray()
{
	return GetAllPeds().ToArray();
}

inline auto GetAllVehsArray()
{
	return GetAllVehs().ToArray();
}

inline auto GetAllPropsArray()
{
	return GetAllProps().ToArray();
}