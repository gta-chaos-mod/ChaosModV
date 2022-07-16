#include <stdafx.h>

#include "Memory/Hooks/ShaderHook.h"
#include "Memory/Snow.h"

static const char *ms_szShaderSrc = R"SRC(
float4 main(float4 v0 : SV_Position0, float4 v1 : TEXCOORD0, float4 v2 : TEXCOORD1) : SV_Target0
{
    discard;

    return float4(1., 1., 1., 1.);
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
		.Name = "Colorful World",
		.Id = "screen_colorfulworld",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Shader,
		.EffectGroupType = EEffectGroupType::Shader
	}
);