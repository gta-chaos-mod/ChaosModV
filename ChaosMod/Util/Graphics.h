
#include "Natives.h"

namespace Util
{
	inline void DRAW_2D_BOX(float x, float y, float width, float height, CRGBA colour, float lineThickness)
	{
		float rx = x + (width / 2);
		float lx = x - (width / 2);

		// Y goes from top to bottom?
		float ty = y - (height / 2);
		float by = y + (height / 2);

		DRAW_RECT(lx, y, lineThickness, height, colour.R, colour.G, colour.B, colour.A, false); // Left Line
		DRAW_RECT(x, ty, width, lineThickness, colour.R, colour.G, colour.B, colour.A, false);  // Top Line
		DRAW_RECT(rx, y, lineThickness, height, colour.R, colour.G, colour.B, colour.A, false); // Right Line
		DRAW_RECT(x, by, width, lineThickness, colour.R, colour.G, colour.B, colour.A, false);  // Bottom Line
	}
}