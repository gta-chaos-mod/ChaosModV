#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"

static const char *ms_ShaderSrc = R"SRC(
Texture2D HDRSampler : register(t5);
SamplerState g_samLinear : register(s5)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 main(float4 position	: SV_POSITION, float3 texcoord : TEXCOORD0, float4 color : COLOR0) : SV_Target0
{
	texcoord.x = saturate(texcoord.x % (0.5 - texcoord.x));
    float4 col = HDRSampler.Sample(g_samLinear, texcoord);

    return col;
}
)SRC";

static void OnStart()
{
	Hooks::OverrideShader(OverrideShaderType::LensDistortion, ms_ShaderSrc);
}

static void OnStop()
{
	Hooks::ResetShader();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Dimension Warp",
		.Id = "screen_dimwarp",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Shader,
		.EffectGroupType = EffectGroupType::Shader
	}
);