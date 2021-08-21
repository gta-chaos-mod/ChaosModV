/*
        Effect by DrUnderscore (James), modified
*/

#include <stdafx.h>
#include "../vendor/scripthookv/inc/enums.h"

// These should be 8 characters all uppercase.
// But there isn't anything enforcing that :^)
const char* ROULETTE_WORDS[] =
{
    "ROCKSTAR",
    "PONGO123",
    "DRUNDER_",
    "LAST0XYG",
    "TAKE_TWO",
    "DAVEYYYY",
    "MWEATHER",
    "RED_DEAD",
    "CHAOSMOD",
    "HACKING!",
    "ALXBLADE",
    "DVIPERAU",
    "HCKERMAN",
    "JIZZLEDS",
    "BURHAC!!",
    "SAURUS88",
    "TORIKSLV",
    "TOASTYYY",
    "ELIAS_GR",
    "KOLYA_VE",
    "LU7YOSHI",
    "P.BIDDLE",
    "SLOTHBEE",
    "ELI_RICK",
    "JUHANA!!",
    "LOSCHIKA",
    "BYHEMECH",
    "$$WASTED",
    "JOSHUAX8",
    "SSOBOSS1",
    "DZWDZWDZ",
    "BIRD1338",
    "BRANDWAR",
    "YZIMRONI",
    "T_AVENGE",
    "HUGO_ONE",
    "GATMUN!!",
    "MOXI____",
    "HUNTER2_"
};

const char* WIN_PHRASES[] =
{
    "Rockstar: Creating realistic hacking since 1998.",
    "I swear that was made for a child, by a child.",
    "I wonder what would happen if you failed...",
    "I'll make it harder next time, I promise!",
    "https://youtube.com/watch?v=dQw4w9WgXcQ",
    "I'm not sure what you hacked, but it's now hacked.",
    "i ran out of phrases to put here. please pity me.",
    "I should get Linux.",
    "Yay, hacking!",
    "ping rockstargames.com",
    "You obviously must know something about something...",
    "I can read machine code!",
    "Well that wasn't fun",
    "Was that a promotion?",
    "We'll get right back to normal gameplay, hope you weren't doing anything important",
    "I use arch btw",
    "Vim > Emacs",
    "loooool cool hacker reference xdd",
    "You wouldn't download a car...",
    "Needs more blockchain"
};

enum class TimerAction
{
    NONE,
    RESET,
    REMOVE,
    KILL
};

static int lives = 0;
static int scaleform = 0;
static int timer = 0;
static TimerAction act = TimerAction::NONE;
static int selectInputReturn = 0;
static bool finished = false;

static void ScaleformPushString(const char* text)
{
    GRAPHICS::BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
    HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    GRAPHICS::END_TEXT_COMMAND_SCALEFORM_STRING();
}

static void ScaleformUpdateLives()
{
    GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_LIVES");
    GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(lives);
    GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(2);
    GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
}

static void ScaleformRunProgram(int prog)
{
    GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "RUN_PROGRAM");
    GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(prog);
    GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
}

static void ScaleformCheckInput(int first, int second, int input)
{
    if (PAD::IS_CONTROL_JUST_PRESSED(2, first) || PAD::IS_CONTROL_JUST_PRESSED(2, second))
    {
        AUDIO::PLAY_SOUND_FRONTEND(-1, "HACKING_MOVE_CURSOR", 0, 1);
        GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_INPUT_EVENT");
        GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(input);
        GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
    }
}

static void ScaleformRemove()
{
    GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&scaleform);
    scaleform = 0;
    finished = true;
}

static void ScaleformReset()
{
    GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleform, "RESET_ROULETTE");
}

