/*
    Effect By Rylxnd
*/

#include <stdafx.h>

#include "Memory/Hooks/ScriptThreadRunHook.h"

#include "Components/Globals.h"

struct PhoneCallLine
{
	const char *lineGXT;
	const char *subtitleGXT;
};

struct PhoneCall
{
	const char *contactName;
	const char *profileTxdName;
	const char *speakerName;
	const char *GXTFile;
	const char *ambientLine;

	std::vector<PhoneCallLine> lines;
};

struct CellPhone
{
	alignas(8) uint32_t unk0;
	alignas(8) uint32_t state;
};

static int lockCellPhoneGlobalHandle  = -1;
static int cellphoneStateGlobalHandle = -1;

static uint8_t *lockCellphoneGlobal;
static CellPhone *cellphoneStateGlobal;

static int phoneScaleform                                     = 0;
static int currentMode                                        = 0;
static int lastTick                                           = 0;
static int soundId                                            = 0;

static bool isDialing                                         = true;
static bool lockCellphone                                     = true;

static std::unordered_map<Hash, const char *> phoneScaleforms = { { 0xD7114C9, "cellphone_ifruit" },
	                                                              { 0x9B22DBAF, "cellphone_badger" },
	                                                              { 0x9B810FA2, "cellphone_facade" } };

static void DrawPhone(PhoneCall &phoneCall)
{
	if (isDialing)
	{
		BEGIN_SCALEFORM_MOVIE_METHOD(phoneScaleform, "SET_SOFT_KEYS");
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(2);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(true);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(5);
		END_SCALEFORM_MOVIE_METHOD();
	}
	else
	{
		BEGIN_SCALEFORM_MOVIE_METHOD(phoneScaleform, "SET_SOFT_KEYS");
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(2);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(true);
		SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(1);
		END_SCALEFORM_MOVIE_METHOD();
	}

	BEGIN_SCALEFORM_MOVIE_METHOD(phoneScaleform, "SET_SOFT_KEYS");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(3);
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(true);
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(1);
	END_SCALEFORM_MOVIE_METHOD();

	BEGIN_SCALEFORM_MOVIE_METHOD(phoneScaleform, "SET_DATA_SLOT");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(4);
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(0);
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(3);

	BEGIN_TEXT_COMMAND_SCALEFORM_STRING(phoneCall.contactName);
	END_TEXT_COMMAND_SCALEFORM_STRING();

	BEGIN_TEXT_COMMAND_SCALEFORM_STRING("CELL_2000");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(phoneCall.profileTxdName);
	END_TEXT_COMMAND_SCALEFORM_STRING();

	BEGIN_TEXT_COMMAND_SCALEFORM_STRING(isDialing ? "CELL_217" : "CELL_219");
	END_TEXT_COMMAND_SCALEFORM_STRING();

	END_SCALEFORM_MOVIE_METHOD();

	BEGIN_SCALEFORM_MOVIE_METHOD(phoneScaleform, "DISPLAY_VIEW");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(4);
	END_SCALEFORM_MOVIE_METHOD();
}

static void UpdatePhone(PhoneCall &phoneCall)
{
	DrawPhone(phoneCall);
	*lockCellphoneGlobal = lockCellphone; // disables phone inputs
	TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("cellphone_controller");
}

