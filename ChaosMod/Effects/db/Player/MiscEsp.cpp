#include <stdafx.h>

#include "Util/Color.h"
#include "Util/Types.h"

#include "Memory/Hooks/DrawingHooks.h"
#include "Memory/Hooks/WorldToScreenHook.h"

static const float maxDistance = 75.0f;
static const float thickness   = 0.001f;

static Color lineColor;

// clang-format off
static const int BONE_IDS[] = {
	0x796e, 0x9995, 0xfcd9, 0x58b7,
	0xb1c5, 0xeeeb, 0x49d9, 0x29d2,
	0x0bb0, 0x9d4d, 0x6e5c, 0xdead,
	0x2e28, 0xe39f, 0xf9bb, 0xca72,
	0x9000, 0x3779, 0xcc4d
};

Vec2 GetBoneScreenCoords(Ped ped, int boneID)
{
	Vector3 boneCoords = GET_PED_BONE_COORDS(ped, boneID, 0, 0, 0);
	Vec2 screenCoords;
	Hooks::WorldToScreen(boneCoords, &screenCoords);
	return screenCoords;
}

void DrawSkeleton(Vec2 points[], int connections[][2], int num_connections, Color lineColor, float thickness)
{
	for (int i = 0; i < num_connections; i++)
	{
		Hooks::DrawLine(points[connections[i][0]], points[connections[i][1]], lineColor, thickness);
	}
}

static bool WithinDistance(Ped from, Ped to)
{
	Vector3 fc = GET_ENTITY_COORDS(from, true);
	Vector3 tc = GET_ENTITY_COORDS(to, true);
	tc.z       = fc.z;

	float dist = fc.DistanceTo(tc);
	return dist <= maxDistance && dist >= -maxDistance;
}

static void OnStart()
{
	lineColor   = GetRandomColorRGB();
	lineColor.A = 255;
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	for (Ped ped : GetAllPeds())
	{
		if (IS_ENTITY_ON_SCREEN(ped) && !IS_ENTITY_DEAD(ped, false) && !IS_PED_A_PLAYER(ped)
		    && WithinDistance(playerPed, ped))
		{
			Vec2 points[sizeof(BONE_IDS) / sizeof(BONE_IDS[0])] {};
			for (size_t i = 0; i < sizeof(BONE_IDS) / sizeof(BONE_IDS[0]); i++)
			{
				points[i] = GetBoneScreenCoords(ped, BONE_IDS[i]);
			}

			// clang-format off
			int connections[][2] = {
				{  0,  1 }, {  1,  4 }, {  4,  5 }, {  5,  6 },
				{  1,  9 }, {  9, 10 }, { 10, 11 }, {  1, 12 },
				{ 12, 13 }, { 13, 14 }, { 12, 15 }, { 15, 16 },
				{ 14, 17 }, { 16, 18 },
			};

			DrawSkeleton(points, connections, sizeof(connections) / sizeof(connections[0]), lineColor, thickness);
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "ESP",
		.Id = "msic_esp",
		.IsTimed = true,
		.EffectGroupType = EffectGroupType::Weapons
	}
);