static void OnStart()
{
    act = TimerAction::NONE;
    timer = 0;
    lives = 2;
    selectInputReturn = 0;
    finished = false;
    PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), false, 0);

    scaleform = GRAPHICS::_REQUEST_SCALEFORM_MOVIE_INTERACTIVE("Hacking_PC");
    while(!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleform))
        WAIT(0);

    GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_BACKGROUND");
    GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(0);
    GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

    ScaleformRunProgram(4);
    ScaleformRunProgram(83);

    ScaleformUpdateLives();

    auto word = g_Random.GetRandomInt(0, sizeof(ROULETTE_WORDS) / sizeof(ROULETTE_WORDS[0]) - 1);
    GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_ROULETTE_WORD");
    ScaleformPushString(ROULETTE_WORDS[word]);
    GRAPHICS::END_SCALEFORM_MOVIE_METHOD();

    for(int i = 0; i < 8; i++)
    {
        GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_COLUMN_SPEED");
        GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(i);
        GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_FLOAT(g_Random.GetRandomInt(10, 100) * 1.0f);
        GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
    }

    bool breakOut = false;
    while (!breakOut)
    {
        WAIT(0);

        GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleform, 255, 255, 255, 255, 0);

        if (act == TimerAction::NONE)
        {
            ScaleformCheckInput(32, 172, 8);
            ScaleformCheckInput(33, 173, 9);
            ScaleformCheckInput(34, 174, 10);
            ScaleformCheckInput(35, 175, 11);

            if (PAD::IS_CONTROL_JUST_PRESSED(2, 201))
            {
                GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_INPUT_EVENT_SELECT");
                selectInputReturn = GRAPHICS::END_SCALEFORM_MOVIE_METHOD_RETURN_VALUE();
            }
        }

        if (selectInputReturn != 0)
        {
            if (GRAPHICS::IS_SCALEFORM_MOVIE_METHOD_RETURN_VALUE_READY(selectInputReturn))
            {
                switch (GRAPHICS::GET_SCALEFORM_MOVIE_METHOD_RETURN_VALUE_INT(selectInputReturn))
                {
                case 86: // Player succeeded in hack
                {
                    timer = MISC::GET_GAME_TIMER() + 2000;
                    act = TimerAction::REMOVE;
                    auto phrase = g_Random.GetRandomInt(0, sizeof(WIN_PHRASES) / sizeof(WIN_PHRASES[0]) - 1);
                    AUDIO::PLAY_SOUND_FRONTEND(-1, "HACKING_SUCCESS", 0, 1);
                    GRAPHICS::BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SET_ROULETTE_OUTCOME");
                    GRAPHICS::SCALEFORM_MOVIE_METHOD_ADD_PARAM_BOOL(true);
                    ScaleformPushString(WIN_PHRASES[phrase]);
                    GRAPHICS::END_SCALEFORM_MOVIE_METHOD();
                    break;
                }
                case 87: // Player failed one of the columns (our job to find if they completely failed)
                    AUDIO::PLAY_SOUND_FRONTEND(-1, "HACKING_CLICK_BAD", 0, 1);
                    if (lives-- == 0) // Out of lives
                    {
                        timer = MISC::GET_GAME_TIMER() + 2000;
                        act = TimerAction::KILL;
                        ScaleformRemove();
                        AUDIO::_PLAY_AMBIENT_SPEECH1(PLAYER::PLAYER_PED_ID(), "GENERIC_CURSE_HIGH", "SPEECH_PARAMS_FORCE_FRONTEND", 1);
                    }
                    else
                    {
                        timer = MISC::GET_GAME_TIMER() + 500;
                        act = TimerAction::RESET;
                        GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleform, "STOP_ROULETTE");
                        ScaleformUpdateLives();
                    }
                    break;
                case 92: // Properly hit character
                    AUDIO::PLAY_SOUND_FRONTEND(-1, "HACKING_CLICK", 0, 1);
                    break;
                default:
                    break;
                }

                selectInputReturn = 0;
            }
        }

        if (!IS_SCREEN_FADED_IN())
        {
            act = TimerAction::REMOVE;
        }

        if (act != TimerAction::NONE && MISC::GET_GAME_TIMER() >= timer)
        {
            switch (act)
            {
            case TimerAction::REMOVE:
                ScaleformRemove();
                PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), true, 0);
                breakOut = true;
                break;
            case TimerAction::RESET:
                ScaleformReset();
                break;
            case TimerAction::KILL:
            {
                breakOut = true;
                auto ped = PLAYER::PLAYER_PED_ID();
                if (PED::IS_PED_IN_ANY_VEHICLE(ped, false))
                {
                    auto veh = PED::GET_VEHICLE_PED_IS_IN(ped, false);
                    VEHICLE::EXPLODE_VEHICLE(veh, true, false);
                }
                else
                {
                    auto coords = ENTITY::GET_ENTITY_COORDS(ped, true);
                    FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, ExplosionTypeStickyBomb, 500.0f, true, false, 3.0f, false);
                }
                ENTITY::SET_ENTITY_HEALTH(ped, 0, 0);
                PLAYER::SET_PLAYER_CONTROL(PLAYER::PLAYER_ID(), true, 0);
            }
            break;
            case TimerAction::NONE:
            default:
                break;
            }

            timer = 0;
            act = TimerAction::NONE;
        }
    }
}

static RegisterEffect registerEffect(EFFECT_PLAYER_HACKING, OnStart, EffectInfo
	{
		.Name = "Realistic Hacking",
		.Id = "player_hacking"
	}
);
