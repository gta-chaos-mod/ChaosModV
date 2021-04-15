#include <stdafx.h>

static Vector3 DegToRadian(const Vector3& angles)
{
	return Vector3::Init(
		angles.x * .0174532925199433F,
		angles.y * .0174532925199433F,
		angles.z * .0174532925199433F
	);
}

static Vector3 GetCoordsFromGameplayCam(float distance)
{
	Vector3 rot = DegToRadian(GET_GAMEPLAY_CAM_ROT(2));
	Vector3 coords = GET_GAMEPLAY_CAM_COORD();

	rot.y = distance * cos(rot.x);
	coords.x = coords.x + rot.y * std::sin(rot.z * -1.f);
	coords.y = coords.y + rot.y * std::cos(rot.z * -1.f);
	coords.z = coords.z + distance * sin(rot.x);

	return coords;
}

static bool IsWeaponShotgun(Hash wepHash)
{
	switch (wepHash)
	{
	case 487013001:
	case 2017895192:
	case -1654528753:
	case -494615257:
	case -1466123874:
	case 984333226:
	case -275439685:
	case 317205821:
		return true;
	}

	return false;
}

static void OnTick()
{
	static const Hash thingHash = GET_HASH_KEY("a_c_thing_01");
	LoadModel(thingHash);

	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 spawnBasePos;
			Vector3 spawnRot;

			if (IS_PED_A_PLAYER(ped))
			{
				Vector3 camCoords = GET_GAMEPLAY_CAM_COORD();
				Vector3 pedPos = GET_ENTITY_COORDS(ped, false);

				float distCamToPed = GET_DISTANCE_BETWEEN_COORDS(pedPos.x, pedPos.y, pedPos.z, camCoords.x, camCoords.y, camCoords.z, true);

				spawnBasePos = GetCoordsFromGameplayCam(distCamToPed + 2.5f);
				spawnRot = GET_GAMEPLAY_CAM_ROT(2);
			}
			else
			{
				spawnBasePos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, .0f, 5.f, .0f);
				spawnRot = GET_ENTITY_ROTATION(ped, 2);
			}

			bool isShotgun = IsWeaponShotgun(GET_SELECTED_PED_WEAPON(ped));
			int thingCount = isShotgun ? 3 : 1;
			for (int i = 0; i < thingCount; i++)
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

				int objType = GET_RANDOM_INT_IN_RANGE(0, 3);
				std::vector<Vehicle> vehs;
				std::vector<Ped> peds;
				std::vector<Entity> props;
				Entity thing, thingProp;
				Vehicle thingVeh;
				Ped thingPed;
				for (Vehicle veh : GetAllVehs())
				{
					vehs.push_back(veh);
				}
				if (!vehs.empty())
				{
					thingVeh = vehs[g_random.GetRandomInt(0, vehs.size() - 1)];
				}
				for (Ped ped : GetAllPeds())
				{
					peds.push_back(ped);
				}
				if (!peds.empty())
				{
					thingPed = peds[g_random.GetRandomInt(0, peds.size() - 1)];
				}
				for (Entity prop : GetAllProps())
				{
					props.push_back(prop);
				}
				if (!props.empty())
				{
					thingProp = props[g_random.GetRandomInt(0, props.size() - 1)];
				}
				
				switch(objType)
				{
				case 0: 
					thing = thingProp;
					break;
				case 1:
					thing = thingPed;
					CLEAR_PED_TASKS_IMMEDIATELY(thingPed);
					SET_PED_TO_RAGDOLL(thingPed, 2000, 2000, 0, 1, 1, 0);
					break;
				case 2:
					thing = thingVeh;
					break;
				}
				if (thing != NULL)
				{
					SET_ENTITY_NO_COLLISION_ENTITY(ped, thing, true);
					SET_ENTITY_COORDS(thing, spawnPos.x, spawnPos.y, spawnPos.z, false, false, false, false);
					SET_ENTITY_ROTATION(thing, spawnRot.x, spawnRot.y, spawnRot.z, 2, true);
					APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(thing, 1, .0f, 1000.f, 0.f, false, true, true, false);
					SET_ENTITY_AS_NO_LONGER_NEEDED(&thing);
				}
			}
		}
	}

	SET_MODEL_AS_NO_LONGER_NEEDED(thingHash);
}

static RegisterEffect registerEffect(EFFECT_MISC_STUFFGUNS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Improvised Weaponry",
		.Id = "misc_stuffguns",
		.IsTimed = true,
		.IsShortDuration = true
	}
);