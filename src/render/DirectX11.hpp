#pragma once
#include <d3d11.h>

class DirectX11 {
public:
    static bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    static void Shutdown();
    static void BeginRender();
    static void EndRender();

    static ID3D11Device* GetDevice();
    static ID3D11DeviceContext* GetDeviceContext();
    static void SetSwapChain(IDXGISwapChain* pSwapChain);

private:
    static void CleanupRenderTarget();
    static bool CreateRenderTarget();

    static ID3D11Device* g_pd3dDevice;
    static ID3D11DeviceContext* g_pd3dDeviceContext;
    static ID3D11RenderTargetView* g_mainRenderTargetView;
    static IDXGISwapChain* g_pSwapChain;
    static HWND g_hWindow;
    static bool initialized;
};
