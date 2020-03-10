#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <codecvt>

#include "mem.hpp"
#include "meta.hpp"
#include "version.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winnt.h>
#include <Psapi.h>

static uintptr_t m_base;
static size_t m_size;
static HANDLE process;

int InvokeContainerI(uintptr_t fnptr) noexcept {
    return -2;
}

int InvokeMapI(uintptr_t fnptr) noexcept {
    return -2;
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

static DWORD FindLeague() noexcept {
    DWORD processes[1024] = {};
    DWORD processesNeded = {};
    HMODULE modules[1] = {};
    DWORD modulesNeded = {};
    char name[256] = {};

    if (!EnumProcesses(processes, sizeof(processes), &processesNeded)) {
        return 0;
    }

    for (unsigned int i = 0; i < (processesNeded / sizeof(DWORD)); i++) {
        if (processes[i] != 0) {
            auto const pid = processes[i];
            auto const handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (handle == nullptr) {
                continue;
            }
            if (EnumProcessModules(handle, modules, sizeof(modules), &modulesNeded)) {
                GetModuleBaseNameA(handle, modules[0], name, sizeof(name));

                if (strstr(name, "League of Legends.exe")) {
                    CloseHandle(handle);
                    return pid;
                }
            }
            CloseHandle(handle);
        }
    }

    return 0;
}

static bool SetBaseAndSize() noexcept {
    HMODULE mods[1024];
    DWORD bmsize;
    if (!EnumProcessModules(process, mods, sizeof(mods), &bmsize)) {
        return false;
    }
    auto const mend = mods + (bmsize / sizeof(HMODULE));
    for (auto m = mods; m < mend; m++) {
        char name[MAX_PATH];
        if (GetModuleFileNameExA(process, *m, name, sizeof(name))) {
            if (strstr(name, "League of Legends.exe") != nullptr) {
                m_base = reinterpret_cast<uintptr_t>(*m);
                m_size = 80 * 1024 * 1024;
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char const** args) {
    auto const pid = (argc > 1 && !strstr(args[1], "-")) ? atoi(args[1]) : FindLeague();
    if (pid == 0) {
        return 0;
    }
    
    if (!OpenLeague(pid)) {
        return 0;
    }

    if (!SetBaseAndSize()) {
        KillLeague();
        return 0;
    }

    auto const data = Mem::Dump();
    auto const version = Version::Dump(data);
    
    if (version.empty()) {
        KillLeague();
    }

    auto const meta = Meta::Dump(data, version);

    if ((argc > 1 && strstr(args[1], "-")) || (argc > 2 && strstr(args[2], "-"))) {
        std::cout << meta.dump(2);
    } else {
        if (std::ofstream out("meta/meta_" + version + ".json"); out) {
            out << meta.dump(2);
        }
    }

    KillLeague();
    return 0;
}
