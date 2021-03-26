#pragma once

enum class EffectGroup
{
	DEFAULT,
	TELEPORT,  // Effects which teleport the player potentially far away without (explicitly) being helpful
	SPAWN,     // Effects which spawn "generic" objects (like vehicles or props)
	PEDS,     // Effects which spawn friendly/enemy peds
	WEAPONS,     // Effects which give/remove weapons
	PLAYERKILL // Effects which are (almost) guaranteed to immediately kill the player under any circumstance
};