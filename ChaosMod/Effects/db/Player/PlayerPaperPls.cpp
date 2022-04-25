/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

std::string txtRoot = "chaosmod/data/player_paper_pls/";
std::string ptxt01 = "player_paper_pls_01.png";
std::string ptxt02 = "player_paper_pls_02.png";

std::string ptxtcur = "cursor.png";

int itxt01, itxt02, itxtcur;

enum class BlinkState : int
{
	ON = 0,
	OFF = 1
};

enum class EffectState : int
{
	NONE,
	DRAWING,
	FINISHED
};

static EffectState efState = EffectState::NONE;
static BlinkState blkState = BlinkState::ON;

static bool draw = true;

static int blinkchangeinterval;
static int blinkchangemaxinterval = 21; // https://www.youtube.com/watch?v=u8zUsZQ_JvA

//Stole from scripthookv sdk samples ;)
#include <windows.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

std::string cachedModulePath;
std::string GetCurrentModulePath()
{
	if (cachedModulePath.empty())
	{
		// get module path
		char modPath[MAX_PATH];
		memset(modPath, 0, sizeof(modPath));
		GetModuleFileNameA((HMODULE)&__ImageBase, modPath, sizeof(modPath));
		for (size_t i = strlen(modPath); i > 0; i--)
		{
			if (modPath[i - 1] == '\\')
			{
				modPath[i] = 0;
				break;
			}
		}
		cachedModulePath = modPath;
	}
	return cachedModulePath;
}

static void GetTxts()
{
	std::string path = GetCurrentModulePath();
	itxt01 = createTexture((path + txtRoot + ptxt01).c_str());
	itxt02 = createTexture((path + txtRoot + ptxt02).c_str());
	itxtcur = createTexture((path + txtRoot + ptxtcur).c_str());
}

static void Abs(float x, float y, float* ax, float* ay)
{
	float ar = _GET_ASPECT_RATIO(false);
	float wdth = 1080.f * ar;

	*ax = wdth * x;
	*ay = 1080.f * y;
}

static void Rel(float x, float y, float* rx, float* ry)
{
	float ar = _GET_ASPECT_RATIO(false);
	float wdth = 1080.f * ar;

	*rx = x / wdth;
	*ry = y / 1080.f;
}

static void GetReal(float x, float y, float* gx, float* gy)
{
	float rx, ry;
	Rel(x, y, &rx, &ry);
	float nx, ny;
	_GET_SCRIPT_GFX_POSITION(rx, ry, &nx, &ny);
	Abs(nx, ny, gx, gy);
}

static bool CursorInArea(float x, float y, float w, float h)
{
	
	float cx = GET_CONTROL_NORMAL(0, 239);
	float cy = GET_CONTROL_NORMAL(0, 240);
	float rw, rh;
	Rel(w, h, &rw, &rh);
	float lx, ly;
	GetReal(x, y, &lx, &ly);
	float rx, ry;
	Rel(lx, ly, &rx, &ry);

	bool isX = cx >= rx && cx <= rx + rw;
	bool isY = cy > ry && cy < ry + rh;

	return (isX && isY);
}

static void DrawCur()
{
	float cx = GET_CONTROL_NORMAL(0, 239);
	float cy = GET_CONTROL_NORMAL(0, 240);
	int screenX, screenY;
	GET_SCREEN_RESOLUTION(&screenX, &screenY);
	float ar = _GET_ASPECT_RATIO(false);
	drawTexture(itxtcur, 0, -9998, 100, /* Scale x,y */0.03, 0.03, /* Center x,y */0.f, 0.f, /* Pos x,y */cx, cy, /* Rotation */0.f, ar, /*RGBA: */1.f, 1.f, 1.f, 1.f);
}

static void DrawTxt(int txt)
{
	int screenX, screenY;
	GET_SCREEN_RESOLUTION(&screenX, &screenY);
	float ar = _GET_ASPECT_RATIO(false);
	drawTexture(txt, 0, -9999, 100, /* Scale x,y */1920 / screenX, (1080 / screenY) / ar, /* Center x,y */0.0f, 0.0f, /* Pos x,y */0.f, 0.f, /* Rotation */0.f, ar, /*RGBA: */1.f, 1.f, 1.f, 1.f);
}

static bool HoveringButton()
{
	return CursorInArea(717, 349, 213, 58);
}

static void OnStart()
{
	int i = 0;
	GetTxts();
	efState = EffectState::NONE;
	blkState = BlinkState::ON;
	draw = true;
	blinkchangeinterval = 0;
	Hooks::EnableScriptThreadBlock();
	Player plr = PLAYER_ID();
	SET_PLAYER_CONTROL(plr, false, 1 << 7);
	if (efState == EffectState::NONE)
	{
		efState = EffectState::DRAWING;
		while (efState != EffectState::FINISHED)
		{
			WAIT(0);			
			if (draw)
			{
				if (blinkchangeinterval >= blinkchangemaxinterval)
				{
					blinkchangeinterval = 0;
					switch (blkState)
					{
					case BlinkState::ON:
						blkState = BlinkState::OFF;
						break;
					case BlinkState::OFF:
						blkState = BlinkState::ON;
						break;
					default:
						blkState = BlinkState::ON;
						break;
					}
				}
				switch (blkState)
				{
				case BlinkState::ON:
					DrawTxt(itxt01);
					break;
				case BlinkState::OFF:
					DrawTxt(itxt02);
					break;
				default:
					DrawTxt(itxt01);
					break;
				}
				DrawCur();
				blinkchangeinterval++; 
			}
			switch (efState)
			{
			case EffectState::DRAWING:
				if (i >= 3500) //Auto time out
				{
					efState = EffectState::FINISHED;
				}
				if (HoveringButton())
				{
					if (IS_CONTROL_JUST_PRESSED(0, 237))
					{
						efState = EffectState::FINISHED;
					}
				}
				break;
			case EffectState::FINISHED:
				SET_PLAYER_CONTROL(plr, true, 1 << 7);
				draw = false;
				break;
			}
			i++;
		}
	}
	SET_PLAYER_CONTROL(plr, true, 1 << 7);
	Hooks::DisableScriptThreadBlock();
}

static RegisterEffect reg(EFFECT_PLAYER_PAPER_PLS, OnStart, EffectInfo
	{
		.Name = "Papers Please",
		.Id = "player_paper_pls",
		.IsTimed = false,
	}
);