static void PlayPhoneCall(PhoneCall &phoneCall)
{
	if (cellphoneStateGlobalHandle < 0)
	{
		cellphoneStateGlobalHandle =
		    Globals::RegisterGlobal("CellPhoneState", "dialogue_handler",
		                            "2D 00 02 00 ? 52 ? ? 41 ? 72 08 2A 06 56 ? ? 52 ? ? 41 ? 71 08 20 56 ? ? 72", 6,
		                            GlobalPatternIdiom::GLOBAL_U16);
	}

	if (lockCellPhoneGlobalHandle < 0)
	{
		lockCellPhoneGlobalHandle = Globals::RegisterGlobal(
		    "CellPhoneLock", "dialogue_handler",
		    "72 54 ? ? 5D ? ? ? 71 2C ? ? ? 2B 72 54 ? ? 77 54 ? ? 52 ? ? 25 ? 2C ? ? ? 53 ? ? 06 56 ? ? 52 ? ? 76", 2,
		    GlobalPatternIdiom::GLOBAL_U16);
	}

	while (Globals::Searching(cellphoneStateGlobalHandle) || Globals::Searching(lockCellPhoneGlobalHandle))
	{
		WAIT(0);
	}

	if (cellphoneStateGlobalHandle < 0 || lockCellPhoneGlobalHandle < 0)
	{
		return;
	}

	cellphoneStateGlobal = Globals::GetGlobalAddr<CellPhone>(cellphoneStateGlobalHandle);
	lockCellphoneGlobal  = Globals::GetGlobalAddr<uint8_t>(lockCellPhoneGlobalHandle);

	currentMode          = 0;

	Ped playerPed        = PLAYER_PED_ID();
	soundId              = GET_SOUND_ID();

	phoneScaleform       = REQUEST_SCALEFORM_MOVIE(phoneScaleforms.at(GET_ENTITY_MODEL(playerPed)));
	while (!HAS_SCALEFORM_MOVIE_LOADED(phoneScaleform))
	{
		WAIT(0);
	}

	CLEAR_ADDITIONAL_TEXT(14, false);
	REQUEST_ADDITIONAL_TEXT_FOR_DLC(phoneCall.GXTFile, 14);

	while (!HAS_ADDITIONAL_TEXT_LOADED(14))
	{
		WAIT(0);
	}

	if (_GET_NUMBER_OF_REFERENCES_OF_SCRIPT_WITH_NAME_HASH(GET_HASH_KEY("cellphone_flashhand")) == 0)
	{
		REQUEST_SCRIPT("cellphone_flashhand");
		while (!HAS_SCRIPT_LOADED("cellphone_flashhand"))
		{
			WAIT(0);
		}

		START_NEW_SCRIPT("cellphone_flashhand", 1424);
		SET_SCRIPT_AS_NO_LONGER_NEEDED("cellphone_flashhand");
	}

	cellphoneStateGlobal->state = 4; // Open Phone

	while (cellphoneStateGlobal->state < 6)
	{
		WAIT(0);
	}

	cellphoneStateGlobal->state = 10; // Put into further state

	lockCellphone = true;

	while (currentMode < 9)
	{
		WAIT(0);
		UpdatePhone(phoneCall);

		auto curTick = GET_GAME_TIMER();

		switch (currentMode)
		{
		case 0: // Start the phone ring and curse
		{
			PLAY_SOUND_FRONTEND(soundId, "PHONE_GENERIC_RING_01", "Phone_SoundSet_Default", true);
			PLAY_PED_AMBIENT_SPEECH_NATIVE(playerPed, "GENERIC_FRIGHTENED_HIGH", "SPEECH_PARAMS_FORCE_SHOUTED", false);

			isDialing = true;
			lastTick  = GET_GAME_TIMER();
			currentMode++;

			break;
		}
		case 1: // Check if phone has ringed long enough or player had hit the Call Accept button
		{
			if ((curTick - lastTick >= 4000)
			    || (IS_CONTROL_JUST_PRESSED(2, 176) or IS_DISABLED_CONTROL_JUST_PRESSED(2, 176)))
			{
				PLAY_SOUND_FRONTEND(-1, "Menu_Accept", "Phone_SoundSet_Default", true);
				STOP_SOUND(soundId);

				isDialing = false;
				currentMode++;
			}
			break;
		}
		case 2: // Play gesture to put player phone to ear
		{
			TASK_USE_MOBILE_PHONE(playerPed, true, 1);
			currentMode++;
			break;
		}
		case 3: // Create conversation and say hello
		{
			if (IS_SCRIPTED_CONVERSATION_ONGOING() || IS_MOBILE_PHONE_CALL_ONGOING())
			{
				STOP_SCRIPTED_CONVERSATION(false);
			}

			CREATE_NEW_SCRIPTED_CONVERSATION();

			ADD_PED_TO_CONVERSATION(0, 0, phoneCall.speakerName);
			ADD_PED_TO_CONVERSATION(1, playerPed, "");

			for (PhoneCallLine line : phoneCall.lines)
			{
				ADD_LINE_TO_CONVERSATION(0, line.lineGXT, line.subtitleGXT, 1, 0, false, false, true, true, 0, false,
				                         false, false);
			}

			PLAY_PED_AMBIENT_SPEECH_NATIVE(playerPed, "GENERIC_HI", "SPEECH_PARAMS_FORCE_SHOUTED", false);
			currentMode++;
			break;
		}
		case 4: // Wait for hello to finish & start convo
		{
			if (!IS_AMBIENT_SPEECH_PLAYING(playerPed))
			{
				START_SCRIPT_PHONE_CONVERSATION(true, true);
				currentMode++;
			}
			break;
		}
		case 5: // Loop while phone call is playing and do gesture logic
		{
			if (IS_MOBILE_PHONE_CALL_ONGOING())
			{
				if (!IS_PED_RUNNING_MOBILE_PHONE_TASK(playerPed))
				{
					TASK_USE_MOBILE_PHONE(playerPed, true, 1);
				}
			}
			else
			{
				PLAY_PED_AMBIENT_SPEECH_NATIVE(playerPed, "GENERIC_BYE", "SPEECH_PARAMS_FORCE_SHOUTED", false);
				currentMode++;
			}
			break;
		}
		case 6: // Wait for goodbye to finish
		{
			if (!IS_AMBIENT_SPEECH_PLAYING(playerPed))
			{
				currentMode++;
			}
			break;
		}
		case 7: // Hang Up Phone
		{
			TASK_USE_MOBILE_PHONE(playerPed, true, 2);
			PLAY_SOUND_FRONTEND(-1, "Hang_Up", "Phone_SoundSet_Michael", true);

			lockCellphone               = false;
			cellphoneStateGlobal->state = 3;
			currentMode++;

			lastTick = GET_GAME_TIMER();

			break;
		}
		case 8: // Say insult
		{
			if (curTick - lastTick > 800)
			{
				lastTick = curTick;
				PLAY_PED_AMBIENT_SPEECH_NATIVE(playerPed, phoneCall.ambientLine, "SPEECH_PARAMS_FORCE_SHOUTED", false);
				currentMode++;
			}
			break;
		}
		}
	}

	REQUEST_SCRIPT("cellphone_controller");

	while (!HAS_SCRIPT_LOADED("cellphone_controller"))
	{
		WAIT(0);
	}

	START_NEW_SCRIPT("cellphone_controller", 1424);

	SET_SCRIPT_AS_NO_LONGER_NEEDED("cellphone_controller");

	RELEASE_SOUND_ID(soundId);
	SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&phoneScaleform);

	*lockCellphoneGlobal = 0;
}

