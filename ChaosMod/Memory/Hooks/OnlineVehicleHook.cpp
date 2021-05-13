/*
 * Original Code Base:
 * https://github.com/E66666666/GTAVAddonLoader
 * Adapted to ChaosMod by MoneyWasted
 */

#include <stdafx.h>

typedef CVehicleModelInfo* (*GetModelInfo_t)(unsigned int modelHash, int* index);
typedef CVehicleModelInfo* (*InitVehicleArchetype_t)(const char*, bool, unsigned int);

GetModelInfo_t GetModelInfo;

GlobalTable globalTable;
ScriptTable* scriptTable;
ScriptHeader* shopController;

CallHook<InitVehicleArchetype_t>* g_InitVehicleArchetype = nullptr;

extern std::unordered_map<Hash, std::string> g_vehicleHashes;

int gameVersion = getGameVersion();

CVehicleModelInfo* initVehicleArchetype_stub(const char* name, bool a2, unsigned int a3) {
    g_vehicleHashes.insert({ joaat(name), name });
    return g_InitVehicleArchetype->fn(name, a2, a3);
}

void setupHooks() {
    auto addr = MemoryAccess::FindPattern("\xE8\x00\x00\x00\x00\x48\x8B\x4D\xE0\x48\x8B\x11", "x????xxxxxxx");
    if (!addr) {
        LOG(ERROR, "Couldn't find InitVehicleArchetype");
        return;
    }
    LOG("Found InitVehicleArchetype at 0x%p", addr);
    g_InitVehicleArchetype = HookManager::SetCall(addr, initVehicleArchetype_stub);
}


void removeHooks() {
    if (g_InitVehicleArchetype) {
        delete g_InitVehicleArchetype;
        g_InitVehicleArchetype = nullptr;
    }
}

void MemoryAccess::Init() {
    uintptr_t addr;

    if (gameVersion <= 57) {
        addr = FindPattern(
            "\x0F\xB7\x05\x00\x00\x00\x00"
            "\x45\x33\xC9\x4C\x8B\xDA\x66\x85\xC0"
            "\x0F\x84\x00\x00\x00\x00"
            "\x44\x0F\xB7\xC0\x33\xD2\x8B\xC1\x41\xF7\xF0\x48"
            "\x8B\x05\x00\x00\x00\x00"
            "\x4C\x8B\x14\xD0\xEB\x09\x41\x3B\x0A\x74\x54",
            "xxx????"
            "xxxxxxxxx"
            "xx????"
            "xxxxxxxxxxxx"
            "xx????"
            "xxxxxxxxxxx");

        if (!addr) {
            LOG(ERROR, "Couldn't find GetModelInfo");
        }
    }
    else {
        addr = FindPattern("\xEB\x09\x41\x3B\x0A\x74\x54", "xxxxxxx");
        if (!addr) {
            LOG(ERROR, "Couldn't find GetModelInfo (v58+)");
        }
        addr = addr - 0x2C;
    }

    GetModelInfo = (GetModelInfo_t)(addr);

    // find enable MP cars patterns
    if (findShopController())
        enableCarsGlobal();

}

// Thank you, Unknown Modder!
template < typename ModelInfo >
std::vector<uint16_t> GetVehicleModKits_t(int modelHash) {
    std::vector<uint16_t> modKits;
    int index = 0xFFFF;
    auto* modelInfo = reinterpret_cast<ModelInfo*>(GetModelInfo(modelHash, &index));
    if (modelInfo && modelInfo->GetModelType() == 5) {
        uint16_t count = modelInfo->m_modKitsCount;
        for (uint16_t i = 0; i < count; i++) {
            uint16_t modKit = modelInfo->m_modKits[i];
            modKits.push_back(modKit);
        }
    }
    return modKits;
}

std::vector<uint16_t> MemoryAccess::GetVehicleModKits(int modelHash) {
    if (gameVersion < 38) {
        return GetVehicleModKits_t<CVehicleModelInfo>(modelHash);
    }
    else {
        return GetVehicleModKits_t<CVehicleModelInfo1290>(modelHash);
    }
}

char* MemoryAccess::GetVehicleGameName(int modelHash) {
    int index = 0xFFFF;
    void* modelInfo = GetModelInfo(modelHash, &index);
    if (gameVersion < 38) {
        return ((CVehicleModelInfo*)modelInfo)->m_displayName;
    }
    else {
        return ((CVehicleModelInfo1290*)modelInfo)->m_displayName;
    }
}
char* MemoryAccess::GetVehicleMakeName(int modelHash) {
    int index = 0xFFFF;
    void* modelInfo = GetModelInfo(modelHash, &index);
    if (gameVersion < 38) {
        return ((CVehicleModelInfo*)modelInfo)->m_manufacturerName;
    }
    else {
        return ((CVehicleModelInfo1290*)modelInfo)->m_manufacturerName;
    }
}

