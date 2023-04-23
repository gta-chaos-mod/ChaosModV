#pragma once

#include "Util/Color.h"

namespace Hooks
{
	void DrawLine(float x1, float y1, float x2, float y2, float width, int r, int g, int b, int a);

	inline void DrawLine(Vec2 firstPos, Vec2 secondPos, Color color, float thickness)
	{
		DrawLine(firstPos.x, firstPos.y, secondPos.x, secondPos.y, thickness, color.R, color.G, color.B, color.A);
	}
}