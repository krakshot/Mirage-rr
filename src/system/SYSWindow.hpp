#pragma once
#include <Windows.h>
#include "../../vendor/imgui/imgui.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WindowHook {
public:
    static bool Initialize(HWND hWindow); 
    static void Shutdown();
    static LRESULT CALLBACK WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    static WNDPROC originalWndProc;
    static HWND windowHandle;
};