static void OnStartAgatha()
{
	PhoneCall phoneCall = { "CELL_CAS_MAN_N",
		                    "CHAR_CASINO_MANAGER",
		                    "CAS_AGATHA",
		                    "CAGTAU",
		                    "GENERIC_INSULT_FEMALE",
		                    {
		                        { "CAGT_HAAA", "CAGT_M1_UC1_1" },
		                        { "CAGT_HAAB", "CAGT_M1_UC1_3" },
		                        { "CAGT_HAAC", "CAGT_M1_UC1_5" },
		                        { "CAGT_HAAD", "CAGT_M1_UC1_7" },
		                        { "CAGT_HAAE", "CAGT_M1_UC1_9" },
		                        { "CAGT_HAAF", "CAGT_M1_UC1_10" },
		                        { "CAGT_HAAG", "CAGT_M1_UC1_11" },
		                        { "CAGT_HAAH", "CAGT_M1_UC1_12" },
		                        { "CAGT_HAAI", "CAGT_M1_UC1_14" },
		                    } };
	PlayPhoneCall(phoneCall);
}

static void OnStartDave()
{
	int random          = g_Random.GetRandomInt(1, 2);

	PhoneCall phoneCall = { "CELL_NCLUBE_N",
		                    "CHAR_ENGLISH_DAVE",
		                    "BTL_DAVE",
		                    "HS4EDAU",
		                    "GENERIC_INSULT_MALE",
		                    { { "HS4ED_AAAA", "HS4ED_POC1_1" },
		                      { "HS4ED_AAAB", "HS4ED_POC1_3" },
		                      { "HS4ED_AAAC", "HS4ED_POC1_5" },
		                      { "HS4ED_AAAD", "HS4ED_POC1_7" },
		                      { (random == 1 ? "HS4ED_ACAB" : "HS4ED_ABAB"),
		                        (random == 1 ? "HS4ED_PDM1_3" : "HS4ED_PAM1_3") } } };
	PlayPhoneCall(phoneCall);
}

// clang-format off
REGISTER_EFFECT(OnStartAgatha, nullptr, nullptr, EffectInfo
	{
		.Name = "Hello, this is Agatha",
		.Id = "misc_phonecall_agatha",
		.IncompatibleWith = { "player_nophone", "misc_phonecall_dave" },
	}
);

REGISTER_EFFECT(OnStartDave, nullptr, nullptr, EffectInfo
	{
		.Name = "Dave Here",
		.Id = "misc_phonecall_dave",
		.IncompatibleWith = { "player_nophone", "misc_phonecall_agatha" },
	}
);