uintptr_t MemoryAccess::FindPattern(const char* pattern, const char* mask, const char* startAddress, size_t size) {
    const char* address_end = startAddress + size;
    const auto mask_length = static_cast<size_t>(strlen(mask) - 1);

    for (size_t i = 0; startAddress < address_end; startAddress++) {
        if (*startAddress == pattern[i] || mask[i] == '?') {
            if (mask[i + 1] == '\0') {
                return reinterpret_cast<uintptr_t>(startAddress) - mask_length;
            }
            i++;
        }
        else {
            i = 0;
        }
    }
    return 0;
}

uintptr_t MemoryAccess::FindPattern(const char* pattern, const char* mask) {
    MODULEINFO modInfo = { };
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &modInfo, sizeof(MODULEINFO));

    return FindPattern(pattern, mask, reinterpret_cast<const char*>(modInfo.lpBaseOfDll), modInfo.SizeOfImage);
}

// from EnableMPCars by drp4lyf
bool MemoryAccess::findShopController() {
    // FindPatterns
    __int64 patternAddr = FindPattern("\x4C\x8D\x05\x00\x00\x00\x00\x4D\x8B\x08\x4D\x85\xC9\x74\x11", "xxx????xxxxxxxx");
    if (!patternAddr) {
        LOG(ERROR, "ERROR: finding address 0");
        LOG(ERROR, "Aborting...");
        return false;
    }
    globalTable.GlobalBasePtr = (__int64**)(patternAddr + *(int*)(patternAddr + 3) + 7);


    patternAddr = FindPattern("\x48\x03\x15\x00\x00\x00\x00\x4C\x23\xC2\x49\x8B\x08", "xxx????xxxxxx");
    if (!patternAddr) {
        LOG(ERROR, "ERROR: finding address 1");
        LOG(ERROR, "Aborting...");
        return false;
    }
    scriptTable = (ScriptTable*)(patternAddr + *(int*)(patternAddr + 3) + 7);

    DWORD startTime = GetTickCount();
    DWORD timeout = 10000; // in millis

    // FindScriptAddresses
    while (!globalTable.IsInitialised()) {
        scriptWait(100); //Wait for GlobalInitialisation before continuing
        if (GetTickCount() > startTime + timeout) {
            LOG(ERROR, "ERROR: couldn't init global table");
            LOG(ERROR, "Aborting...");
            return false;
        }
    }

    //logger.Write(INFO, "Found global base pointer " + std::to_string((__int64)globalTable.GlobalBasePtr));

    ScriptTableItem* Item = scriptTable->FindScript(0x39DA738B);
    if (Item == NULL) {
        LOG(ERROR, "ERROR: finding address 2");
        LOG(ERROR, "Aborting...");
        return false;
    }
    while (!Item->IsLoaded())
        Sleep(100);

    shopController = Item->Header;
    //logger.Write(INFO, "Found shopcontroller");
    return true;
}

void MemoryAccess::enableCarsGlobal() {
    const char* patt617_1 = "\x2C\x01\x00\x00\x20\x56\x04\x00\x6E\x2E\x00\x01\x5F\x00\x00\x00\x00\x04\x00\x6E\x2E\x00\x01";
    const char* mask617_1 = "xx??xxxxxx?xx????xxxx?x";
    const unsigned int offset617_1 = 13;

    const char* patt1604_0 = "\x2D\x00\x00\x00\x00\x2C\x01\x00\x00\x56\x04\x00\x6E\x2E\x00\x01\x5F\x00\x00\x00\x00\x04\x00\x6E\x2E\x00\x01";
    const char* mask1604_0 = "x??xxxx??xxxxx?xx????xxxx?x";
    const unsigned int offset1064_0 = 17;

    const char* pattern = patt617_1;
    const char* mask = mask617_1;
    int offset = offset617_1;

    if (getGameVersion() >= 46) {
        pattern = patt1604_0;
        mask = mask1604_0;
        offset = offset1064_0;
    }

    for (int i = 0; i < shopController->CodePageCount(); i++)
    {
        int size = shopController->GetCodePageSize(i);
        if (size)
        {
            uintptr_t address = FindPattern(pattern, mask, (const char*)shopController->GetCodePageAddress(i), size);
            if (address)
            {
                int globalindex = *(int*)(address + offset) & 0xFFFFFF;
                LOG("Setting Global Variable " + std::to_string(globalindex) + " to true");
                *globalTable.AddressOf(globalindex) = 1;
                LOG("MP Cars enabled");
                return;
            }
        }
    }

    LOG(ERROR, "Global Variable not found, check game version >= 1.0.678.1");
}