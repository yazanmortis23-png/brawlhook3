// ============================================================================
// Brawl Stars Cheat — Standalone Loader
// Based on Nexus Brawl reverse engineering
// tg by @dre6d @HernBrawl
// BUILD:
//    Android NDK r27+ with CMake
//    Target: arm64-v8a
//    Min SDK: 28
//
// LOAD:
//    Option A: Add to APK as System.loadLibrary("brawlhook")
//    Option B: Zygisk module (rooted devices)
//    Option C: ptrace injection
// ============================================================================

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "hooks/inline_hook.h"

// إعلان الدالة القادمة من game_hooks.cpp بدون الحاجة لعمل include لملف الـ cpp نفسه
bool install_game_hooks();

#define LOG_TAG "BrawlHook"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Wait for libg.so to be loaded, then install hooks
static void* hook_thread(void*) {
    LOGI("Hook thread started, waiting for libg.so...");

    // Wait until libg.so is loaded
    int attempts = 0;
    while (attempts < 60) { // 30 seconds max
        uintptr_t base = hook::find_library_base("libg.so");
        if (base != 0) {
            LOGI("libg.so found at 0x%lX", base);
            break;
        }
        usleep(500000); // 500ms
        attempts++;
    }

    if (hook::find_library_base("libg.so") == 0) {
        LOGI("libg.so not found after 30s, aborting");
        return nullptr;
    }

    // Small delay for full library initialization
    usleep(2000000); // 2s

    // Install game hooks
    if (install_game_hooks()) {
        LOGI("=== ALL HOOKS INSTALLED SUCCESSFULLY ===");
    } else {
        LOGI("=== HOOK INSTALLATION FAILED ===");
    }

    return nullptr;
}

// JNI entry point — called when the library is loaded
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("=== Brawl Stars Hook v1.0 ===");
    LOGI("Based on Nexus Brawl 69.252 reverse engineering");

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGI("Failed to get JNI env");
        return JNI_VERSION_1_6;
    }

    // Start hook thread (don't block JNI_OnLoad)
    pthread_t thread;
    pthread_create(&thread, nullptr, hook_thread, nullptr);
    pthread_detach(thread);

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved) {
    LOGI("Unloading hooks...");
    // TODO: unhook everything, free trampolines
}
