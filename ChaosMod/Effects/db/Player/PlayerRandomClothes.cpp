#include <stdafx.h>

static void OnStart()
{
	for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
	{
		Ped playerPed = PLAYER_PED_ID();

		for (int j = 0; j < 12; j++)
		{
			int drawableAmount = GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(playerPed, j);
			int drawable = drawableAmount == 0 ? 0 : g_Random.GetRandomInt(0, drawableAmount - 1);

			int textureAmount = GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(playerPed, j, drawable);
			int texture = textureAmount == 0 ? 0 : g_Random.GetRandomInt(0, textureAmount - 1);

			SET_PED_COMPONENT_VARIATION(playerPed, j, drawable, texture, g_Random.GetRandomInt(0, 3));

			if (j < 4)
			{
				int propDrawableAmount = GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(playerPed, j);
				int propDrawable = propDrawableAmount == 0 ? 0 : g_Random.GetRandomInt(0, propDrawableAmount - 1);

				int propTextureAmount = GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(playerPed, j, drawable);
				int propTexture = propTextureAmount == 0 ? 0 : g_Random.GetRandomInt(0, propTextureAmount - 1);

				SET_PED_PROP_INDEX(playerPed, j, propDrawable, propTexture, true);
			}
		}

		if (i + 1 < g_MetaInfo.m_fChaosMultiplier)
		{
			WAIT(3500);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_RANDOMCLOTHES, OnStart, EffectInfo
	{
		.Name = "Randomize Player Clothes",
		.Id = "player_randclothes"
	}
);