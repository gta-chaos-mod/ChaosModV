/*
    Effect by Gorakh
*/

#include <stdafx.h>

#include "Effects/MetaModifiers.h"

#include "Memory/UI.h"

static constexpr int NUM_HUD_COMPONENTS = 21;

static float originalMobilePhoneX;

static std::map<int, float> originalHudComponentX;

static std::map<int, float> hudComponentXValues = {
	{ 1, -0.9f },  // HUD_WANTED_STARS
	{ 2, -0.9f },  // HUD_WEAPON_ICON
	{ 3, -0.9f },  // HUD_CASH
	{ 6, -0.8f },  // HUD_VEHICLE_NAME
	{ 7, -0.8f },  // HUD_AREA_NAME
	{ 8, -0.8f },  // HUD_VEHICLE_CLASS
	{ 9, -0.8f },  // HUD_STREET_NAME
	{ 10, 0.8f },  // HUD_HELP_TEXT
	{ 11, 0.8f },  // HUD_FLOATING_HELP_TEXT_1
	{ 12, 0.8f },  // HUD_FLOATING_HELP_TEXT_2
	{ 13, -0.8f }, // HUD_CASH_CHANGE
	{ 17, -0.8f }, // HUD_SAVING_GAME
	{ 20, -0.7f }, // HUD_WEAPON_WHEEL_STATS
};

static void OnStart()
{
	for (int i = 0; i < NUM_HUD_COMPONENTS; i++)
	{
		if (hudComponentXValues.contains(i))
		{
			originalHudComponentX[i] = GET_HUD_COMPONENT_POSITION(i).x;
		}
	}

	Vector3 originalMobilePhonePos;
	GET_MOBILE_PHONE_POSITION(&originalMobilePhonePos);
	originalMobilePhoneX = originalMobilePhonePos.x;
}

static void OnTick()
{
	MetaModifiers::m_bFlipChaosUI = true;

	for (int i = 0; i < NUM_HUD_COMPONENTS; i++)
	{
		if (hudComponentXValues.contains(i))
		{
			float yPos = GET_HUD_COMPONENT_POSITION(i).y;
			SET_HUD_COMPONENT_POSITION(i, hudComponentXValues[i], yPos);
		}
	}

	Vector3 mobilePos;
	GET_MOBILE_PHONE_POSITION(&mobilePos);
	SET_MOBILE_PHONE_POSITION(-110.f, mobilePos.y, mobilePos.z);

	Memory::SetRadarOffsetX(0.825f);

	// TODO: Find a way to move character swap circle to the left
}

static void OnStop()
{
	MetaModifiers::m_bFlipChaosUI = false;

	for (int i = 0; i < NUM_HUD_COMPONENTS; i++)
	{
		if (hudComponentXValues.contains(i))
		{
			SET_HUD_COMPONENT_POSITION(i, originalHudComponentX[i], GET_HUD_COMPONENT_POSITION(i).y);
		}
	}

	Vector3 mobilePos;
	GET_MOBILE_PHONE_POSITION(&mobilePos);
	SET_MOBILE_PHONE_POSITION(originalMobilePhoneX, mobilePos.y, mobilePos.z);

	Memory::SetRadarOffsetX(0.f);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Flipped HUD",
		.Id = "misc_flip_ui",
		.IsTimed = true,
		.IncompatibleWith = { "no_hud" }
	}
);