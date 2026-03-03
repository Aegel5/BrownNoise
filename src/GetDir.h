
#ifdef _WIN32
#include <windows.h>
#endif

fs::path getExeDirectory() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
#else
    // Для Linux (на всякий случай)
    return fs::canonical("/proc/self/exe").parent_path();
#endif
}

