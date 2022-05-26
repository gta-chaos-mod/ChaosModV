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
    texcoord.y = (texcoord.y * 2) - 0.5;
    texcoord.x = (texcoord.x * 2) - 0.5;

    float4 col = HDRSampler.Sample(g_samLinear, texcoord);
    col = min(col, 1 - 255 * (texcoord.x - 0) * (texcoord.x - 1));
    col = min(col, 1 - 255 * (texcoord.y - 0) * (texcoord.y - 1));

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
		.Name = "Low Resolution",
		.Id = "screen_lowres",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader,
		.IncompatibleWith = { "misc_dvdscreensaver" }
	}
);