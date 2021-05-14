#pragma once

#include <stdafx.h>

#define STATIC_ASSERT_SIZE(Type, Size) static_assert(sizeof(Type) == Size, "invalid " #Type " size")

typedef uint8_t eModelType;
typedef uint32_t eVehicleType;
typedef uint32_t eVehicleFlag1;
typedef uint32_t eVehicleFlag2;
typedef uint32_t eVehicleFlag3;
typedef uint32_t eVehicleFlag4;
typedef uint32_t eVehicleFlag5;
typedef uint32_t eVehicleFlag6;

// Various snippets from FiveM source and Unknown Modder
#pragma pack(push, 1)
namespace rage {
    class fwArchetype {
    public:
        virtual ~fwArchetype() = default;

        char _0x0008[0x10]; // 0x0000
        Hash m_hash; // 0x0018
        char _0x001C[0x10]; // 0x001C
        float m_radius; // 0x002C
        float m_aabbMin[4]; // 0x0030
        float m_aabbMax[4]; // 0x0040
        uint32_t m_flags; // 0x0050
        char _0x0054[0x12]; // 0x0054
        uint16_t m_index; // 0x0066
    };

    class fwEntity
    {
    public:
        virtual ~fwEntity() = 0;

        virtual bool IsOfType(uint32_t hash) = 0;

        template<typename T>
        bool IsOfType()
        {
            return reinterpret_cast<T*>(this->IsOfType(HashString(boost::typeindex::type_id<T>().pretty_name().substr(6).c_str())));
        }
    };

    class fwArchetypeDef
    {
    public:
        virtual ~fwArchetypeDef();

        virtual int64_t GetTypeIdentifier();

        float lodDist;
        uint32_t flags; // 0x10000 = alphaclip
        uint32_t specialAttribute; // lower 5 bits == 31 -> use alpha clip, get masked to 31 in InitializeFromArchetypeDef
        uint32_t pad;
        void* pad2;
        float bbMin[4];
        float bbMax[4];
        float bsCentre[4];
        float bsRadius;
        float hdTextureDist;
        uint32_t name;
        uint32_t textureDictionary;
        uint32_t clipDictionary;
        uint32_t drawableDictionary;
        uint32_t physicsDictionary;
        uint32_t assetType;
        uint32_t assetName;
        uint32_t pad5[7];

    public:
        fwArchetypeDef()
        {
            flags = 0x10000; // was 0x2000
            lodDist = 299.0f;
            hdTextureDist = 375.0f;

            drawableDictionary = 0;
            assetType = 3;
            assetName = 0x12345678;

            specialAttribute = 31;

            pad = 0;
            pad2 = 0;
            clipDictionary = 0;
            physicsDictionary = 0;
            memset(pad5, 0, sizeof(physicsDictionary));
        }
    };

}

class CBaseModelInfo : public rage::fwArchetype
{
public:
    virtual ~CBaseModelInfo() {}
    virtual void Initialize() {}
    virtual void InitializeFromArchetypeDef(uint32_t, rage::fwArchetypeDef*, bool) {}
    virtual rage::fwEntity* CreateEntity() { return nullptr; }
    // and lots of other functions...

public:
    eModelType GetModelType() const
    {
        return m_modelType & 0x1F;
    }

protected:
    char _0x0068[0x35];    // 0x0068
    eModelType m_modelType;    // 0x009D (& 0x1F)
    char _0x009E[0x2];    // 0x009E
    uint32_t m_unkFlag;    // 0x00A0
    char _0x00A4[0x4];    // 0x00A4
    void* m_0x00A8;        // 0x00A8
};

STATIC_ASSERT_SIZE(CBaseModelInfo, 0xB0);

