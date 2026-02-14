#include "HookManager.hpp"
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <stdexcept>

#include "HookManager.hpp"
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <stdexcept>

HookManager* pHookManager = nullptr;

void HookManager::Initialize() {
    if (!pHookManager) {
        pHookManager = new HookManager();
    }
}

HookManager* HookManager::GetInstance() {
    if (!pHookManager) {
        Initialize();
    }
    return pHookManager;
}

bool HookManager::AddInline(uint64_t target, uint64_t hook, uint64_t* original) {
    if (!pHookManager) return false;

    RemoveHook(target);

    auto detour = std::make_unique<PLH::x64Detour>(target, hook, original);

    if (!detour->hook()) {
        std::cout << "[HookManager] Failed to hook 0x" << std::hex << target << std::dec << std::endl;
        return false;
    }

    m_hooks[target] = std::move(detour);
    std::cout << "[HookManager] Successfully hooked 0x" << std::hex << target << std::dec << std::endl;
    return true;
}

bool HookManager::RemoveHook(uint64_t target) {
    if (!pHookManager) return false;

    auto it = m_hooks.find(target);
    if (it == m_hooks.end()) {
        return false;
    }

    it->second->unHook();
    m_hooks.erase(it);
    std::cout << "[HookManager] Removed hook from 0x" << std::hex << target << std::dec << std::endl;
    return true;
}

void HookManager::RemoveAll() {
    if (!pHookManager) return;

    for (auto& pair : m_hooks) {
        pair.second->unHook();
    }
    m_hooks.clear();
    std::cout << "[HookManager] Removed all hooks" << std::endl;
}