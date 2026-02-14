#include "EntryPoint.hpp"
#include "Application.hpp"
#include <iostream>
#include <Windows.h>
#include "../Utils/HookManager.hpp"
#include "../Utils/Logger/Logger.hpp"
#include "../../includes/driver/km.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        //Logger::Initialize();
        if(km::try_connect()) {
            Logger::Success("driver initialized successfully");  
		}
        else {
            Logger::Debug("driver not found yet, will retry in background thread");
        }

            HookManager::Initialize();


        CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
                if (!km::is_initialized()) {
                    if (km::initialize(30, 500)) {
                        Logger::Success("driver connected (deferred)");
                    } else {
                        char errbuf[128];
                        sprintf_s(errbuf, "failed to connect to driver after retries (error: %lu)", km::last_error);
                        Logger::Error(errbuf);
                    }
                }
                EntryPoint::Initialize();

            return 0;
        }, hModule, 0, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH) {
        EntryPoint::Shutdown();

    }
    return TRUE;
}

void EntryPoint::Initialize() {
    Application::Initialize();
}

void EntryPoint::Shutdown() {
    Application::Shutdown();
    km::cleanup();
}
