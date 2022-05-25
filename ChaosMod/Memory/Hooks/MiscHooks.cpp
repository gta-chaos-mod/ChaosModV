#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

// Work around crash related to SET_PED_SHOOTS_AT_COORD
// Thanks to Rainbomizer!
void (*OG_crSkeleton_GetGlobalMtx)(__int64 skeleton, unsigned int ulId, void *matrix);
void HK_crSkeleton_GetGlobalMtx(__int64 skeleton, unsigned int ulId, void *matrix)
{
	if (!skeleton)
	{
		return;
	}

	if (ulId == -1)
	{
		ulId = 0;
	}

	OG_crSkeleton_GetGlobalMtx(skeleton, ulId, matrix);
}

// Thanks to FiveM
struct ChangeSetEntry
{
	struct DataFile
	{
		char name[128];
	} * dataFile;

	int type;
	// ...
};
void (*OG_ApplyChangeSetEntryStub)(ChangeSetEntry *entry);
void HK_ApplyChangeSetEntryStub(ChangeSetEntry *entry)
{
	static const std::unordered_set<std::string> badFiles {
		"dlc_mpG9EC:/x64/levels/gta5/vehicles/mpG9EC.rpf",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/vehlivery3_mods.rpf",
		"dlc_mpG9ECCRC:/common/data/levels/gta5/vehicles.meta",
		"dlc_mpG9ECCRC:/common/data/carcols.meta",
		"dlc_mpG9ECCRC:/common/data/carvariations.meta",
		"dlc_mpG9ECCRC:/common/data/handling.meta",
		"dlc_mpG9ECCRC:/common/data/shop_vehicle.meta",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/arbitergt_mods.rpf",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/astron2_mods.rpf",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/cyclone2_mods.rpf",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/ignus2_mods.rpf",
		"dlc_mpG9EC:/x64/levels/mpg9ec/vehiclemods/s95_mods.rpf",
		"dlc_mpG9EC:/x64/levels/gta5/props/Prop_Exc_01.rpf",
		"dlc_mpG9EC:/x64/levels/gta5/props/exc_Prop_Exc_01.ityp",
		"dlc_mpG9EC:/x64/levels/gta5/props/Prop_tr_overlay.rpf",
		"dlc_mpG9EC:/x64/levels/gta5/props/exc_Prop_tr_overlay.ityp",
		"dlc_mpG9EC:/x64/anim/creaturemetadata.rpf",
		"dlc_mpG9EC:/common/data/effects/peds/first_person_alternates.meta",
		"dlc_mpG9ECCRC:/common/data/mp_m_freemode_01_mpg9ec_shop.meta",
		"dlc_mpG9EC:/x64/models/cdimages/mpg9ec_male.rpf",
		"dlc_mpG9ECCRC:/common/data/mp_f_freemode_01_mpg9ec_shop.meta",
		"dlc_mpG9EC:/x64/models/cdimages/mpg9ec_female.rpf",
	};

	if (entry->type != 6 && entry->type != 7 && entry->dataFile && badFiles.contains(entry->dataFile->name))
	{
		return;
	}

	OG_ApplyChangeSetEntryStub(entry);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("E8 ? ? ? ? 4D 03 F5");
	if (!handle.IsValid())
	{
		LOG("crSkeleton::GetGlobalMtx not found!");
	}
	else
	{
		Memory::AddHook(handle.Into().Get<void>(), HK_crSkeleton_GetGlobalMtx, &OG_crSkeleton_GetGlobalMtx);
	}

	//

	handle = Memory::FindPattern("48 8D 0C 40 48 8D 0C CE E8 ? ? ? ? FF C3");
	if (!handle.IsValid())
	{
		LOG("_ApplyChangeSetEntryStub not found!");
	}
	else
	{
		Memory::AddHook(handle.At(8).Get<void>(), HK_ApplyChangeSetEntryStub, &OG_ApplyChangeSetEntryStub);
	}

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "MiscHooks", true);