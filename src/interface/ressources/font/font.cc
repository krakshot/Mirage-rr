
#include "font.hh"
#include "../data.h"

namespace gui
{
    namespace font
    {
        static Fonts fonts_t;

        const Fonts& Get()
        {
            return fonts_t;
        }

        void Init()
        {
            ImGuiIO& io = ImGui::GetIO();

            ImFontConfig cfg;
            cfg.OversampleH = 2;
            cfg.OversampleV = 2;
            cfg.PixelSnapH = true;

            ImFont* custom_base = io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 13.0f, &cfg, io.Fonts->GetGlyphRangesDefault());
            ImFont* custom_title = io.Fonts->AddFontFromMemoryTTF((void*)tahomabd, sizeof(tahomabd), 16.0f, &cfg, io.Fonts->GetGlyphRangesDefault());

            fonts_t.base = custom_base ? custom_base : io.Fonts->AddFontDefault();
            fonts_t.title = custom_title ? custom_title : (fonts_t.base ? fonts_t.base : io.Fonts->AddFontDefault());

            io.Fonts->Build();
        }

        void Shutdown()
        {
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            fonts_t.base = nullptr;
            fonts_t.title = nullptr;
        }
    }
}