class CVehicleModelInfo : public CBaseModelInfo
{
public:
    virtual ~CVehicleModelInfo() {}
    virtual void Initialize() {}
    virtual void InitializeFromArchetypeDef(uint32_t, rage::fwArchetypeDef*, bool) {}
    virtual rage::fwEntity* CreateEntity() { return nullptr; }
    // and lots of other functions...

public:
    void* m_0x00B0; // 0x00B0
    char _0x00B8[0x40]; // 0x00B8
    uint8_t m_primaryColorCombinations[25]; // 0x00F8
    uint8_t m_secondaryColorCombinations[25]; // 0x0111
    uint8_t m_unkColor1Combinations[25]; // 0x012A
    uint8_t m_unkColor2Combinations[25]; // 0x0143
    uint8_t m_interiorColorCombinations[25]; // 0x015C
    uint8_t m_dashboardColorCombinations[25]; // 0x0175
    char _0x018E[0xE2]; // 0x018E
    char m_displayName[12]; // 0x0270 (aka gameName)
    char m_manufacturerName[12]; // 0x027C (aka makeName)
    uint8_t* m_modKits; // 0x0288
    uint16_t m_modKitsCount; // 0x0290
    char _0x0292[0x46]; // 0x0292
    void* m_driverInfo; // 0x02D8
    uint8_t m_numDrivers; // 0x02E0
    char _0x02E1[0x37]; // 0x02E1
    eVehicleType m_vehicleType; // 0x0318
    uint32_t m_unkVehicleType; // 0x031C
    uint32_t m_diffuseTint; // 0x0320
    char _0x0324[0x90]; // 0x0324
    uint8_t m_unkModKitVal; // 0x03B4
    char _0x03B5[0xA7]; // 0x03B5
    float m_wheelScale; // 0x045C
    float m_wheelScaleRear; // 0x0460
    float m_defaultBodyHealth; // 0x0464
    char _0x0468[0x20]; // 0x0468
    uint32_t m_handlingIndex; // 0x0488
    uint32_t m_identicalModelSpawnDistance; // 0x048C
    char _0x0490[0x4]; // 0x0490
    uint32_t m_numColorCombinations; // 0x0494
    char _0x0498[0x30]; // 0x0498
    void* m_0x04C8; // 0x04C8 (wheel data? 0xAC -> burnout mult?)
    char _0x04D0[0x3B]; // 0x04D0
    uint8_t m_sirenInfoId; // 0x050B
    char _0x050C[0xC]; // 0x050C
    uint8_t m_vehicleClass; // 0x0518 (& 0x1F; (>> 5) & 3 -> plate type)
    char _0x0519[0x2F]; // 0x0519
    int m_seatCount; // 0x0548
    eVehicleFlag1 m_flags1; // 0x054C
    eVehicleFlag2 m_flags2; // 0x0550
    eVehicleFlag3 m_flags3; // 0x0554
    eVehicleFlag4 m_flags4; // 0x0558
    eVehicleFlag5 m_flags5; // 0x055C
};

STATIC_ASSERT_SIZE(CVehicleModelInfo, 0x560);

