#pragma once

#include "Handle.h"
#include "Memory.h"
#include "Util/Logging.h"
#include "Util/Natives.h"

#include <scripthookv/inc/main.h>

namespace Memory
{
	typedef uintptr_t(__cdecl PlaySpeech)(uintptr_t a1, unsigned int speechNameHash, const char *speechParam,
	                                      unsigned int voiceNameHash, int a5, uintptr_t a6, int a7, int a8, float a9,
	                                      char a10, unsigned int *speechIndexPtr, __m128 *position);

	inline void PlayAmbientSpeechAtCoordsWithIndex(const std::string &speechName, const std::string &voiceName,
	                                               Vector3 coords, unsigned int speechIndex,
	                                               const std::string &speechParam)
	{
		static PlaySpeech *playSpeechFunc;
		static uintptr_t globalPtr;

		static Handle handle = []() -> Handle
		{
			if (IsLegacy())
			{
				Handle handle = Memory::FindPattern("48 89 5C 24 08 57 48 83 EC 40 F3 41 0F 10 00");

				if (!handle.IsValid())
					return Handle();

				Handle handle2 = handle.At(0x63).Into();
				if (!handle2.IsValid())
					return false;
				playSpeechFunc = handle2.At(0x6E).Into().Get<PlaySpeech>();

				return handle;
			}
			else
			{
				Handle handle = Memory::FindPattern("E8 ?? ?? ?? ?? E9 17 01 00 00 45 84 F6");

				if (!handle.IsValid())
					return Handle();

				playSpeechFunc = handle.Into().Get<PlaySpeech>();

				return Memory::FindPattern("48 8D 0D ?? ?? ?? ?? 48 89 DA 44 89 D6");
			}
		}();

		if (!handle.IsValid())
			return;

		globalPtr    = handle.At(IsLegacy() ? 0x53 : 2).Into().Addr();

		__m128 pos;
		pos.m128_f32[0] = coords.x;
		pos.m128_f32[1] = coords.y;
		pos.m128_f32[2] = coords.z;

		playSpeechFunc(globalPtr + 0x4410, GET_HASH_KEY(speechName.c_str()), speechParam.c_str(),
		               GET_HASH_KEY(voiceName.c_str()), -1, 0, 0, -1, 1.0, 1, &speechIndex, &pos);
	}
}