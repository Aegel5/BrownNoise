#include <map>
#include <mutex>
#include <cassert>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

class OSAllocator {
public:
    // Удаляем возможность копирования
    OSAllocator(const OSAllocator&) = delete;
    OSAllocator& operator=(const OSAllocator&) = delete;

    static OSAllocator& instance() {
        static OSAllocator inst;
        return inst;
    }

    void* allocate(size_t size) {
#ifdef _WIN32
        return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
        void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) return nullptr;

        std::lock_guard<std::mutex> lock(registry_mutex);
        posix_size_registry[ptr] = size;
        return ptr;
#endif
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

#ifdef _WIN32
        VirtualFree(ptr, 0, MEM_RELEASE);
#else
        size_t size = 0;
        {
            std::lock_guard<std::mutex> lock(registry_mutex);
            auto it = posix_size_registry.find(ptr);
            if (it != posix_size_registry.end()) {
                size = it->second;
                posix_size_registry.erase(it);
            }
        }

        if (size > 0) {
            munmap(ptr, size);
        }
        else {
            assert(false && "Pointer not found in POSIX registry!");
        }
#endif
    }

private:
    OSAllocator() = default;
    ~OSAllocator() = default;

#ifndef _WIN32
    std::map<void*, size_t> posix_size_registry;
    std::mutex registry_mutex;
#endif
};
