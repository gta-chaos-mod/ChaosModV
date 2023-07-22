#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

static std::unordered_map<Hash, std::string_view> ms_ChaosLabels;

const char *(*OG_GetLabelText)(void *, Hash);
const char *HK_GetLabelText(void *text, Hash hash)
{
	const auto &result = ms_ChaosLabels.find(hash);
	if (result != ms_ChaosLabels.end())
	{
		return result->second.data();
	}

	return OG_GetLabelText(text, hash);
}

static bool OnHook()
{
	auto handle = Memory::FindPattern("48 8B CB 8B D0 E8 ? ? ? ? 48 85 C0 0F 95 C0");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.At(5).Into().Get<void>(), HK_GetLabelText, &OG_GetLabelText);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "GetLabelText");

namespace Hooks
{
	void AddLabel(std::string_view label, std::string_view text)
	{
		ms_ChaosLabels[GET_HASH_KEY(label.data())] = text;
	}
}
