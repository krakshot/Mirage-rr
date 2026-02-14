#include "CrashHandler.hpp"
#include "../Utils/Logger/Logger.hpp"
#include <windows.h>
#include <dbghelp.h>
#include <string>
#include <chrono>
#include <sstream>


#define CRASH_HANDLER_WRITE_MINIDUMP 0

static BOOL WriteMiniDump(EXCEPTION_POINTERS* pExceptionPointers)
{
#if !CRASH_HANDLER_WRITE_MINIDUMP
    (void)pExceptionPointers;
    return FALSE;
#else
    HMODULE hDbg = LoadLibraryA("dbghelp.dll");
    if (!hDbg) return FALSE;

    typedef BOOL(WINAPI* MiniDumpWriteDump_t)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);
    auto pMiniDumpWriteDump = (MiniDumpWriteDump_t)GetProcAddress(hDbg, "MiniDumpWriteDump");
    if (!pMiniDumpWriteDump) return FALSE;

    char tmpPath[MAX_PATH] = {0};
    if (!GetTempPathA(MAX_PATH, tmpPath)) return FALSE;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tmLocal{};
    localtime_s(&tmLocal, &t);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y%m%d_%H%M%S", &tmLocal);

    std::string dumpName = std::string(tmpPath) + "mirage_crash_" + timebuf + ".dmp";

    HANDLE hFile = CreateFileA(dumpName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    MINIDUMP_EXCEPTION_INFORMATION mei;
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = pExceptionPointers;
    mei.ClientPointers = TRUE;

    BOOL ok = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory), &mei, nullptr, nullptr);

    CloseHandle(hFile);

    if (ok) {
        Logger::Error(std::string("Wrote minidump to: ") + dumpName);
    } else {
        Logger::Error("Failed to write minidump");
    }

    return ok;
#endif
}

static LONG WINAPI UnhandledFilter(EXCEPTION_POINTERS* pExceptionPointers)
{
    WriteMiniDump(pExceptionPointers);
    return EXCEPTION_EXECUTE_HANDLER;
}

void CrashHandler::Initialize()
{
    SetUnhandledExceptionFilter(UnhandledFilter);
    AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)[](PEXCEPTION_POINTERS ep) -> LONG {
        if (ep && ep->ExceptionRecord && ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
            WriteMiniDump(ep);
        }
        return EXCEPTION_CONTINUE_SEARCH;
    });
    Logger::Debug("Crash handler installed");
}
