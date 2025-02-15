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
    return HDRSampler.Sample(g_samLinear, float2(1. - texcoord.x, texcoord.y));
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
		.Name = "Mirrored Screen",
		.Id = "screen_mirrored",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Shader,
		.EffectGroupType = EffectGroupType::Shader
	}
);