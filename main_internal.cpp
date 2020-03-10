#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "mem.hpp"
#include "meta.hpp"
#include "version.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winnt.h>
#include <Psapi.h>

static size_t m_size;
static uintptr_t m_base;
static HANDLE process;

int InvokeContainerI(uintptr_t fnptr) noexcept {
    auto const fn = reinterpret_cast<int(__stdcall *)()>(fnptr);
    return fn();
}

int InvokeMapI(uintptr_t fnptr) noexcept {
    auto const fn = reinterpret_cast<int(__stdcall *)(uintptr_t*)>(fnptr);
    uintptr_t dummy[8] = { 0, 0x78000000, 1 };
    return fn(dummy);
}

uintptr_t GetBase() noexcept {
    return m_base;
}

size_t GetSize() noexcept {
    return m_size;
}

void ReadMemory(uintptr_t src, void* dst, size_t size) noexcept {
    ReadProcessMemory(process,
        reinterpret_cast<void const*>(src),
        dst,
        size,
        nullptr);
}

static bool OpenLeague(DWORD pid) noexcept {
    process = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE,
        false,
        pid);
    return process != nullptr && process != INVALID_HANDLE_VALUE;
}

static void KillLeague() noexcept {
    TerminateProcess(process, 0);
}

static void dump() noexcept {
    m_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("League of Legends.exe"));
    m_size = 80 * 1024 * 1024;

    if (m_base == 0) {
        return;
    }

    auto const pid = GetCurrentProcessId();

    if (!OpenLeague(pid)) {
        return;
    }

    auto const data = Mem::Dump();
    auto const version = Version::Dump(data);

    if (version.empty()) {
        KillLeague();
    }

    std::filesystem::create_directory("meta");
    auto const meta = Meta::Dump(data, version);
    if (std::ofstream out("meta/meta_" + version + ".json"); out) {
        out << meta.dump(2);
        out.flush();
    }

    KillLeague();
}

#ifndef NO_DLL_MAIN
BOOL WINAPI DllMain(HANDLE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        dump();
    }
    return TRUE;
}
#endif
