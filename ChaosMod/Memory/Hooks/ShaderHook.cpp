#include <stdafx.h>

#include "ShaderHook.h"

#include "Memory/Shader.h"

#include "Memory/Hooks/Hook.h"
#include "Memory/Hooks/PresentHook.h"

#define SHADER_CACHE_MAX_ENTRIES 20

using DWORD64 = unsigned long long;

static std::vector<BYTE> ms_ShaderBytecode;
static OverrideShaderType ms_ShaderType;

static bool ms_RefreshShaders = false;

void *(*OG_rage__CreateShader)(const char *, BYTE *, DWORD, DWORD, DWORD *);
void *HK_rage__CreateShader(const char *name, BYTE *data, DWORD size, DWORD type, DWORD *out)
{
	if (!ms_ShaderBytecode.empty())
	{
		const char *nameFilter = nullptr;
		switch (ms_ShaderType)
		{
		case OverrideShaderType::LensDistortion:
			nameFilter = "PS_LensDistortion";
			break;
		case OverrideShaderType::Snow:
			nameFilter = "PS_snow";
			break;
		}

		if (nameFilter && std::string_view(name).find(nameFilter) != std::string_view::npos)
			return OG_rage__CreateShader(name, ms_ShaderBytecode.data(), ms_ShaderBytecode.size(), type, out);
	}

	return OG_rage__CreateShader(name, data, size, type, out);
}

static bool OnHook()
{
	static CHAOS_EVENT_LISTENER(Hooks::OnPresent) onPresentListener;

	onPresentListener.Register(Hooks::OnPresent,
	                           []()
	                           {
		                           if (ms_RefreshShaders)
		                           {
			                           ms_RefreshShaders = false;
			                           Memory::InvalidateShaderCache();
		                           }
	                           });

	Handle handle;

	//

	handle = Memory::FindPattern("E8 ? ? ? ? 48 89 83 28 02 00 00 8B 44 24 30 89 83 30 02 00 00 EB 0F");
	if (!handle.IsValid())
		return false;

	Memory::AddHook(handle.Into().Get<void>(), HK_rage__CreateShader, &OG_rage__CreateShader);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "ShaderHook", true);

namespace Hooks
{
	void OverrideShader(OverrideShaderType shaderType, std::string_view shaderSrc)
	{
		static std::unordered_map<size_t, std::vector<BYTE>> shaderCache;

		auto hash   = std::hash<std::string_view>()(shaderSrc);
		auto result = shaderCache.find(hash);
		if (result == shaderCache.end())
		{
			auto future = std::async(
			    std::launch::async,
			    [&]()
			    {
				    ID3DBlob *shader;
				    ID3DBlob *errorMessages;
				    HRESULT compileResult;
				    if ((compileResult = D3DCompile(shaderSrc.data(), shaderSrc.size(), NULL, NULL, NULL, "main",
				                                    "ps_4_0", 0, 0, &shader, &errorMessages))
				        == S_OK)
				    {
					    auto ptr = reinterpret_cast<BYTE *>(shader->GetBufferPointer());
					    std::vector<BYTE> shaderBytecode;
					    shaderBytecode.reserve(shader->GetBufferSize());
					    std::copy(ptr, ptr + shader->GetBufferSize(), std::back_inserter(shaderBytecode));

					    if (shaderCache.size() > SHADER_CACHE_MAX_ENTRIES)
						    shaderCache.erase(shaderCache.begin());

					    shaderCache[hash] = shaderBytecode;
					    result            = shaderCache.find(hash);
				    }
				    else
				    {
					    LOG("Error compiling shader: Error code 0x" << std::hex << std::uppercase << compileResult  << std::dec);
					    if (errorMessages)
					    {
						    auto ptr = reinterpret_cast<BYTE *>(errorMessages->GetBufferPointer());
						    std::string buffer;
						    buffer.reserve(errorMessages->GetBufferSize());
						    std::copy(ptr, ptr + errorMessages->GetBufferSize(), std::back_inserter(buffer));

						    LOG(buffer);
						    return;
					    }
				    }
			    });

			using namespace std::chrono_literals;
			while (future.wait_for(0ms) != std::future_status::ready)
				WAIT(0);
		}

		if (result != shaderCache.end())
		{
			ms_ShaderBytecode = result->second;
			ms_ShaderType     = shaderType;
			ms_RefreshShaders = true;
		}
	}

	void ResetShader()
	{
		ms_ShaderBytecode.clear();
		ms_RefreshShaders = true;
	}
}