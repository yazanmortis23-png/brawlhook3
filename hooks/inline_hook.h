#pragma once
// ============================================================================
// Custom Inline Hook — mprotect-based (ARM64)
// Based on Nexus Brawl's hooking mechanism (no external framework)
// ============================================================================

#include <cstdint>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <link.h>
#include <android/log.h>

#define LOG_TAG "BrawlHook"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace hook {

// Find base address of a loaded library
inline uintptr_t find_library_base(const char* library_name) {
    struct CallbackData {
        const char* name;
        uintptr_t base;
    };

    CallbackData data = {library_name, 0};

    dl_iterate_phdr([](struct dl_phdr_info* info, size_t, void* userdata) -> int {
        auto* d = static_cast<CallbackData*>(userdata);
        if (info->dlpi_name && strstr(info->dlpi_name, d->name)) {
            d->base = info->dlpi_addr;
            return 1; // stop iteration
        }
        return 0;
    }, &data);

    return data.base;
}

// Page-align an address
inline uintptr_t page_align(uintptr_t addr) {
    return addr & ~(static_cast<uintptr_t>(sysconf(_SC_PAGESIZE)) - 1);
}

// ARM64 inline hook structure
struct HookInfo {
    void* target;           // original function address
    void* replacement;      // our hook function
    void* trampoline;       // trampoline to call original
    size_t trampoline_size;
    bool installed;
};

// Install an inline hook on ARM64
// target_addr: absolute address of function to hook
// hook_func: our replacement function
// Returns: trampoline pointer to call original, or nullptr on failure
inline void* install_hook(uintptr_t target_addr, void* hook_func) {
    long page_size = sysconf(_SC_PAGESIZE);
    uintptr_t page = page_align(target_addr);

    // Allocate trampoline (RWX)
    // Save first 16 bytes of original + branch back
    size_t tramp_size = 4096;
    void* trampoline = mmap(nullptr, tramp_size,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (trampoline == MAP_FAILED) {
        LOGE("mmap trampoline failed");
        return nullptr;
    }

    // Copy original instructions to trampoline (first 16 bytes = 4 instructions)
    memcpy(trampoline, reinterpret_cast<void*>(target_addr), 16);

    // Add branch back to original + 16 (after our patch)
    // LDR X16, [PC, #8]
    // BR X16
    // <8-byte address>
    uint32_t* tramp_code = reinterpret_cast<uint32_t*>(
        reinterpret_cast<uintptr_t>(trampoline) + 16);
    uintptr_t resume_addr = target_addr + 16;

    tramp_code[0] = 0x58000050; // LDR X16, [PC, #8]
    tramp_code[1] = 0xD61F0200; // BR X16
    memcpy(&tramp_code[2], &resume_addr, 8);

    // Unprotect target page
    if (mprotect(reinterpret_cast<void*>(page), page_size * 2,
                 PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        LOGE("mprotect RWX failed");
        munmap(trampoline, tramp_size);
        return nullptr;
    }

    // Write branch to our hook at target
    // LDR X16, [PC, #8]
    // BR X16
    // <8-byte address of hook_func>
    uint32_t* target = reinterpret_cast<uint32_t*>(target_addr);
    uintptr_t hook_addr = reinterpret_cast<uintptr_t>(hook_func);

    target[0] = 0x58000050; // LDR X16, [PC, #8]
    target[1] = 0xD61F0200; // BR X16
    memcpy(&target[2], &hook_addr, 8);

    // Restore protection
    mprotect(reinterpret_cast<void*>(page), page_size * 2,
             PROT_READ | PROT_EXEC);

    // Flush instruction cache
    __builtin___clear_cache(
        reinterpret_cast<char*>(target_addr),
        reinterpret_cast<char*>(target_addr + 16));
    __builtin___clear_cache(
        reinterpret_cast<char*>(trampoline),
        reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(trampoline) + 32));

    LOGI("Hook installed: 0x%lX -> %p (trampoline: %p)",
         target_addr, hook_func, trampoline);

    return trampoline;
}

// Hook a function by library name + RVA offset
inline void* hook_by_offset(const char* lib_name, uintptr_t offset, void* hook_func) {
    uintptr_t base = find_library_base(lib_name);
    if (!base) {
        LOGE("Library not found: %s", lib_name);
        return nullptr;
    }
    LOGI("Found %s at base 0x%lX", lib_name, base);
    return install_hook(base + offset, hook_func);
}

// Hook EGL/GL functions by name (for ESP rendering)
inline void* hook_gl_func(const char* lib_name, const char* func_name, void* hook_func) {
    void* handle = dlopen(lib_name, RTLD_LAZY);
    if (!handle) {
        LOGE("dlopen %s failed: %s", lib_name, dlerror());
        return nullptr;
    }
    void* func = dlsym(handle, func_name);
    if (!func) {
        LOGE("dlsym %s failed: %s", func_name, dlerror());
        return nullptr;
    }
    return install_hook(reinterpret_cast<uintptr_t>(func), hook_func);
}

} // namespace hook
