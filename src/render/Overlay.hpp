#pragma once

class Overlay {
public:
    static bool Initialize();
    static void Shutdown();
    static void Render();
    static void NewFrame();
    static void EndFrame();
    static inline bool menuOpen = false;
};