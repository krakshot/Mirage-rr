#pragma once

class Application {
public:
    static bool Initialize();
    static void Shutdown();
    static void MainLoop();


    static bool IsRunning;
};