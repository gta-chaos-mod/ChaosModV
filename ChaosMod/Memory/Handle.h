#pragma once

typedef unsigned long long DWORD64;
typedef unsigned long DWORD;

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
		return IsValid() ? m_addr + offset : 0;
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
		if (IsValid())
		{
			Handle handle = At(1);
			return handle.At(handle.Value<DWORD>()).At(4);
		}

		return 0;
	}

private:
	DWORD64 m_addr;
};