#include <stdafx.h>

#include "Memory/Hooks/Hook.h"
#include "Memory/Hooks/ShaderHook.h"

static void **ms_PresentAddr = nullptr;

HRESULT (*OG_IDXGISwapChain_Present)(IDXGISwapChain *, UINT, UINT);
HRESULT HK_IDXGISwapChain_Present(IDXGISwapChain *swapChain, UINT syncInterval, UINT flags)
{
	if (!(flags & DXGI_PRESENT_TEST))
	{
		Hooks::OnPresentCallback();
	}

	return OG_IDXGISwapChain_Present(swapChain, syncInterval, flags);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("80 7E 10 00 48 8B");
	if (!handle.IsValid())
	{
		return false;
	}

	// IDXGISwapChain
	handle                    = *handle.At(6).Into().Value<DWORD64 *>();

	ms_PresentAddr            = handle.At(64).Get<void *>();
	OG_IDXGISwapChain_Present = *(HRESULT(**)(IDXGISwapChain *, UINT, UINT))ms_PresentAddr;
	Memory::Write<void *>(ms_PresentAddr, HK_IDXGISwapChain_Present);

	return true;
}

static void OnCleanup()
{
	// Only reset vftable entries if address still points to our retour
	if (ms_PresentAddr && *ms_PresentAddr == HK_IDXGISwapChain_Present)
	{
		Memory::Write<void *>(ms_PresentAddr, OG_IDXGISwapChain_Present);
	}
}

static RegisterHook registerHook(OnHook, OnCleanup, "IDXGISwapChain::Present", true);