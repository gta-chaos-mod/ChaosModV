#include <stdafx.h>

#include "LabelHook.h"

KHASH_MAP_INIT_INT(text_map, std::uint32_t)
KHASH_MAP_INIT_STR(text_maps, kh_text_map_t*)

struct KHashDeleter
{
	void operator()(kh_text_maps_t* h) const
	{
		for (auto k = kh_begin(h); k != kh_end(h); k++)
		{
			if (kh_exist(h, k))
			{
				kh_destroy(text_map, kh_value(h, k));
			}
		}

		kh_destroy(text_maps, h);
	}
};

static std::unique_ptr<kh_text_maps_t, KHashDeleter> ms_pTextMaps = nullptr;
static kh_text_map_t* ms_pCurTextMap = nullptr;
static std::vector<std::string> ms_textStorage;

static bool ms_bEnableHook = false;

static const char* (*_OG_GetLabelText)(void* text, uint32_t hash);
static const char* _HK_GetLabelText(void* text, uint32_t hash)
{
	if (ms_bEnableHook && ms_pCurTextMap)
	{
		auto k = kh_get(text_map, ms_pCurTextMap, hash);
		if (k != kh_end(ms_pCurTextMap))
		{
			return ms_textStorage[kh_value(ms_pCurTextMap, k)].c_str();
		}
	}

	return _OG_GetLabelText(text, hash); 
}

static bool OnHook()
{
	// Thanks to FiveM for these patterns!

	Handle handle;

	handle = Memory::FindPattern("48 8B CB 8B D0 E8 ? ? ? ? 48 85 C0 0F 95 C0");
	if (!handle.IsValid())
	{
		return false;
	}

	handle = handle.At(5).Into();
	Memory::AddHook(handle.Get<void>(), _HK_GetLabelText, &_OG_GetLabelText);

	handle = Memory::FindPattern("48 85 C0 75 34 8B 0D");
	if (handle.IsValid())
	{
		handle = handle.At(-5).Into();
		Memory::AddHook(handle.Get<void>(), _HK_GetLabelText, nullptr);
	}

	ms_pTextMaps = std::unique_ptr<kh_text_maps_t, KHashDeleter>(kh_init(text_maps));

	return true;
}

static RegisterHook registerHook(OnHook, "_GetLabelText");

namespace Hooks
{
	void EnableLabelHook(const char* szLabelFile)
	{
		auto k = kh_get(text_maps, ms_pTextMaps.get(), szLabelFile);
		if (k != kh_end(ms_pTextMaps.get()))
		{
			ms_pCurTextMap = kh_value(ms_pTextMaps.get(), k);
			ms_bEnableHook = true;

			return;
		}

		if (!DoesFileExist(szLabelFile))
		{
			return;
		}

		FILE* file;
		if (fopen_s(&file, szLabelFile, "r"))
		{
			return;
		}

		fseek(file, 0, SEEK_END);
		auto fileSize = ftell(file);
		rewind(file);

		std::string buffer;
		buffer.resize(fileSize);
		fread(&buffer[0], sizeof(char), fileSize, file);
		fclose(file);

		auto entries = std::count(buffer.begin(), buffer.end(), '\n');
		ms_textStorage.reserve(ms_textStorage.capacity() + entries);

		auto h = kh_init(text_map);

		int i = ms_textStorage.size();
		std::string_view str = buffer;
		while (str.size() > 9)
		{
			std::string hashStr(str.substr(0, 8));
			auto endlineIndex = str.find('\n');
			std::string label(str.substr(9, endlineIndex - 9));

			int ret;
			auto k = kh_put(text_map, h, std::strtoul(hashStr.c_str(), NULL, 16), &ret);
			ms_textStorage.push_back(label);
			kh_value(h, k) = i++;

			str = str.substr(endlineIndex + 1);
		}

		int ret;
		k = kh_put(text_maps, ms_pTextMaps.get(), szLabelFile, &ret);
		kh_value(ms_pTextMaps.get(), k) = h;

		ms_pCurTextMap = h;
		ms_bEnableHook = true;
	}

	void DisableLabelHook()
	{
		ms_bEnableHook = false;
		ms_pCurTextMap = nullptr;
	}
}