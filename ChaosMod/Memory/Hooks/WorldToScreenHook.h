#pragma once

struct Vec2
{
	float x, y;
};

struct Vec3
{
	float x, y, z;
};

struct Resolution
{
	uint32_t Width, Height;
};

namespace Hooks
{
	bool WorldToScreen(Vector3 worldPosition, Vec2 *screenPosition);
}