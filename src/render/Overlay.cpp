#include "Overlay.hpp"

#include "DirectX11.hpp"
#include "../../vendor/imgui/imgui_impl_dx11.h"
#include "../../vendor/imgui/imgui_impl_win32.h"
#include "../Interface/interface/gui.hh"
#include <exception>
#include "../utils/logger/Logger.hpp"



bool Overlay::Initialize() {
    return true;
}

void Overlay::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Overlay::Render() {


    ngui->render();

#ifdef TEST

    ngui->renderLogs();
    //ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
    //ImGui::Begin("Debug Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    //{
    //    ImGui::Text("this bs works!!!!");
    //    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    //}
    //ImGui::End();
#endif

}

void Overlay::NewFrame() {}

void Overlay::EndFrame() {}
