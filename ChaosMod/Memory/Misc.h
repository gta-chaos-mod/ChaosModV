#pragma once

namespace Memory
{
	inline void SetSkyDisabled(bool state)
	{
		static BYTE* patchByte = nullptr;

		if (!patchByte)
		{
			Handle handle = FindPattern("E8 ? ? ? ? C6 05 ? ? ? ? ? 48 83 C4 58");
			if (!handle.IsValid())
			{
				return;
			}

			patchByte = handle.Into().Get<BYTE>();
		}

		WriteByte(patchByte, state ? 0xC3 : 0x48);
	}

	inline void SetHealthArmorBarHidden(bool state)
	{
		static BYTE* patchByte = nullptr;

		if (!patchByte)
		{
			Handle handle = FindPattern("40 53 48 83 EC 20 48 83 0D");
			if (!handle.IsValid())
			{
				return;
			}

			patchByte = handle.At(0x93).Into().Get<BYTE>();
		}

		WriteByte(patchByte, state ? 0xC3 : 0x48);
	}
}