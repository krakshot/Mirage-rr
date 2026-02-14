#include "Application.hpp"
#include "../System/DirectX.hpp"
#include "../System/SYSWindow.hpp"
#include "../Render/Overlay.hpp"
#include "../Utils/Logger/Logger.hpp"
#include "../../includes/unity/backend/main/bExports.h"
#include <thread>
#include <atomic>
#include <Windows.h>
#include <memory.hh>
#include <struct/Il2CppDomain.h>
#include <bypass/Bypass.hh>
#include "../utils/CrashHandler.hpp"
#include <cstdlib>

bool Application::IsRunning = false;

bool Application::Initialize() {
	Logger::Initialize();
	CrashHandler::Initialize();
	Logger::Debug("Initializing Supremacy...");

	if (!DirectXHook::Initialize()) {
		Logger::Error("Failed to initialize DirectX hook");
		return false;
	}


	var::mainSpoof = memory::findPattern("GameAssembly.dll", "\xFF\x23", "xx");
	var::unitySpoof = memory::findPattern("UnityPlayer.dll", "\xFF\x23", "xx");
	Logger::Success("SpoofSetup");


	Pointers::Exports.Setup(GetModuleHandleA("GameAssembly.dll"));
	Logger::Success("Pointer setup complete");


	auto domain = IL2CPP::Il2CppDomain::Get();
	if (!domain) {
		Logger::Debug("Failed to get IL2CPP domain");
	}
	else {
		Logger::Success("IL2CPP domain obtained");

		if (Pointers::Exports.il2cpp_thread_attach) {
			auto thread = domain->ThreadAttach();
			Logger::Success("Thread attached to IL2CPP domain");
		}
		else {
			Logger::Debug("ThreadAttach skipped (il2cpp_thread_attach not resolved)");
		}

	}


	Bypass::Setup();
	Logger::Success("Bypass setup complete");

	IsRunning = true;
	Logger::Debug("Supremacy initialized successfully");

	return true;
}

void Application::Shutdown() {
	Logger::Debug("Shutting down Supremacy...");

	DirectXHook::Shutdown();
	WindowHook::Shutdown();

	IsRunning = false;
	Logger::Debug("Supremacy shutdown complete");
}




void Application::MainLoop() {
}
