#include <stdafx.h>

static std::vector<Ped> frozenPeds;
static std::map<Ped, Entity> pedGuns;
static std::map<Ped, Entity> heldWeapons;

static bool isFrozen(const Ped ped)
{
	return std::find(frozenPeds.begin(), frozenPeds.end(), ped) != frozenPeds.end();
}

static bool hasNailgun(const Ped ped)
{
	return pedGuns.count(ped) > 0;
}

static void giveNailgun(const Ped ped)
{
	int bone = GET_PED_BONE_INDEX(ped, 0xDEAD); // (kolyaventuri): Right Hand

	Entity nailgun =
		CREATE_OBJECT(1854391800, 0, 0, 0, false, false, false); // (kolyaventuri): 1854391800 == nailgun hash
	ATTACH_ENTITY_TO_ENTITY(nailgun, ped, bone, 0.15f, 0.05f, 0.01f, 70.0f, 0.0f, 180.f, true, true, false, false, 2,
	                        true);
	pedGuns[ped] = nailgun;
}

static void OnTick()
{
	for (const Ped ped : GetAllPeds())
	{
		if (!hasNailgun(ped))
		{
			giveNailgun(ped);
		}
		else
		{
			Entity nailgun = pedGuns[ped];

			if (IS_PED_DEAD_OR_DYING(ped, 1))
			{
				DETACH_ENTITY(nailgun, 0, 0);
			}
			else
			{
				// (kolyaventuri): Check for weapon visiblity
				Weapon weapon    = GET_SELECTED_PED_WEAPON(ped);
				int weaponType   = GET_WEAPON_DAMAGE_TYPE(weapon);

				int isHoldingGun = weaponType == 3;
				SET_ENTITY_VISIBLE(nailgun, isHoldingGun, 0);

				if (isHoldingGun)
				{
					Entity currWeapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(ped, 0);
					heldWeapons[ped]  = currWeapon;

					SET_ENTITY_VISIBLE(currWeapon, false, 0);
				}
			}
		}

		if (HAS_ENTITY_BEEN_DAMAGED_BY_ANY_PED(ped) && !isFrozen(ped))
		{
			frozenPeds.push_back(ped);
		}

		if (isFrozen(ped))
		{
			FREEZE_ENTITY_POSITION(ped, true);
		}
	}
}

static void OnStop()
{
	for (const Ped ped : frozenPeds)
	{
		FREEZE_ENTITY_POSITION(ped, false);
	}

	// (kolyaventuri): Reshow weapons
	for (std::map<Ped, Entity>::iterator it = heldWeapons.begin(); it != heldWeapons.end(); ++it)
	{
		SET_ENTITY_VISIBLE(it->second, true, 0);
	}

	// (kolyaventuri): Remove weapons
	for (std::map<Ped, Entity>::iterator it = pedGuns.begin(); it != pedGuns.end(); ++it)
	{
		Entity nailgun = it->second;

		SET_ENTITY_VISIBLE(nailgun, false, 0);
		SET_ENTITY_AS_MISSION_ENTITY(nailgun, true, true);

		DELETE_ENTITY(&nailgun);
	}

	// (kolyaventuri): Clean up and deallocate memory
	std::vector<Ped>().swap(frozenPeds);
	pedGuns.clear();
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Nailguns",
		.Id = "peds_nailguns",
		.IsTimed = true,
		.IsShortDuration = true
	}
);