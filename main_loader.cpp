// LoLMetaDumperInternal
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <thread>
#define EXPORT __declspec(dllexport)
#define NO_DLL_MAIN
#include "main_internal.cpp"

struct _EXCEPTION_POINTERS;
class MiniDmpSender {
public:
    char data[16];
    enum _BS_MINIDUMP_TYPE {};
    EXPORT virtual ~MiniDmpSender() {}
    EXPORT void setMiniDumpType(_BS_MINIDUMP_TYPE) {}
    EXPORT void resetAppIdentifier(wchar_t const *) {}
    EXPORT void sendAdditionalFile(wchar_t const *) {}
    EXPORT void setLogFilePath(wchar_t const *) {}
    EXPORT long unhandledExceptionHandler(_EXCEPTION_POINTERS *) { return 0; }
    EXPORT void setCallback(bool(__cdecl *)(unsigned int, void *, void *)) {}
    EXPORT MiniDmpSender(wchar_t const *, wchar_t const *, wchar_t const *, wchar_t const *,
                         unsigned long) {
        dump();
        // AllocConsole();
        // freopen("CONOUT$", "w", stdout);
        exit(0);
    }
    EXPORT MiniDmpSender(MiniDmpSender const &) {}
};
