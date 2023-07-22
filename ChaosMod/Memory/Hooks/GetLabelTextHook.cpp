#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

static std::unordered_map<Hash, std::string> ms_CustomLabels;

const char *(*OG_GetLabelText)(void *, Hash);
const char *HK_GetLabelText(void *text, Hash hash)
{
	const auto &result = ms_CustomLabels.find(hash);
	if (result != ms_CustomLabels.end())
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
	void AddCustomLabel(std::string_view label, const std::string &text)
	{
		ms_CustomLabels[GET_HASH_KEY(label.data())] = text;
	}

	void ClearCustomLabels()
	{
		ms_CustomLabels.clear();
	}
}
