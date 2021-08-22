#include <stdafx.h>
#include "VehicleHooks.h"

GlobalTable globalTable;
ScriptHeader* shopController;
ScriptTable* scriptTable;

// from EnableMPCars by drp4lyf
bool FindShopController()
{
    __int64 patternAddr = Memory::FindPattern("\x4C\x8D\x05\x00\x00\x00\x00\x4D\x8B\x08\x4D\x85\xC9\x74\x11", "xxx????xxxxxxxx");
    if (!patternAddr) {
        LOG("ERROR: finding address 0");
        return false;
    }
    globalTable.GlobalBasePtr = (__int64**)(patternAddr + *(int*)(patternAddr + 3) + 7);

    patternAddr = Memory::FindPattern("\x48\x03\x15\x00\x00\x00\x00\x4C\x23\xC2\x49\x8B\x08", "xxx????xxxxxx");
    if (!patternAddr) {
        LOG("ERROR: finding address 1");
        return false;
    }
    scriptTable = (ScriptTable*)(patternAddr + *(int*)(patternAddr + 3) + 7);

    DWORD startTime = GetTickCount();
    DWORD timeout = 10000; // in millis

    // FindScriptAddresses
    while (!globalTable.IsInitialised()) {
        scriptWait(100); // Wait for GlobalInitialisation before continuing
        if (GetTickCount() > startTime + timeout) {
            LOG("ERROR: couldn't init global table");
            return false;
        }
    }

    ScriptTableItem* Item = scriptTable->FindScript(0x39DA738B);
    if (Item == NULL) {
        LOG("ERROR: finding address 2");
        return false;
    }
    while (!Item->IsLoaded())
        Sleep(100);

    shopController = Item->Header;
    LOG("Found ShopController");
    return true;
}

void EnableCarsGlobal() {
    const char* pattern = "\x2D\x00\x00\x00\x00\x2C\x01\x00\x00\x56\x04\x00\x6E\x2E\x00\x01\x5F\x00\x00\x00\x00\x04\x00\x6E\x2E\x00\x01";
    const char* mask = "x??xxxx??xxxxx?xx????xxxx?x";
    const unsigned int offset = 17;

    for (int i = 0; i < shopController->CodePageCount(); i++)
    {
        int size = shopController->GetCodePageSize(i);
        if (size)
        {
            uintptr_t address = Memory::FindPattern(pattern, mask, (const char*)shopController->GetCodePageAddress(i), size);
            if (address)
            {
                int globalindex = *(int*)(address + offset) & 0xFFFFFF;
                LOG("Setting Global Variable " + std::to_string(globalindex) + " to true");
                *globalTable.AddressOf(globalindex) = 1;
                LOG("Online Cars enabled");
                return;
            }
        }
    }
}