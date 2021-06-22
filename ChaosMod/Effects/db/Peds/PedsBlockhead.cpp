/*
    Effect by Gorakh
*/

#include <stdafx.h>

#define IK_Head_ID 0x322c
#define BOX_SIZE 0.4f

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		Vector3 boneCoords = GET_PED_BONE_COORDS(ped, IK_Head_ID, 0.f, 0.f, 0.f);

		Vector3 startBoxCoords;
		startBoxCoords.x = boneCoords.x - (BOX_SIZE / 2.f);
		startBoxCoords.y = boneCoords.y - (BOX_SIZE / 2.f);
		startBoxCoords.z = boneCoords.z - (BOX_SIZE / 2.f);

		Vector3 endBoxCoords;
		endBoxCoords.x = boneCoords.x + (BOX_SIZE / 2.f);
		endBoxCoords.y = boneCoords.y + (BOX_SIZE / 2.f);
		endBoxCoords.z = boneCoords.z + (BOX_SIZE / 2.f);

		DRAW_BOX(startBoxCoords.x, startBoxCoords.y, startBoxCoords.z, endBoxCoords.x, endBoxCoords.y, endBoxCoords.z, 200, 200, 200, 255);
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_BLOCKHEAD, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Blockhead",
		.Id = "peds_blockhead",
		.IsTimed = true
	}
);