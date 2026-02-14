#pragma once
#include "shared.h"
#include <cstdio>


namespace km {
    
    inline HANDLE driver_handle = nullptr;
    inline DWORD last_error = 0;
    
    inline bool try_connect() {
        if (driver_handle) return true;
        
        HANDLE h = CreateFileW(
            L"\\\\.\\sickmadriver",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );
        
        if (h != INVALID_HANDLE_VALUE) {
            driver_handle = h;
            return true;
        }
        
        last_error = GetLastError();
        return false;
    }
    
    inline bool initialize(int max_retries = 30, int retry_ms = 500) {
        for (int i = 0; i < max_retries; i++) {
            if (try_connect()) {
                return true;
            }
            
            if (last_error != ERROR_FILE_NOT_FOUND) {
                return false;
            }
            
            Sleep(retry_ms);
        }
        
        return false;
    }
    
    inline bool ensure_connected() {
        if (driver_handle) return true;
        return initialize(5, 200);
    }
    
    inline void cleanup() {
        if (driver_handle) {
            CloseHandle(driver_handle);
            driver_handle = nullptr;
        }
    }
    
    template<typename T>
    T read(ULONG process_id, uintptr_t address) {
        T buffer = {};
        
        if (!ensure_connected()) return buffer;
        
        km_request_t request = {};
        request.process_id = process_id;
        request.address = address;
        request.size = sizeof(T);
        
        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            read_km_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            if (request.success) {
                memcpy(&buffer, request.data, sizeof(T));
            }
        }
        
        return buffer;
    }
    
    template<typename T>
    bool write(ULONG process_id, uintptr_t address, T value) {
        if (!ensure_connected()) return false;
        
        km_request_t request = {};
        request.process_id = process_id;
        request.address = address;
        request.size = sizeof(T);
        memcpy(request.data, &value, sizeof(T));
        
        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            write_km_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            return request.success;
        }
        
        return false;
    }

    inline bool read_buffer(ULONG process_id, uintptr_t address, void* buffer, size_t size) {
        if (!ensure_connected() || size > 256) return false;
        
        km_request_t request = {};
        request.process_id = process_id;
        request.address = address;
        request.size = static_cast<ULONG>(size);
        
        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            read_km_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            if (request.success) {
                memcpy(buffer, request.data, size);
                return true;
            }
        }
        
        return false;
    }

    inline bool write_buffer(ULONG process_id, uintptr_t address, void* buffer, size_t size) {
        if (!ensure_connected() || size > 256) return false;
        
        km_request_t request = {};
        request.process_id = process_id;
        request.address = address;
        request.size = static_cast<ULONG>(size);
        memcpy(request.data, buffer, size);
        
        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            write_km_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            return request.success;
        }
        
        return false;
    }

    inline uintptr_t allocate(ULONG process_id, SIZE_T size, ULONG protect) {
        if (!ensure_connected()) return 0;

        alloc_request_t request = {};
        request.process_id = process_id;
        request.size = (ULONG64)size;
        request.protect = protect;

        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            alloc_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            if (request.success)
                return (uintptr_t)request.result;
        }
        return 0;
    }

    inline bool free_memory(ULONG process_id, uintptr_t address) {
        if (!ensure_connected()) return false;

        free_request_t request = {};
        request.process_id = process_id;
        request.address = (ULONG64)address;

        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            free_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            return request.success;
        }
        return false;
    }

    inline ULONG protect(ULONG process_id, uintptr_t address, SIZE_T size, ULONG new_protect) {
        if (!ensure_connected()) return 0;

        protect_request_t request = {};
        request.process_id = process_id;
        request.address = (ULONG64)address;
        request.size = (ULONG64)size;
        request.protection = new_protect;

        DWORD bytes_returned = 0;
        if (DeviceIoControl(
            driver_handle,
            protect_code,
            &request,
            sizeof(request),
            &request,
            sizeof(request),
            &bytes_returned,
            nullptr
        )) {
            return request.old_protection;
        }
        return 0;
    }
    
    inline bool is_initialized() {
        return driver_handle != nullptr;
    }
}
