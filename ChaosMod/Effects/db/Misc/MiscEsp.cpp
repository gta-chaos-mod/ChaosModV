#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Color.h"
#include "Util/Types.h"

#include "Memory/Drawing.h"
#include "Memory/WorldToScreen.h"

// This file is manually formatted.
// clang-format off

#define BONE_COUNT 19

CHAOS_VAR const float maxDistance = 75.0f;
CHAOS_VAR const float thickness   = 0.001f;

CHAOS_VAR Color lineColor;

CHAOS_VAR const std::array<int, BONE_COUNT> BONE_IDS = {
	0x796e, 0x9995, 0xfcd9, 0x58b7,
	0xb1c5, 0xeeeb, 0x49d9, 0x29d2,
	0x0bb0, 0x9d4d, 0x6e5c, 0xdead,
	0x2e28, 0xe39f, 0xf9bb, 0xca72,
	0x9000, 0x3779, 0xcc4d
};

CHAOS_VAR std::array<std::array<int, 2>, 14> connections = {{
	{  0,  1 }, {  1,  4 }, {  4,  5 }, {  5,  6 },
	{  1,  9 }, {  9, 10 }, { 10, 11 }, {  1, 12 },
	{ 12, 13 }, { 13, 14 }, { 12, 15 }, { 15, 16 },
	{ 14, 17 }, { 16, 18 },
}};

static ChaosVector2 GetBoneScreenCoords(Ped ped, int boneID)
{
	Vector3 boneCoords = GET_PED_BONE_COORDS(ped, boneID, 0, 0, 0);
	ChaosVector2 screenCoords;
	Memory::WorldToScreen((ChaosVector3)boneCoords, &screenCoords);
	return screenCoords;
}

static void DrawSkeleton(
	const std::array<ChaosVector2, 19> &points,
	const std::array<std::array<int, 2>, 14> &connections,
	Color lineColor, float thickness)
{
	for (const auto &connection : connections)
	{
		ChaosVector2 pointA = points[connection[0]];
		ChaosVector2 pointB = points[connection[1]];

		bool validPointA = pointA.x > 0 && pointA.x < 1 && pointA.y > 0 && pointA.y < 1;
		bool validPointB = pointB.x > 0 && pointB.x < 1 && pointB.y > 0 && pointB.y < 1;

		if (validPointA && validPointB)
		{
			Memory::DrawLine(pointA, pointB, lineColor, thickness);
		}
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
	lineColor = GetRandomColorRGB();
}

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	for (auto ped : GetAllPeds())
	{
		if (IS_ENTITY_ON_SCREEN(ped) && !IS_ENTITY_DEAD(ped, false) &&
			!IS_PED_A_PLAYER(ped) && WithinDistance(playerPed, ped))
		{
			std::array<ChaosVector2, BONE_COUNT> points {};
			for (size_t i = 0; i < points.size(); i++)
			{
				points[i] = GetBoneScreenCoords(ped, BONE_IDS[i]);
			}

			DrawSkeleton(points, connections, lineColor, thickness);
		}
	}
}

REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "ESP",
		.Id = "misc_esp",
		.IsTimed = true
	}
);