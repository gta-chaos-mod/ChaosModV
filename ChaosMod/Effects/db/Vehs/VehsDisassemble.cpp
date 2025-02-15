/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/EntityFragments.h"

static void OnStart()
{
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		Vehicle veh = GET_VEHICLE_PED_IS_IN(player, false);

		for (int i = 0; i < Memory::GetFragCount(veh); ++i)
		{
			int boneIndex = Memory::GetBoneIndexByFragIndex(veh, i);

			// vehicle clips through the ground if these get removed
			if (boneIndex != GET_ENTITY_BONE_INDEX_BY_NAME(veh, "chassis")
			    && boneIndex != GET_ENTITY_BONE_INDEX_BY_NAME(veh, "chassis_lowlod")
			    && boneIndex != GET_ENTITY_BONE_INDEX_BY_NAME(veh, "chassis_dummy"))
			{
				Memory::DetachBone(veh, boneIndex);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, 
	{
		.Name = "Disassemble Current Vehicle",
		.Id = "vehs_disassemble"
	}
);