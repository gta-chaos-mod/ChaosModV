#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

enum TiredMode {
	closingEyes,
	waiting,
	done
};
static TiredMode currentMode = TiredMode::closingEyes;
static float alpha;
static int closingIterator;
static int nextTimestamp;

static const Hash playerGroup = GET_HASH_KEY("PLAYER");
static const Hash civGroup = GET_HASH_KEY("MEDIC");
static const float variance = 0.005f;
static Ped doctor;
static Hash relationshipGroup;
static bool hasBeenStabbed = false;

static void BlackOut(float alpha)
{
	DRAW_RECT(.5f, .5f, 1.f, 1.f, 0, 0, 0, alpha, false);
}

static void OnStart() {
	currentMode = TiredMode::closingEyes;
	alpha = 0;
	closingIterator = 20;
	hasBeenStabbed = false;

	ADD_RELATIONSHIP_GROUP("_HOSTILE_RANDOM", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relationshipGroup, civGroup);

	Ped player = PLAYER_PED_ID();
	Vector3 pos = GET_ENTITY_COORDS(player, false);

	Vector3 newPos = pos;
	newPos.x *= g_random.GetRandomFloat(1.f - variance, 1.f + variance);
	newPos.y *= g_random.GetRandomFloat(1.f - variance, 1.f + variance);

	doctor = CreatePoolPed(civGroup, 0xD47303AC, newPos.x, newPos.y, newPos.z, GET_ENTITY_HEADING(player));

	if (IS_PED_IN_ANY_VEHICLE(player, false))
	{
		SET_PED_INTO_VEHICLE(doctor, GET_VEHICLE_PED_IS_IN(player, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(doctor, relationshipGroup);
	SET_PED_HEARING_RANGE(doctor, 9999.f);
	SET_PED_CONFIG_FLAG(doctor, 281, true);

	SET_PED_COMBAT_ATTRIBUTES(doctor, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(doctor, 46, true);

	GIVE_WEAPON_TO_PED(doctor, GET_HASH_KEY("WEAPON_KNIFE"), 1, true, true);

	Entity currWeapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(doctor);
	SET_ENTITY_VISIBLE(currWeapon, false, 0);

	int bone = GET_PED_BONE_INDEX(doctor, 0xDEAD); // (kolyaventuri): Right Hand
	Entity syringe = CREATE_OBJECT(4233000725, 0, 0, 0, false, false, false); // (kolyaventuri): 4233000725 == syringe hash
	ATTACH_ENTITY_TO_ENTITY(syringe, doctor, bone, 0.1f, 0.f, 0.f, 70.0f, 0.0f, 0.f, true, true, false, false, 2, true);
	
}

static void OnTick() {
	Ped player = PLAYER_PED_ID();

	if (HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(player, doctor, 1) && !hasBeenStabbed) {
		SET_RELATIONSHIP_BETWEEN_GROUPS(3, relationshipGroup, playerGroup);
		SET_RELATIONSHIP_BETWEEN_GROUPS(3, relationshipGroup, civGroup);
		hasBeenStabbed = true;
		SET_ENTITY_HEALTH(doctor, 0, 1);

		CLEAR_PED_TASKS_IMMEDIATELY(player);
		SET_PED_TO_RAGDOLL(player, 10000, 10000, 0, true, true, false);
	}

	if (hasBeenStabbed) {
		switch (currentMode)
		{
			case closingEyes:
				alpha += closingIterator;
				if (alpha >= 255)
				{
					currentMode = TiredMode::waiting;
					nextTimestamp = GET_GAME_TIMER() + 30000;
					if (closingIterator > 1)
					{
						closingIterator = max(1, closingIterator - 2);
					}
				}
				break;
			case TiredMode::waiting:
				DISABLE_ALL_CONTROL_ACTIONS(0);
				if (DOES_ENTITY_EXIST(doctor)) {
					SET_ENTITY_AS_MISSION_ENTITY(doctor, 1, 1);
					DELETE_PED(&doctor);
				}

				if (GET_GAME_TIMER() > nextTimestamp)
				{
					currentMode = TiredMode::done;
					alpha = 0;
				}
				break;
		}
		BlackOut(alpha);
	}
}

static void OnStop() {
	if (!hasBeenStabbed) {
		SET_ENTITY_AS_MISSION_ENTITY(doctor, 1, 1);
		DELETE_PED(&doctor);
	}
	BlackOut(0);
}

static RegisterEffect registerEffect(EFFECT_MISC_SLEEPYTIME, OnStart, OnStop, OnTick);