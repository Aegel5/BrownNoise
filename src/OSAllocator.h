#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h> // Для полноты
#endif

[[nodiscard]] inline void* OS_AllocateForever(size_t size) {
    if (size == 0) return nullptr;
#ifdef _WIN32
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    auto ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return nullptr; // Приводим к общему знаменателю с Win32
    return ptr;
#endif
}


