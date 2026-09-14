cmake_minimum_required(VERSION 3.18)
project(brawlhook)

# Target: ARM64 Android
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(brawlhook SHARED
    main.cpp
    hooks/game_hooks.cpp
)

# حل مشكلة المسارات النسبية لتطابق أي مكان يطلبه الكود
target_include_directories(brawlhook PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}/hooks
    ${CMAKE_CURRENT_SOURCE_DIR}/features
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

target_link_libraries(brawlhook
    log           # android logging
    dl            # dlopen/dlsym
    EGL           # eglSwapBuffers hook
    GLESv2        # OpenGL ES for ESP rendering
)

# Strip debug info for smaller size
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")
target_compile_options(brawlhook PRIVATE -fvisibility=hidden)

# Only export JNI_OnLoad
set_target_properties(brawlhook PROPERTIES
    LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/exports.map"
)

# Create version script to hide internal symbols
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/exports.map
"{ global: JNI_OnLoad; JNI_OnUnload; local: *; };"
)