class CVehicleModelInfo1290 : public CBaseModelInfo
{
public:
    virtual ~CVehicleModelInfo1290() {}
    virtual void Initialize() {}
    virtual void InitializeFromArchetypeDef(uint32_t, rage::fwArchetypeDef*, bool) {}
    virtual rage::fwEntity* CreateEntity() { return nullptr; }
    // and lots of other functions...

public:
    void* m_0x00B0; // 0x00B0
    char _0x00B8[0x40]; // 0x00B8
    uint8_t m_primaryColorCombinations[25]; // 0x00F8
    uint8_t m_secondaryColorCombinations[25]; // 0x0111
    uint8_t m_unkColor1Combinations[25]; // 0x012A
    uint8_t m_unkColor2Combinations[25]; // 0x0143
    uint8_t m_interiorColorCombinations[25]; // 0x015C
    uint8_t m_dashboardColorCombinations[25]; // 0x0175
    char _0x018E[0x10A]; // 0x018E
    char m_displayName[12]; // 0x0298 (aka gameName)
    char m_manufacturerName[12]; // 0x02A4 (aka makeName)
    uint16_t* m_modKits; // 0x02B0
    uint16_t m_modKitsCount; // 0x02B8
    char _0x02BA[0x46]; // 0x02BA
    void* m_driverInfo; // 0x0300
    uint8_t m_numDrivers; // 0x0308
    char _0x0309[0x37]; // 0x02E3
    eVehicleType m_vehicleType; // 0x0340
    uint32_t m_unkVehicleType; // 0x0344
    uint32_t m_diffuseTint; // 0x0348
    char _0x034C[0x90]; // 0x034C
    uint8_t m_unkModKitVal; // 0x03DC (also uint16_t now?)
    char _0x03DD[0xA7]; // 0x03DD
    float m_wheelScale; // 0x0484
    float m_wheelScaleRear; // 0x0488
    float m_defaultBodyHealth; // 0x048C
    char _0x0490[0x20]; // 0x0490
    uint32_t m_handlingIndex; // 0x04B8
    uint32_t m_identicalModelSpawnDistance; // 0x04BC
    char _0x04C0[0x4]; // 0x04C0
    uint32_t m_numColorCombinations; // 0x04C4
    uint32_t m_fragmentIndex; // 0x04C8
    char _0x04CC[0x2C]; // 0x04CC
    void* m_0x04F8; // 0x04F8 (wheel data? 0xAC -> burnout mult?)
    char _0x0500[0x3B]; // 0x0500
    uint8_t m_sirenInfoId; // 0x053B
    char _0x053C[0x7]; // 0x053C
    uint8_t m_0x0543; // 0x0543
    char _0x0544_[0x4]; // 0x0544
    uint8_t m_vehicleClass; // 0x0548 (& 0x1F; (>> 5) & 3 -> plate type)
    char _0x0549[0x2F]; // 0x0549
    int m_seatCount; // 0x0578 (use only if unk_0x00B0->seatCount can't be used)
    eVehicleFlag1 m_flags1; // 0x057C
    eVehicleFlag2 m_flags2; // 0x0580
    eVehicleFlag3 m_flags3; // 0x0584
    eVehicleFlag4 m_flags4; // 0x0588
    eVehicleFlag5 m_flags5; // 0x058C
    eVehicleFlag6 m_flags6; // 0x0590
    char _0x0594[0xC]; // 0x0594
};

// ?
STATIC_ASSERT_SIZE(CVehicleModelInfo1290, 0x598);

struct ScriptHeader {
    char padding1[16];                    //0x0
    unsigned char** codeBlocksOffset;    //0x10
    char padding2[4];                    //0x18
    int codeLength;                        //0x1C
    char padding3[4];                    //0x20
    int localCount;                        //0x24
    char padding4[4];                    //0x28
    int nativeCount;                    //0x2C
    __int64* localOffset;                //0x30
    char padding5[8];                    //0x38
    __int64* nativeOffset;                //0x40
    char padding6[16];                    //0x48
    int nameHash;                        //0x58
    char padding7[4];                    //0x5C
    char* name;                            //0x60
    char** stringsOffset;                //0x68
    int stringSize;                        //0x70
    char padding8[12];                    //0x74
                                        //END_OF_HEADER

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

#pragma pack(pop)

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
            return NULL;//table initialisation hasnt happened yet
        }
        for (int i = 0; i < count; i++) {
            if (TablePtr[i].hash == hash) {
                return &TablePtr[i];
            }
        }
        return NULL;
    }
};

struct GlobalTable {
    __int64** GlobalBasePtr;
    __int64* AddressOf(int index) const { return &GlobalBasePtr[index >> 18 & 0x3F][index & 0x3FFFF]; }
    bool IsInitialised()const { return *GlobalBasePtr != NULL; }
};

struct HashNode {
    int hash;
    UINT16 data;
    UINT16 padding;
    HashNode* next;
};

class MemoryAccess {
public:
    static uintptr_t FindPattern(const char* pattern, const char* mask, const char* startAddress, size_t size);
    static uintptr_t FindPattern(const char* pattern, const char* mask);
    static void Init();
    static char* GetVehicleGameName(int modelHash);
    static char* GetVehicleMakeName(int modelHash);
    static std::vector<uint16_t> GetVehicleModKits(int modelHash);
private:
    static bool findShopController();
    static void enableCarsGlobal();

};

void setupHooks();
void removeHooks();