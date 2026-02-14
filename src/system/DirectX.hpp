#pragma once
#include <d3d11.h>

class DirectXHook {
public:
    static bool Initialize();
    static void Shutdown();

    static HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);

private:
    static bool hooked;
    static HRESULT(STDMETHODCALLTYPE* PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
};