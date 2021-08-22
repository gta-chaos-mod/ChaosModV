#include <stdafx.h>

struct GlobalTable {
    __int64** GlobalBasePtr;
    __int64* AddressOf(int index) const { return &GlobalBasePtr[index >> 18 & 0x3F][index & 0x3FFFF]; }
    bool IsInitialised()const { return *GlobalBasePtr != NULL; }
};

struct ScriptHeader {
    char padding1[16];                // 0x0
    unsigned char** codeBlocksOffset; // 0x10
    char padding2[4];                 // 0x18
    int codeLength;                   // 0x1C
    char padding3[4];                 // 0x20
    int localCount;                   // 0x24
    char padding4[4];                 // 0x28
    int nativeCount;                  // 0x2C
    __int64* localOffset;             // 0x30
    char padding5[8];                 // 0x38
    __int64* nativeOffset;            // 0x40
    char padding6[16];                // 0x48
    int nameHash;                     // 0x58
    char padding7[4];                 // 0x5C
    char* name;                       // 0x60
    char** stringsOffset;             // 0x68
    int stringSize;                   // 0x70
    char padding8[12];                // 0x74

    bool IsValid() const { return codeLength > 0; }
    int CodePageCount() const { return (codeLength + 0x3FFF) >> 14; }
    int GetCodePageSize(int page) const {
        return (page < 0 || page >= CodePageCount() ? 0 : (page == CodePageCount() - 1) ? codeLength & 0x3FFF : 0x4000);
    }
    unsigned char* GetCodePageAddress(int page) const { return codeBlocksOffset[page]; }
    unsigned char* GetCodePositionAddress(int codePosition) const {
        return codePosition < 0 || codePosition >= codeLength ? NULL : &codeBlocksOffset[codePosition >> 14][codePosition & 0x3FFF];
    }
    char* GetString(int stringPosition)const {
        return stringPosition < 0 || stringPosition >= stringSize ? NULL : &stringsOffset[stringPosition >> 14][stringPosition & 0x3FFF];
    }
};

struct ScriptTableItem {
    ScriptHeader* Header;
    char padding[4];
    int hash;

    inline bool IsLoaded() const {
        return Header != NULL;
    }
};

struct ScriptTable {
    ScriptTableItem* TablePtr;
    char padding[16];
    int count;
    ScriptTableItem* FindScript(int hash) {
        if (TablePtr == NULL) {
            return NULL; // Table initialisation hasnt happened yet
        }
        for (int i = 0; i < count; i++) {
            if (TablePtr[i].hash == hash) {
                return &TablePtr[i];
            }
        }
        return NULL;
    }
};

bool FindShopController();
void EnableCarsGlobal();