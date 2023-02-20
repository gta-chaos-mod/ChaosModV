#pragma once

#include "Handle.h"
#include "Memory.h"
#include "Util/Logging.h"
#include "Util/Natives.h"

#include <scripthookv/inc/main.h>

namespace Memory
{
	typedef uintptr_t(__cdecl PlayAmbientSpeech)(uintptr_t a1, unsigned int speechNameHash, const char *speechParam,
	                                              unsigned int voiceNameHash, int a5, uintptr_t a6, int a7, int a8,
	                                              float a9, char a10, unsigned int *speechIndexPtr, float *position);

	inline void PlayAmbientSpeechAtCoordsWithIndex(const std::string &speechName, const std::string &voiceName,
	                                               Vector3 coords, unsigned int speechIndex, const std::string &speechParam)
	{
		static PlayAmbientSpeech* playAmbientSpeechFunc;
		static unsigned int(__cdecl * getHashKeyFunc)(const char *string, unsigned int init);

		static Handle handle = []() -> Handle
		{
			Handle initHandle = Memory::FindPattern("48 89 5C 24 08 57 48 83 EC 40 F3 41 0F 10 00");

			if (!initHandle.IsValid())
			{
				return Handle();
			}

			Handle handle2 = initHandle.At(0x63).Into();
			if (!handle2.IsValid())
			{
				return Handle();
			}
			getHashKeyFunc        = handle2.At(0x20).Into().Get<unsigned int __cdecl(const char *, unsigned int)>();
			playAmbientSpeechFunc = handle2.At(0x6E).Into().Get<PlayAmbientSpeech>();

			return initHandle;
		}();

		if (!handle.IsValid())
		{
			return;
		}

		uintptr_t globalPtr = handle.At(0x53).Into().Addr();

		float pos[3]        = { coords.x, coords.y, coords.z };

		playAmbientSpeechFunc(globalPtr + 0x4410, getHashKeyFunc(speechName.c_str(), 0), speechParam.c_str(),
		                      GET_HASH_KEY(voiceName.c_str()), -1, 0, 0, -1, 1.0, 1,
		                      &speechIndex, pos);
	}
}