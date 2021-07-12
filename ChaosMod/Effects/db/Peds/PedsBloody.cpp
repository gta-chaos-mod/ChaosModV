/*
	Effect by ubscal
*/

#include <stdafx.h>

static void OnStart()
{
	for (Ped ped : GetAllPeds())
	{
		APPLY_PED_DAMAGE_PACK(ped, "TD_SHOTGUN_FRONT_KILL", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "BigRunOverByVehicle ", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "Dirt_Mud", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "Explosion_Large", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "RunOverByVehicle", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "Splashback_Face_0", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "Splashback_Face_1", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "SCR_Shark", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "SCR_Cougar", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "Car_Crash_Heavy", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "TD_SHOTGUN_REAR_KILL", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "SCR_Torture", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "TD_melee_face_l", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "MTD_melee_face_r", 0, 10);
		APPLY_PED_DAMAGE_PACK(ped, "MTD_melee_face_jaw", 0, 10);
	}
}
	

static RegisterEffect registerEffect(EFFECT_BLOODY_PEDS, OnStart, EffectInfo
	{
		.Name = "Everyone Is Bloody",
		.Id = "peds_bloody"
	}
);
