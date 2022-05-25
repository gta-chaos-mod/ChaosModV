#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"

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

Texture2D HDRSampler : register(t5);
SamplerState g_samLinear : register(s5)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 main(float4 position	: SV_POSITION, float3 texcoord : TEXCOORD0, float4 color : COLOR0) : SV_Target0
{
    float time = globalScalars.x * globalScalars2.x;
    float mod = sin(time) * 0.05;
    return HDRSampler.Sample(g_samLinear, float2(texcoord.x + texcoord.x % mod, texcoord.y + texcoord.y % mod));
}
)SRC";

static void OnStart()
{
	Hooks::OverrideShader(EOverrideShaderType::LensDistortion, ms_szShaderSrc);
}

static void OnStop()
{
	Hooks::ResetShader();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Screen Freakout",
		.Id = "screen_screenfreakout",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);