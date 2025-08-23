#pragma once

#include "Util/Types.h"

#define MAXWATERQUADS 821
#define MAXRIVERENTITIES 64

struct CWaterQuad
{
	short MinX;     // 0x00
	short MinY;     // 0x02
	short MaxX;     // 0x04
	short MaxY;     // 0x06
	uint32_t Alpha; // 0x08
	float Unk0;     // 0x0C
	float Unk1;     // 0x10
	float Z;        // 0x14
	union           // Bitfield Flags
	{
		uint8_t Flags; // 0x18
		struct
		{
			uint8_t UnkFlag0 : 1;  // Bit 0
			uint8_t IsDeep : 1;    // Bit 1
			uint8_t IsAudible : 1; // Bit 2
			uint8_t IsVisible : 1; // Bit 3
			uint8_t UnkFlag4 : 1;  // Bit 4
			uint8_t UnkFlag5 : 1;  // Bit 5
			uint8_t UnkFlag6 : 1;  // Bit 6
			uint8_t UnkFlag7 : 1;  // Bit 7
		};
	};
	uint8_t Type; // 0x19
	char Pad_01A; // 0x1A
	char Pad_01B; // 0x1B
};
static_assert(sizeof(CWaterQuad) == 0x1C);

struct CCalmingQuad
{
	short MinX; // 0x00
	short MinY; // 0x02
	short MaxX; // 0x04
	short MaxY; // 0x06
	float Unk0; // 0x08
};
static_assert(sizeof(CCalmingQuad) == 0xC);

struct CWaveQuad
{
	short MinX;    // 0x00
	short MinY;    // 0x02
	short MaxX;    // 0x04
	short MaxY;    // 0x06
	uint16_t Unk0; // 0x08
	uint8_t X;     // 0x0A
	uint8_t Y;     // 0x0B
};
static_assert(sizeof(CWaveQuad) == 0xC);

#pragma pack(push, 1)
struct CEntity
{
	char pad_0000[32];      // 0x0000
	void *ModelInfo;        // 0x0020
	char pad_0028[104];     // 0x0028
	fChaosVector3 Position; // 0x0090
};
static_assert(offsetof(CEntity, Position) == 0x90);
#pragma pack(pop)

struct alignas(8) CRiverEntity
{
	CEntity *Entity;     // 0x0000
	bool IsRiver;        // 0x0008
	char pad_0009[7];    // 0x0009
	fChaosVector3 Min;   // 0x0010
	char pad_001C[4];    // 0x001C
	fChaosVector3 Max;   // 0x0020
	char pad_002C[4];    // 0x002C
	void *TextureData;   // 0x0030
	bool HasTextureData; // 0x0038
	char pad_0039[7];    // 0x0039
};
static_assert(sizeof(CRiverEntity) == 0x40);

namespace Memory
{
	inline static CWaterQuad *GetAllWaterQuads()
	{
		static Handle handle =
		    Memory::FindPattern("4C 8B 05 ? ? ? ? 42 8B 04 03", "48 8B 15 ? ? ? ? 0F 57 C0 F3 44 0F 10 05");
		if (!handle.IsValid())
			return nullptr;

		return *handle.At(2).Into().Get<CWaterQuad *>();
	}

	inline static CCalmingQuad *GetAllCalmingQuads()
	{
		static Handle handle =
		    Memory::FindPattern("4C 8B 05 ? ? ? ? 49 83 C1 0C", "48 89 05 ? ? ? ? 66 89 3D ? ? ? ? 31 C9");
		if (!handle.IsValid())
			return nullptr;

		return *handle.At(2).Into().Get<CCalmingQuad *>();
	}

	inline static CWaveQuad *GetAllWaveQuads()
	{
		static Handle handle =
		    Memory::FindPattern("4C 8B 05 ? ? ? ? 49 83 C1 18", "48 89 05 ? ? ? ? 66 89 35 ? ? ? ? 48 C1 E6 03");
		if (!handle.IsValid())
			return nullptr;

		return *handle.At(2).Into().Get<CWaveQuad *>();
	}

	inline static CRiverEntity *GetAllRiverEntities()
	{
		static Handle handle =
		    Memory::FindPattern("48 8D 0D ? ? ? ? 48 C1 E0 06 48 8B 04 08", "89 C8 48 C1 E0 06 48 8D 0D ? ? ? ? 48");
		if (!handle.IsValid())
			return nullptr;

		return handle.At(IsLegacy() ? 2 : 8).Into().Get<CRiverEntity>();
	}
}