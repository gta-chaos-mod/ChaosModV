#include <stdafx.h>

static void OnTick()
{
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

				spawnBasePos = Util::GetCoordsFromGameplayCam(distCamToPed + 2.5f);
				spawnRot = GET_GAMEPLAY_CAM_ROT(2);
			}
			else
			{
				spawnBasePos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, .0f, 5.f, .0f);
				spawnRot = GET_ENTITY_ROTATION(ped, 2);
			}

			bool isShotgun = Util::IsWeaponShotgun(GET_SELECTED_PED_WEAPON(ped));
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

				//these two can't go in the switch, not sure why but it won't build.
				std::vector<Ped> peds;
				std::vector<Entity> props;

				Entity thing;
				
				switch(objType)
				{
				case 0: 
					for (Entity prop : GetAllProps())
					{
						props.push_back(prop);
					}
					if (!props.empty())
					{
						Entity thingProp = props[g_Random.GetRandomInt(0, props.size() - 1)];
						thing = thingProp;
					}			
					break;
				case 1:
					for (Ped ped : GetAllPeds())
					{
						peds.push_back(ped);
					}
					if (!peds.empty())
					{
						Ped thingPed = peds[g_Random.GetRandomInt(0, peds.size() - 1)];
						thing = thingPed;
					}				
					break;
				case 2:
					std::vector<Vehicle> vehs;
					for (Vehicle veh : GetAllVehs())
					{
						vehs.push_back(veh);
					}
					if (!vehs.empty())
					{
						Vehicle thingVeh = vehs[g_Random.GetRandomInt(0, vehs.size() - 1)];
						thing = thingVeh;
					}
					break;
				}
				if (thing != NULL)
				{
					SET_ENTITY_NO_COLLISION_ENTITY(ped, thing, true);
					SET_ENTITY_COORDS(thing, spawnPos.x, spawnPos.y, spawnPos.z, false, false, false, false);
					SET_ENTITY_ROTATION(thing, spawnRot.x, spawnRot.y, spawnRot.z, 2, true);
					if (GET_ENTITY_TYPE(thing) == 1)
					{
						//has to be done after ped is teleported or 'APPLY_FORCE' doesn't work
						CLEAR_PED_TASKS_IMMEDIATELY(thing);
						SET_PED_TO_RAGDOLL(thing, 2000, 2000, 0, 1, 1, 0);
					}
					if (GET_ENTITY_TYPE(thing) == 3)
					{
						//prevent non-moveable objects cluttering up important areas
						SET_OBJECT_AS_NO_LONGER_NEEDED(&thing);
					}
					APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(thing, 1, .0f, 1000.f, 0.f, false, true, true, false);
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_STUFFGUNS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Improvised Weaponry",
		.Id = "misc_stuffguns",
		.IsTimed = true,
		.IsShortDuration = true
	}
);