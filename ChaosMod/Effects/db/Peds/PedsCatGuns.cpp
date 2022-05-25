#include <stdafx.h>

#include "Memory/Physics.h"

#include "Util/Camera.h"
#include "Util/Weapon.h"

static void OnTick()
{
	static const Hash catHash = GET_HASH_KEY("a_c_cat_01");
	LoadModel(catHash);

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 spawnBasePos;
			Vector3 spawnRot;

			if (IS_PED_A_PLAYER(ped))
			{
				Vector3 camCoords  = GET_GAMEPLAY_CAM_COORD();
				Vector3 pedPos     = GET_ENTITY_COORDS(ped, false);

				float distCamToPed = GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, camCoords.x, camCoords.y,
				                                                 camCoords.z, true);

				spawnBasePos       = Util::GetCoordsFromGameplayCam(distCamToPed + .5f);
				spawnRot           = GET_GAMEPLAY_CAM_ROT(2);
			}
			else
			{
				spawnBasePos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, .0f, 1.f, .0f);
				spawnRot     = GET_ENTITY_ROTATION(ped, 2);
			}

			bool isShotgun = Util::IsWeaponShotgun(GET_SELECTED_PED_WEAPON(ped));
			int catCount   = isShotgun ? 3 : 1;
			for (int i = 0; i < catCount; i++)
			{
				if (i > 0)
				{
					WAIT(0);
				}

				Vector3 spawnPos = spawnBasePos;
				if (isShotgun)
				{
					spawnPos.z = spawnBasePos.z - .25f + i * .25f;
				}

				Ped cat = CREATE_PED(28, catHash, spawnPos.x, spawnPos.y, spawnPos.z, .0f, true, false);
				SET_ENTITY_ROTATION(cat, spawnRot.x, spawnRot.y, spawnRot.z, 2, true);

				SET_PED_TO_RAGDOLL(cat, 3000, 3000, 0, true, true, false);

				Memory::ApplyForceToEntityCenterOfMass(cat, 1, .0f, 300.f, 0.f, false, true, true, false);

				SET_PED_AS_NO_LONGER_NEEDED(&cat);
			}
		}
	}

	SET_MODEL_AS_NO_LONGER_NEEDED(catHash);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Catto Guns",
		.Id = "peds_catguns",
		.IsTimed = true
	}
);