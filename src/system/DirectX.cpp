#include "DirectX.hpp"
#include "../Render/DirectX11.hpp"
#include "../Render/Overlay.hpp"
#include "../Core/Application.hpp"
#include <d3d11.h>
#include "../../vendor/kiero/kiero.h"
#include "../Utils/Logger/Logger.hpp"



bool DirectXHook::hooked = false;
HRESULT(STDMETHODCALLTYPE* DirectXHook::PresentOriginal)(IDXGISwapChain*, UINT, UINT) = nullptr;

bool DirectXHook::Initialize() {
    if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) {
        Logger::Error("Failed to initialize Kiero");
        return false;
    }

    if (kiero::bind(8, (void**)&PresentOriginal, (void**)&PresentHook) == kiero::Status::Success) {
        hooked = true;
        Logger::Debug("DirectX11 hook initialized successfully with Kiero");
        return true;
    }
    Logger::Error("Failed to initialize DirectX11 hook with Kiero");
    return false;
}

void DirectXHook::Shutdown() {
    if (hooked) {
        kiero::shutdown();
        hooked = false;
        Logger::Debug("DirectX11 hook shutdown");
    }
}

HRESULT STDMETHODCALLTYPE DirectXHook::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool init = false;

    if (!init) {
        DirectX11::SetSwapChain(pSwapChain);
        init = true;
        Logger::Debug("SwapChain set in PresentHook");
    }

    static const bool kSkipAllDraw = false;
    if (!kSkipAllDraw) {
        DirectX11::BeginRender();
        Overlay::Render();
        DirectX11::EndRender();
    }

    if (PresentOriginal)
        return PresentOriginal(pSwapChain, SyncInterval, Flags);
    return S_OK;
}
