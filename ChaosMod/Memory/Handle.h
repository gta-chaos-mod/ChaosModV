#pragma once

#include "../../vendor/minhook/include/MinHook.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

class Handle
{
public:
	Handle() : m_addr(0) {}
	Handle(DWORD64 addr) : m_addr(addr) {}

	inline bool IsValid() const
	{
		return m_addr;
	}

	inline Handle At(int offset) const
	{
		if (IsValid())
		{
			return m_addr + offset;
		}
	}

	template<typename T>
	inline T* Get() const
	{
		return reinterpret_cast<T*>(m_addr);
	}

	template<typename T>
	inline T Value() const
	{
		return IsValid() ? *Get<T>() : 0;
	}

	inline DWORD64 Addr() const
	{
		return m_addr;
	}

	inline Handle Into() const
	{
		return m_addr + *reinterpret_cast<DWORD*>(m_addr) + 4;
	}

private:
	DWORD64 m_addr;
};