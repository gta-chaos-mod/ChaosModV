#pragma once

#include <Windows.h>
#include <scripthookv/inc/main.h>

#include "Util/Logging.h"

enum class GameVersion
{
	GTA5_LEGACY,
	GTA5_ENHANCED
};

#pragma pack(push, 1)
struct RSDSHeader
{
	DWORD Signature;
	GUID Guid;
	DWORD Age;
	char PdbFileName[1];
};
#pragma pack(pop)

inline std::string GetPdbPath()
{
	HMODULE hModule = GetModuleHandle(nullptr);
	if (!hModule)
		throw std::runtime_error("Failed to get main module handle.");

	const BYTE *base            = reinterpret_cast<BYTE *>(hModule);
	const IMAGE_DOS_HEADER *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(base);
	if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		throw std::runtime_error("Invalid DOS signature");

	const IMAGE_NT_HEADERS *nt = reinterpret_cast<const IMAGE_NT_HEADERS *>(base + dos->e_lfanew);
	if (nt->Signature != IMAGE_NT_SIGNATURE)
		throw std::runtime_error("Invalid PE signature.");

	const IMAGE_DATA_DIRECTORY &debugData = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	if (debugData.VirtualAddress == 0)
		throw std::runtime_error("No debug directory found.");

	const IMAGE_DEBUG_DIRECTORY *debugDir =
	    reinterpret_cast<const IMAGE_DEBUG_DIRECTORY *>(base + debugData.VirtualAddress);
	if (debugDir->Type != IMAGE_DEBUG_TYPE_CODEVIEW)
		throw std::runtime_error("Not a CodeView debug directory.");

	const BYTE *debugInfo  = base + debugDir->AddressOfRawData;
	const RSDSHeader *rsds = reinterpret_cast<const RSDSHeader *>(debugInfo);
	if (rsds->Signature != 'SDSR')
		throw std::runtime_error("Invalid RSDS signature.");

	return std::string(rsds->PdbFileName);
}

inline bool CheckPdbPathForVersion(const std::string &pdbPath)
{
	if (pdbPath.find("dev_gen9") != std::string::npos)
	{
		LOG("Enhanced Version Detected with PDB Path: " << pdbPath);
		return true;
	}
	else if (pdbPath.find("dev_ng") != std::string::npos)
	{
		LOG("Legacy Version Detected with PDB Path: " << pdbPath);
		return false;
	}

	throw std::runtime_error(std::string("Unknown PDB Path: " + pdbPath));
}

inline bool DetectByFilename()
{
	WCHAR moduleName[MAX_PATH];
	GetModuleFileName(nullptr, moduleName, MAX_PATH);
	std::wstring ws(moduleName);

	if (ws.ends_with(L"_Enhanced.exe"))
	{
		LOG("Enhanced Version Detected With Filename Check");
		return true;
	}

	LOG("Legacy Version Detected");
	return false;
}

inline GameVersion GetGame()
{
	static const GameVersion cachedVersion = []() -> GameVersion
	{
		try
		{
			const std::string pdbPath = GetPdbPath();
			bool isEnhanced           = CheckPdbPathForVersion(pdbPath);
			return isEnhanced ? GameVersion::GTA5_ENHANCED : GameVersion::GTA5_LEGACY;
		}
		catch (const std::runtime_error &e)
		{
			LOG("Failed to extract PDB path! Falling back to Filename Check: " << e.what());
			bool isEnhanced = DetectByFilename();
			return isEnhanced ? GameVersion::GTA5_ENHANCED : GameVersion::GTA5_LEGACY;
		}
	}();

	return cachedVersion;
}

inline bool IsEnhanced()
{
	return GetGame() == GameVersion::GTA5_ENHANCED;
}

inline bool IsLegacy()
{
	return GetGame() == GameVersion::GTA5_LEGACY;
}