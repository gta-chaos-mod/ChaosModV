/*
	Effect by Last0xygen
*/

#include <stdafx.h>

static void OnStop()
{

}

struct PedHitInfo
{
	Ped pedHandle;
	int boneIdx;
	Vector3 offset;
};

static bool didSelect = false;
static std::vector<PedHitInfo> selectedOffsets;
static bool isBlocked = false;

static void PointAndShoot(PedHitInfo hitInfo, Hash weaponHash)
{
	if (!DOES_ENTITY_EXIST(hitInfo.pedHandle))
	{
		return;
	}
	// TODO Fix Camera
	WAIT(200);
	Ped player = PLAYER_PED_ID();
	Entity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(player);
	Vector3 weaponCoord = GET_ENTITY_COORDS(weapon, false);

	Vector3 targPos = GET_PED_BONE_COORDS(hitInfo.pedHandle, hitInfo.boneIdx, hitInfo.offset.x, hitInfo.offset.y, hitInfo.offset.z);
	SHOOT_SINGLE_BULLET_BETWEEN_COORDS(weaponCoord.x, weaponCoord.y, weaponCoord.z, targPos.x, targPos.y, targPos.z, 5, true, weaponHash, player, true, false, 24000);
	WAIT(200);
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Hash weaponHash;
	if (!GET_CURRENT_PED_WEAPON(player, &weaponHash, true) || GET_WEAPONTYPE_GROUP(weaponHash) == 0xD49321D4)
	{
		return;
	}


	static std::ofstream log("ChaosLog.txt");
	if (IS_CONTROL_PRESSED(0, 25))
	{
		SET_TIME_SCALE(0.2f);
		DISABLE_CONTROL_ACTION(0, 24, true);
		DISABLE_CONTROL_ACTION(2, 257, true);

		if (IS_DISABLED_CONTROL_PRESSED(0, 24) || IS_DISABLED_CONTROL_PRESSED(2, 257))
		{
			if (!didSelect)
			{
				didSelect = true;
				Vector3 launchPos = GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 0, 0));
				Vector3 targPos = GetGameplayCamOffsetInWorldCoords(Vector3::Init(0, 1000, 0));

				int rayHandle = _START_SHAPE_TEST_RAY(launchPos.x, launchPos.y, launchPos.z, targPos.x, targPos.y, targPos.z, 12, player, 7);
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
							Vector3 coord = GET_ENTITY_COORDS(entityHandle, false);
							Vector3 offset = coord - hitCoords;
							int bestBone = -1;
							float bestDistance = -1;
							static const int boneIds[] = { 0x0, 0x2e28, 0xe39f, 0xf9bb, 0x3779, 0x83c, 0xfedd, 0xe175, 0xb3fe, 0xca72, 0x9000, 0xcc4d, 0x512d, 0x8aae, 0x60e6, 0x3fcf, 0x5c57, 0x192a, 0xe0fd, 0x5c01, 0x60f0, 0x60f1, 0x60f2, 0xfcd9, 0xb1c5, 0xeeeb, 0x49d9, 0x67f2, 0xff9, 0xffa, 0x67f3, 0x1049, 0x104a, 0x67f4, 0x1059, 0x105a, 0x67f5, 0x1029, 0x102a, 0x67f6, 0x1039, 0x103a, 0xeb95, 0x8cbd, 0xee4f, 0x1470, 0x58b7, 0x29d2, 0x9d4d, 0x6e5c, 0xdead, 0xe5f2, 0xfa10, 0xfa11, 0xe5f3, 0xfa60, 0xfa61, 0xe5f4, 0xfa70, 0xfa71, 0xe5f5, 0xfa40, 0xfa41, 0xe5f6, 0xfa50, 0xfa51, 0x6f06, 0x188e, 0xab22, 0x90ff, 0xbb0, 0x9995, 0x796e, 0x322c, 0xfe2c, 0xe3db, 0xb2b6, 0x62ac, 0x542e, 0x74ac, 0xaa10, 0x6b52, 0x4b88, 0x54c, 0x2ba6, 0x9149, 0x4ed2, 0xf18f, 0x4f37, 0x4537, 0xb4a0, 0x4324, 0x508f, 0xb93b, 0xc33b, 0xb987, 0x8b93, 0xdd1c };
							for (int id : boneIds)
							{
								Vector3 boneCoord = GET_PED_BONE_COORDS(entityHandle, id, 0, 0, 0);
								float distance = hitCoords.distanceTo(boneCoord);
								if (bestBone < 0 || bestDistance < 0 || distance < bestDistance)
								{
									bestDistance = distance;
									bestBone = id;
								}
							}
							PedHitInfo hitInfo;
							hitInfo.pedHandle = entityHandle;
							hitInfo.boneIdx = bestBone;
							hitInfo.offset = Vector3::Init(0, 0, 0);
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
				Vector3 destOffset = GET_PED_BONE_COORDS(info.pedHandle, info.boneIdx, info.offset.x, info.offset.y, info.offset.z);

				DRAW_BOX(destOffset.x - 0.2, destOffset.y - 0.2, destOffset.z - 0.2, destOffset.x + 0.2, destOffset.y + 0.2, destOffset.z + 0.2, 255, 0, 0, 255);
			}
		}
	}
	else
	{
		if (selectedOffsets.size() > 0)
		{
			for (PedHitInfo info : selectedOffsets)
			{
				PointAndShoot(info, weaponHash);
				WAIT(100);
			}
		}
		SET_TIME_SCALE(1);
		selectedOffsets.clear();
	}
}

static RegisterEffect registerEffect(EFFECT_IN_THE_HOOD, nullptr, OnStop, OnTick);