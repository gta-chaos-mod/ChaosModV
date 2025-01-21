#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/ShaderHook.h"

CHAOS_VAR const char *ms_szShaderSrcPrefix = R"SRC(
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
    float rotation = )SRC";

CHAOS_VAR const char *ms_szShaderSrcSuffix = R"SRC(;
    float sinA = sin(radians(rotation));
    float cosA = cos(radians(rotation));
    
    float val1 = cosA + (1.0 - cosA) / 3.0;
    float val2 = 1./3. * (1.0 - cosA) - sqrt(1./3.) * sinA;
    float val3 = 1./3. * (1.0 - cosA) + sqrt(1./3.) * sinA;
    float3x3 mat = {val1, val2, val3,
                    val3, val1, val2,
                    val2, val3, val1};
    float3x3 colMat = {col.r, col.r, col.r,
                        col.g, col.g, col.g,
                        col.b, col.b, col.b};
	float3x3 result = mul(mat, colMat);
    return float4(saturate(result[0][0]), saturate(result[1][1]), saturate(result[2][2]), 1.);
}
)SRC";

static void OnStart()
{
	Hooks::OverrideShader(OverrideShaderType::LensDistortion, ms_szShaderSrcPrefix
	                                                              + std::to_string(g_Random.GetRandomFloat(20.f, 340.f))
	                                                              + ms_szShaderSrcSuffix);
}

static void OnStop()
{
	Hooks::ResetShader();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr,  
    { 
        .Name            = "Hue Shift",
        .Id              = "screen_hueshift",
        .IsTimed         = true,
        .EffectCategory  = EffectCategory::Shader,
        .EffectGroupType = EffectGroupType::Shader
    }
);