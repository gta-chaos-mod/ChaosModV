#pragma once

#include <cstdint>

class Handle
{
  private:
	std::uintptr_t m_Addr = 0;

  public:
	Handle() = default;
	Handle(uintptr_t addr) : m_Addr(addr)
	{
	}

	inline bool IsValid() const
	{
		return m_Addr;
	}

	inline Handle At(int offset) const
	{
		return IsValid() ? m_Addr + offset : 0;
	}

	template <typename T> inline T *Get() const
	{
		return reinterpret_cast<T *>(m_Addr);
	}

	template <typename T> inline T Value() const
	{
		return IsValid() ? *Get<T>() : 0;
	}

	inline uintptr_t Addr() const
	{
		return m_Addr;
	}

	inline Handle Into() const
	{
		if (IsValid())
		{
			auto handle = At(1);
			return handle.At(handle.Value<std::uint32_t>()).At(4);
		}

		return 0;
	}
};