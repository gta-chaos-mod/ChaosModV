#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"

static const char *ms_szShaderSrcPrefix = R"SRC(
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
)SRC";

static void OnStart()
{
	// Build shader source
	std::string shaderSrcSuffix;
	switch (g_Random.GetRandomInt(0, 4))
	{
	case 0:
		shaderSrcSuffix = "col.rgb = col.brg;return col;}";
		break;
	case 1:
		shaderSrcSuffix = "col.rgb = col.grb;return col;}";
		break;
	case 2:
		shaderSrcSuffix = "col.rgb = col.rbg;return col;}";
		break;
	case 3:
		shaderSrcSuffix = "col.rgb = col.bgr;return col;}";
		break;
	case 4:
		shaderSrcSuffix = "col.rgb = col.gbr;return col;}";
		break;
	}

	if (!shaderSrcSuffix.empty())
	{
		Hooks::OverrideShader(EOverrideShaderType::LensDistortion, ms_szShaderSrcPrefix + shaderSrcSuffix);
	}
}

static void OnStop()
{
	Hooks::ResetShader();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Swapped Colors",
		.Id = "screen_swappedcolors",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);