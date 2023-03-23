#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"
#include "Memory/Snow.h"

static const char *ms_szShaderSrc = R"SRC(
cbuffer misc_globals : register(b2)
{
    float4 globalFade;                 // Offset:    0 Size:    16 [unused]
    float globalHeightScale;           // Offset:   16 Size:     4 [unused]
    float globalShaderQuality;         // Offset:   20 Size:     4 [unused]
    float globalReuseMe00001;          // Offset:   24 Size:     4 [unused]
    float globalReuseMe00002;          // Offset:   28 Size:     4 [unused]
    float4 POMFlags;
    float4 g_Rage_Tessellation_CameraPosition;// Offset:   32 Size:    16 [unused]
    float4 g_Rage_Tessellation_CameraZAxis;// Offset:   48 Size:    16 [unused]
    float4 g_Rage_Tessellation_ScreenSpaceErrorParams;// Offset:   64 Size:    16 [unused]
    float4 g_Rage_Tessellation_LinearScale;// Offset:   80 Size:    16 [unused]
    float4 g_Rage_Tessellation_Frustum[4];// Offset:   96 Size:    64 [unused]
    float4 g_Rage_Tessellation_Epsilons;// Offset:  160 Size:    16 [unused]
    float4 globalScalars;              // Offset:  176 Size:    16 [unused]
    float4 globalScalars2;             // Offset:  192 Size:    16 [unused]
    float4 globalScalars3;             // Offset:  208 Size:    16
    float4 globalScreenSize;           // Offset:  224 Size:    16 [unused]
    uint4 gTargetAAParams;             // Offset:  240 Size:    16 [unused]
    float4 colorize;                   // Offset:  256 Size:    16 [unused]
    float4 gGlobalParticleShadowBias;  // Offset:  272 Size:    16 [unused]
    float gGlobalParticleDofAlphaScale;// Offset:  288 Size:     4 [unused]
    float gGlobalFogIntensity;         // Offset:  292 Size:     4 [unused]
    float4 gPlayerLFootPos;            // Offset:  304 Size:    16 [unused]
    float4 gPlayerRFootPos;            // Offset:  320 Size:    16 [unused]
};

float rgb(float mult)
{
    return (sin(globalScalars.x * globalScalars2.x * mult) + 1) * 0.5;
}

float4 main(float4 v0 : SV_Position0, float4 v1 : TEXCOORD0, float4 v2 : TEXCOORD1) : SV_Target0
{
    return float4(rgb(v2.x * 0.00002 + 0.025), rgb(v2.y * 0.00002 + 0.05), rgb(v2.x * v2.y * 0.0002 + 0.1), 1.);
}
)SRC";

static void OnStart()
{
	Hooks::OverrideShader(EOverrideShaderType::Snow, ms_szShaderSrc);
}

static void OnStop()
{
	Memory::SetSnow(false);
	Hooks::ResetShader();
}

static void OnTick()
{
	Memory::SetSnow(true);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "RGB Land",
		.Id = "screen_rgbland",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);