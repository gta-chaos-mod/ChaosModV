#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

#include <queue>

#define MAX_CUSTOM_LABELS 1000

static std::unordered_map<Hash, std::string> ms_CustomLabels;
static std::queue<Hash> ms_CustomLabelHashes;

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

static RegisterHook registerHook(OnHook, nullptr, "GetLabelText", true);

namespace Hooks
{
	void AddCustomLabel(std::string_view label, const std::string &text)
	{
		auto hash = GET_HASH_KEY(label.data());

		if (ms_CustomLabels.contains(hash))
		{
			return;
		}

		ms_CustomLabels[hash] = text;
		ms_CustomLabelHashes.push(hash);

		if (ms_CustomLabelHashes.size() > MAX_CUSTOM_LABELS)
		{
			auto eraseHash = ms_CustomLabelHashes.front();
			ms_CustomLabels.erase(eraseHash);
			ms_CustomLabelHashes.pop();
		}
	}

	void ClearCustomLabels()
	{
		ms_CustomLabels.clear();
		ms_CustomLabelHashes = std::queue<Hash>();
	}
}
