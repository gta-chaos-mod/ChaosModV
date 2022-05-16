#pragma once

#include <string_view>

class IDXGISwapChain;

namespace Hooks
{
    void OverrideScreenShader(std::string_view szShaderSrc);
    void ResetScreenShader();
    void OnPresentShaderHook(IDXGISwapChain* pSwapChain);
}