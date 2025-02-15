#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/ShaderHook.h"

CHAOS_VAR const char *ms_ShaderSrc = R"SRC(
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
    col.r = saturate(max(col.r, texcoord.x - col.r));
    col.g = saturate(max(col.g, texcoord.x - col.g - 0.2));
    col.b = saturate(max(col.b, texcoord.x - col.b - 0.4));

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
REGISTER_EFFECT(OnStart, OnStop, nullptr, 
	{
		.Name = "TN Panel",
		.Id = "screen_tnpanel",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Shader,
		.EffectGroupType = EffectGroupType::Shader
	}
);