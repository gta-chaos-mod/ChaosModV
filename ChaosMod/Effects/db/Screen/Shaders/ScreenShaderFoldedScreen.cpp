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
)SRC";

static void OnStart()
{
	// Build shader source
	std::string shaderSrcSuffix;
	switch (g_Random.GetRandomInt(0, 3))
	{
	case 0:
		shaderSrcSuffix = "return HDRSampler.Sample(g_samLinear, float2(lerp(texcoord.x >= 0.5, texcoord.x < 0.5, "
						  "texcoord.x), texcoord.y));}";
		break;
	case 1:
		shaderSrcSuffix = "return HDRSampler.Sample(g_samLinear, float2(lerp(texcoord.x < 0.5, texcoord.x >= 0.5, "
						  "texcoord.x), texcoord.y));}";
		break;
	case 2:
		shaderSrcSuffix =
			"return HDRSampler.Sample(g_samLinear, float2(texcoord.x, lerp(texcoord.y >= 0.5, texcoord.y < 0.5, "
			"texcoord.y)));}";
		break;
	case 3:
		shaderSrcSuffix =
			"return HDRSampler.Sample(g_samLinear, float2(texcoord.x, lerp(texcoord.y < 0.5, texcoord.y >= 0.5, "
			"texcoord.y)));}";
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
		.Name = "Folded Screen",
		.Id = "screen_foldedscreen",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);