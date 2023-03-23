#pragma once

using DWORD64 = unsigned long long;
using DWORD   = unsigned long;

class Handle
{
  private:
	DWORD64 m_ullAddr;

  public:
	Handle() : m_ullAddr(0)
	{
	}
	Handle(DWORD64 ullAddr) : m_ullAddr(ullAddr)
	{
	}

	inline bool IsValid() const
	{
		return m_ullAddr;
	}

	inline Handle At(int iOffset) const
	{
		return IsValid() ? m_ullAddr + iOffset : 0;
	}

	template <typename T> inline T *Get() const
	{
		return reinterpret_cast<T *>(m_ullAddr);
	}

	template <typename T> inline T Value() const
	{
		return IsValid() ? *Get<T>() : 0;
	}

	inline DWORD64 Addr() const
	{
		return m_ullAddr;
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
};