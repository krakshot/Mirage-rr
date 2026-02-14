#pragma once
#include <windows.h>
#include <winioctl.h>
#include <cstdint>


constexpr ULONG init_code      = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG read_code      = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG write_code     = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG read_km_code   = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x778, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG write_km_code  = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x779, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG alloc_code     = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x77A, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG free_code      = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x77B, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG protect_code   = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x77C, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);


struct rw_request_t {
    HANDLE target_pid;
    void*  target_address;
    void*  buffer_address;
    SIZE_T size;
    SIZE_T return_size;
};


struct km_request_t {
    ULONG         process_id;
    ULONG64       address;
    ULONG         size;
    unsigned char data[256];
    BOOLEAN       success;
};


struct alloc_request_t {
    ULONG   process_id;
    ULONG64 size;
    ULONG   protect;
    ULONG64 result;
    BOOLEAN success;
};

struct free_request_t {
    ULONG   process_id;
    ULONG64 address;
    BOOLEAN success;
};

struct protect_request_t {
    ULONG   process_id;
    ULONG64 address;
    ULONG64 size;
    ULONG   protection;
    ULONG   old_protection;
    BOOLEAN success;
};
