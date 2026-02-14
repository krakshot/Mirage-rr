#include "SYSWindow.hpp"
#include <string>
#include "../Utils/Logger/Logger.hpp"
#include "../Render/Overlay.hpp"
WNDPROC WindowHook::originalWndProc = nullptr;
HWND WindowHook::windowHandle = nullptr;

bool WindowHook::Initialize(HWND hWindow) {
    if (!hWindow) {
        Logger::Error("Invalid window handle provided");
        return false;
    }

    windowHandle = hWindow;
    originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHandler)));

    if (!originalWndProc) {
        Logger::Error("Failed to set window procedure");
        return false;
    }

    Logger::Debug("Window hook initialized successfully for handle: 0x" + std::to_string((uintptr_t)windowHandle));
    return true;
}

void WindowHook::Shutdown() {
    if (windowHandle && originalWndProc) {
        SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
        Logger::Debug("Window hook shutdown");
    }
}

LRESULT CALLBACK WindowHook::WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYUP && wParam == VK_DELETE) {

        Overlay::menuOpen = !Overlay::menuOpen;

        if (Overlay::menuOpen) {
            ImGui::GetIO().MouseDrawCursor = true;

        }
        else {
            ImGui::GetIO().MouseDrawCursor = false;
                    }

        return TRUE;
    }

    if (Overlay::menuOpen) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return TRUE;
    }

    return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}