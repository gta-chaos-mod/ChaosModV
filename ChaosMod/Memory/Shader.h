#pragma once

#include "Memory/Handle.h"
#include "Memory/Memory.h"

#include "Util/Hash.h"
#include "Util/Logging.h"

#include <vector>

using Hash = unsigned long;

namespace Memory
{
	inline void InvalidateShaderCache()
	{
		static auto rage_sgaShader_ResolveShader = []() -> void *(*)(Hash ulHash)
		{
			auto handle = FindPattern("E8 ? ? ? ? 8B C8 E8 ? ? ? ? ? 85 C0 75 ? 81 3D ? ? ? ? 00 02 00 00");
			if (!handle.IsValid())
			{
				LOG("rage::sgaShader::ResolveShader not found");
				return nullptr;
			}

			return handle.At(7).Into().Get<void *(Hash)>();
		}();

		static auto rage_sgaShader_destructor = []() -> void (*)(void *_this)
		{
			auto handle = FindPattern("8B CB 84 C0 74 ? E8 ? ? ? ? EB ? E8 ? ? ? ? ? 8B CB E8");
			if (!handle.IsValid())
			{
				LOG("rage::sgaShader::~sgaShader not found");
				return nullptr;
			}

			return handle.At(13).Into().Get<void(void *)>();
		}();

		static auto reloadShaders = []() -> void (*)()
		{
			auto handle = FindPattern("89 ? ? ? ? ? E8 ? ? ? ? ? 8B ? E8 ? ? ? ? ? 84 FF 74 ? ? 8B ? E8");
			if (!handle.IsValid())
			{
				LOG("_reloadShaders not found!");
				return nullptr;
			}

			return handle.At(27).Into().Get<void()>();
		}();

		if (reloadShaders && rage_sgaShader_ResolveShader && rage_sgaShader_destructor)
		{
			static const std::vector<Hash> ugToDelete = { "postfx"_hash,
				                                          "postfxms"_hash,
				                                          "postfxms0"_hash,
				                                          "deferred_lighting"_hash,
				                                          "deferred_lightingms"_hash,
				                                          "deferred_lightingms0"_hash };

			for (auto hash : ugToDelete)
			{
				auto shader = rage_sgaShader_ResolveShader(hash);
				if (shader)
				{
					rage_sgaShader_destructor(shader);
				}
			}

			reloadShaders();
		}
	}
}