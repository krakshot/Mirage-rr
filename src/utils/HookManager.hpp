#pragma once

#define POLYHOOK_STATIC_LIB
#define ZYDIS_STATIC_BUILD  
#define ASMJIT_STATIC

#include <PolyHook2/Detour/x64Detour.hpp>
#include <memory>
#include <unordered_map>

class HookManager {
public:
    static HookManager* GetInstance();
    static void Initialize();

    bool AddInline(uint64_t target, uint64_t hook, uint64_t* original);

    template<typename T>
    bool AddInline(uint64_t target, T hook, T* original) {
        return AddInline(target, reinterpret_cast<uint64_t>(hook), reinterpret_cast<uint64_t*>(original));
    }

    bool RemoveHook(uint64_t target);
    void RemoveAll();

    ~HookManager() = default;

private:
    HookManager() = default;
    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

    std::unordered_map<uint64_t, std::unique_ptr<PLH::x64Detour>> m_hooks;
};

extern HookManager* pHookManager;