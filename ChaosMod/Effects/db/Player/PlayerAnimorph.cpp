/*
	Effect by Chanka

	Player weapons are removed when switching character model so I have to add it all back.
*/

#include <stdafx.h>

static std::vector<const char*> animalModels = {"a_c_boar", "a_c_cat_01", "a_c_chickenhawk", "a_c_chimp", "a_c_chop", "a_c_cow", "a_c_coyote", "a_c_crow", "a_c_deer", "a_c_hen", "a_c_husky", "a_c_mtlion", "a_c_pig", "a_c_pigeon", "a_c_poodle", "a_c_pug", "a_c_rabbit_01", "a_c_rat", "a_c_retriever", "a_c_rhesus", "a_c_rottweiler", "a_c_seagull", "a_c_shepherd", "a_c_westy"};
static Hash currentModel;
static std::vector<Hash> playerWeapons = {};
static std::vector<int> playerAmmo = {};

static void Cleanup() {
	SET_PLAYER_MODEL(PLAYER_ID(), currentModel);
	_SET_PLAYER_IS_IN_ANIMAL_FORM(false);

	for (int i = 0; i < playerWeapons.size(); i++) {
		GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), playerWeapons[i], playerAmmo[i], false, false);
	}

	playerWeapons = {};
	playerAmmo = {};
}

static void OnStart() 
{
	//Prep before model change
	currentModel = GET_ENTITY_MODEL(PLAYER_PED_ID());
	for (Hash weapon : Memory::GetAllWeapons()) {
		if (HAS_PED_GOT_WEAPON(PLAYER_PED_ID(), weapon, false)) {
			playerWeapons.push_back(weapon);
			playerAmmo.push_back(GET_AMMO_IN_PED_WEAPON(PLAYER_PED_ID(), weapon));
		}
	}

	//Removing while HAS_MODEL_LOADED causes a crash
	Hash animalHash = GET_HASH_KEY(animalModels[GET_RANDOM_INT_IN_RANGE(0, static_cast<int>(animalModels.size()))]);
	REQUEST_MODEL(animalHash);
	while (!STREAMING::HAS_MODEL_LOADED(animalHash)) WAIT(0);
	SET_PLAYER_MODEL(PLAYER_ID(), animalHash);
	SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER_PED_ID());
	SET_MODEL_AS_NO_LONGER_NEEDED(animalHash);
	_SET_PLAYER_IS_IN_ANIMAL_FORM(true);
}

static void OnStop()
{
	Cleanup();
}

static void OnTick()
{
	//Softlock prevention
	if (IS_PED_DEAD_OR_DYING(PLAYER_PED_ID(), 1)) {
		Cleanup();
	}
}

static RegisterEffect registerEffect(EFFECT_ANIMORPH_PLAYER, OnStart, OnStop, OnTick);