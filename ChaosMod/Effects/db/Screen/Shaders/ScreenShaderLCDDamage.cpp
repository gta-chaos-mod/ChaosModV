#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"

static const char *ms_szShaderSrc = R"SRC(
Texture2D HDRSampler : register(t5);
SamplerState g_samLinear : register(s5)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 main(float4 position	: SV_POSITION, float3 texcoord : TEXCOORD0, float4 color : COLOR0) : SV_Target0
{
    float4 col = HDRSampler.Sample(g_samLinear, texcoord);

    col.r = saturate(max(texcoord.x % 1, col.r));
    col.g = saturate(max(texcoord.x % 1, col.g));
    col.b = saturate(max(texcoord.x % 1, col.b));

    return col;
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
		.Name = "LCD Damage",
		.Id = "screen_lcddamm",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);