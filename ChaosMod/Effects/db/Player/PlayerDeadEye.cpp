/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Util/Camera.h"

struct PedHitInfo
{
	Ped pedHandle;
	int boneIdx;
};

static bool didSelect = false;
static std::vector<PedHitInfo> selectedOffsets;
static bool isBlocked = false;

static void OnStop()
{
	didSelect = false;
	selectedOffsets.clear();
	isBlocked = false;
	SET_TIME_SCALE(1);
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Hash weaponHash;
	if (isBlocked || !GET_CURRENT_PED_WEAPON(player, &weaponHash, true)
	    || GET_WEAPONTYPE_GROUP(weaponHash) == 0xD49321D4 || GET_WEAPONTYPE_GROUP(weaponHash) == 0xB7BBD827)
	{
		return;
	}
	// Exclude non working weapons (minigun, hellbringer)
	if (weaponHash == 0x42BF8A85 || weaponHash == 0xB62D1F67)
	{
		return;
	}
	// Check if player is aiming
	if (IS_CONTROL_PRESSED(0, 25))
	{
		SET_TIME_SCALE(0.2f);
		// Disable Fire-Buttons
		DISABLE_CONTROL_ACTION(0, 24, true);
		DISABLE_CONTROL_ACTION(2, 257, true);

		if (IS_DISABLED_CONTROL_PRESSED(0, 24) || IS_DISABLED_CONTROL_PRESSED(2, 257))
		{
			if (!didSelect)
			{
				Vector3 launchPos = Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 0, 0));
				Vector3 targPos   = Util::GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 10000, 0));

				int rayHandle     = START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(
						launchPos.x, launchPos.y, launchPos.z, targPos.x, targPos.y, targPos.z, 12, player, 7);
				if (rayHandle != 0)
				{
					BOOL didHit;
					Entity entityHandle;
					Vector3 hitCoords;
					Vector3 hitCoordsNormal;
					int result = GET_SHAPE_TEST_RESULT(rayHandle, &didHit, &hitCoords, &hitCoordsNormal, &entityHandle);
					if (didHit)
					{
						if (IS_ENTITY_A_PED(entityHandle))
						{
							didSelect                  = true;
							/*
							    SKEL_ROOT: 0x0
							    SKEL_Pelvis: 0x2e28
							    SKEL_L_Thigh: 0xe39f
							    SKEL_L_Calf: 0xf9bb
							    SKEL_L_Foot: 0x3779
							    SKEL_R_Thigh: 0xca72
							    SKEL_R_Calf: 0x9000
							    SKEL_R_Foot: 0xcc4d
							    SKEL_Spine_Root: 0xe0fd
							    SKEL_Spine0: 0x5c01
							    SKEL_Spine1: 0x60f0
							    SKEL_Spine2: 0x60f1
							    SKEL_Spine3: 0x60f2
							    SKEL_L_Clavicle: 0xfcd9
							    SKEL_L_UpperArm: 0xb1c5
							    SKEL_L_Forearm: 0xeeeb
							    SKEL_L_Hand: 0x49d9
							    SKEL_R_Clavicle: 0x29d2
							    SKEL_R_UpperArm: 0x9d4d
							    SKEL_R_Forearm: 0x6e5c
							    SKEL_R_Hand: 0xdead
							    SKEL_Neck_1: 0x9995
							    SKEL_Head: 0x796e
							*/
							int bestBone               = -1;
							float bestDistance         = -1;

							static const int boneIds[] = { 0x0,    0x2e28, 0xe39f, 0xf9bb, 0x3779, 0xca72,
								                           0x9000, 0xcc4d, 0xe0fd, 0x5c01, 0x60f0, 0x60f1,
								                           0x60f2, 0xfcd9, 0xb1c5, 0xeeeb, 0x49d9, 0x29d2,
								                           0x9d4d, 0x6e5c, 0xdead, 0x9995, 0x796e };
							for (int id : boneIds)
							{
								Vector3 boneCoord = GET_PED_BONE_COORDS(entityHandle, id, 0, 0, 0);
								float distance    = hitCoords.DistanceTo(boneCoord);
								if (bestBone < 0 || bestDistance < 0 || distance < bestDistance)
								{
									bestDistance = distance;
									bestBone     = id;
								}
							}
							PedHitInfo hitInfo;
							hitInfo.pedHandle = entityHandle;
							hitInfo.boneIdx   = bestBone;
							selectedOffsets.push_back(hitInfo);
						}
					}
				}
			}
		}
		else
		{
			didSelect = false;
		}

		for (PedHitInfo info : selectedOffsets)
		{
			if (DOES_ENTITY_EXIST(info.pedHandle))
			{
				Vector3 destOffset = GET_PED_BONE_COORDS(info.pedHandle, info.boneIdx, 0, 0, 0);
				float boxSize      = 0.1;
				DRAW_BOX(destOffset.x - boxSize, destOffset.y - boxSize, destOffset.z - boxSize, destOffset.x + boxSize,
				         destOffset.y + boxSize, destOffset.z + boxSize, 255, 0, 0, 100);
			}
		}
	}
	else
	{
		isBlocked = true;
		if (selectedOffsets.size() > 0)
		{
			Ped player    = PLAYER_PED_ID();
			Entity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(player, 0);
			for (PedHitInfo info : selectedOffsets)
			{
				if (DOES_ENTITY_EXIST(info.pedHandle))
				{
					Vector3 weaponCoord = GET_ENTITY_COORDS(weapon, false);
					Vector3 targPos     = GET_PED_BONE_COORDS(info.pedHandle, info.boneIdx, 0, 0, 0);
					SHOOT_SINGLE_BULLET_BETWEEN_COORDS(weaponCoord.x, weaponCoord.y, weaponCoord.z, targPos.x,
					                                   targPos.y, targPos.z, 5, true, weaponHash, player, true, false,
					                                   24000);
					WAIT(100);
				}
			}
		}
		SET_TIME_SCALE(1);
		selectedOffsets.clear();
		isBlocked = false;
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Dead Eye",
		.Id = "player_dead_eye",
		.IsTimed = true,
		.IncompatibleWith = { "time_x02", "time_x05", "time_superhot" }
	}
);