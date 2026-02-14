#include "DirectX11.hpp"
#include "../System/SYSWindow.hpp"
#include "../../vendor/imgui/imgui_impl_dx11.h"
#include "../../vendor/imgui/imgui_impl_win32.h"
#include "../Utils/Logger/Logger.hpp"
#include "../Interface/interface/gui.hh"


ID3D11Device* DirectX11::g_pd3dDevice = nullptr;
ID3D11DeviceContext* DirectX11::g_pd3dDeviceContext = nullptr;
ID3D11RenderTargetView* DirectX11::g_mainRenderTargetView = nullptr;
IDXGISwapChain* DirectX11::g_pSwapChain = nullptr;
HWND DirectX11::g_hWindow = nullptr;
bool DirectX11::initialized = false;
static UINT s_lastBufferWidth = 0;
static UINT s_lastBufferHeight = 0;
static bool s_win32Initialized = false;

bool DirectX11::Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
	return true;
}

void DirectX11::Shutdown() {
	CleanupRenderTarget();

	if (g_pd3dDevice) {
		ImGui_ImplDX11_Shutdown();
		if (s_win32Initialized) {
			ImGui_ImplWin32_Shutdown();
			s_win32Initialized = false;
		}
		ImGui::DestroyContext();
	}

	if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
	if (g_pd3dDeviceContext) {
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = nullptr;
	}

	initialized = false;
}

void DirectX11::CleanupRenderTarget() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}

bool DirectX11::CreateRenderTarget() {
	if (!g_pSwapChain || !g_pd3dDevice)
		return false;

	CleanupRenderTarget();

	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (FAILED(hr) || !pBackBuffer)
		return false;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr) || !g_mainRenderTargetView)
		return false;
	return true;
}

void DirectX11::BeginRender() {
	if (initialized) {
		ImGui_ImplDX11_NewFrame();
		if (s_win32Initialized)
			ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		return;
	}

	if (!g_pd3dDevice && g_pSwapChain) {
		HRESULT hr = g_pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice));
		if (SUCCEEDED(hr) && g_pd3dDevice) {
			g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
			if (!g_pd3dDeviceContext) {
				Logger::Error("Failed to get immediate context from device");
				return;
			}
			DXGI_SWAP_CHAIN_DESC desc = {};
			if (FAILED(g_pSwapChain->GetDesc(&desc))) {
				Logger::Error("Failed to get swap chain desc");
				return;
			}
			g_hWindow = desc.OutputWindow;
			if (!g_hWindow) {
				Logger::Error("Failed to get window from swap chain");
				return;
			}


			if (!WindowHook::Initialize(g_hWindow)) {
				Logger::Error("Failed to initialize window hook");
				return;
			}

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			if (!ImGui_ImplWin32_Init(g_hWindow)) {
				Logger::Error("Failed to initialize ImGui Win32");
				return;
			}
			s_win32Initialized = true;

			if (!ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext)) {
				Logger::Error("Failed to initialize ImGui DX11");
				return;
			}

			if (!CreateRenderTarget()) {
				Logger::Error("Failed to create render target");
				return;
			}

			ImGui::StyleColorsDark();
			ngui->init();

			initialized = true;
			Logger::Debug("DirectX11 and ImGui fully initialized");
		}
	}

	if (initialized) {
		ImGui_ImplDX11_NewFrame();
		if (s_win32Initialized)
			ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
}

void DirectX11::EndRender() {
	if (!g_mainRenderTargetView || !initialized)
		return;

	// Recreate RTV on resize (base doesn't; keeps overlay working when window is resized)
	DXGI_SWAP_CHAIN_DESC scDesc = {};
	if (SUCCEEDED(g_pSwapChain->GetDesc(&scDesc)) && scDesc.BufferDesc.Width > 0 && scDesc.BufferDesc.Height > 0) {
		bool needRtv = !g_mainRenderTargetView ||
			(scDesc.BufferDesc.Width != s_lastBufferWidth || scDesc.BufferDesc.Height != s_lastBufferHeight);
		if (needRtv) {
			s_lastBufferWidth = scDesc.BufferDesc.Width;
			s_lastBufferHeight = scDesc.BufferDesc.Height;
			CreateRenderTarget();
		}
	}
	if (!g_mainRenderTargetView)
		return;

	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pd3dDeviceContext->Flush();
}

ID3D11Device* DirectX11::GetDevice() {
	return g_pd3dDevice;
}

ID3D11DeviceContext* DirectX11::GetDeviceContext() {
	return g_pd3dDeviceContext;
}

void DirectX11::SetSwapChain(IDXGISwapChain* pSwapChain) {
	if (pSwapChain && !g_pSwapChain) {
		g_pSwapChain = pSwapChain;
		Logger::Debug("SwapChain set: 0x" + std::to_string((uintptr_t)pSwapChain));
	}
}
