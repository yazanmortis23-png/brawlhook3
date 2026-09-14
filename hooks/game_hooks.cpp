// ============================================================================
// Game Hooks — Brawl Stars 69.252
// Hooks into libg.so game functions by RVA offset
// All features connected
// tg by @dre6d @HernBrawl
// ============================================================================

#include "inline_hook.h"
#include "../features/speed.h"
#include "../features/aimbot.h"
#include "../features/esp.h"
#include "../features/killaura.h"
#include "../features/autododge.h"
#include "../features/fly.h"
#include "../features/xray.h"
#include "../features/autofarm.h"
#include "../features/camera.h"
#include "../features/antiafk.h"
#include "../features/boltmod.h"
#include "../features/spin.h"
#include "../features/outline.h"
#include "../features/spray_pin.h"
#include "../features/follow.h"
#include "../features/hitbox.h"
#include "../features/respawn_timer.h"
#include "../features/tile_grid.h"
#include "../../game/offsets_69252.h"

// Trampoline pointers (to call original functions)
static void* orig_tick_func = nullptr;
static void* orig_observer_func = nullptr;
static void* orig_post_entry_func = nullptr;
static void* orig_eglSwapBuffers = nullptr;
static void* orig_glDrawElements = nullptr;
static void* orig_glUseProgram = nullptr;

// ============================================================================
// Helper: extract player entity from game state
// Chain: game_state -> [0xDD0] -> [0xD60] -> entity[0]
// ============================================================================
static void* get_local_player(void* game_state) {
    if (!game_state) return nullptr;

    // TODO: Реализовать цепочку указателей
    // Точная структура зависит от того как game_state передаётся
    // в хукнутую функцию. Из реверса:
    //    GOT_GAME_CONTEXT (0xDD0) -> GOT_ENTITY_LIST (0xD60) -> entity[0]
    // Но это GOT оффсеты чит-модуля, не game_state.
    // Нужно определить как game_state ссылается на entity list.

    return nullptr; // заглушка
}

static int get_player_team(void* player_entity) {
    if (!player_entity) return -1;
    int team = 0;
    memcpy(&team, reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(player_entity) + offsets::entity::TEAM_DATA_1), 4);
    return team;
}

// ============================================================================
// Tick/Scheduling Hook (0x5921A0)
// Вызывается каждый тик. Все модификации геймплея здесь.
// ============================================================================
void hook_tick_function(void* game_state, void* tick_data) {
    // Вызвать оригинал — пусть игра обработает тик
    typedef void (*orig_t)(void*, void*);
    reinterpret_cast<orig_t>(orig_tick_func)(game_state, tick_data);

    void* player = get_local_player(game_state);
    int team = get_player_team(player);

    // === MOVEMENT ===
    if (features::speed::enabled) {
        features::speed::on_tick(game_state, tick_data);
    }
    if (features::fly::enabled && player) {
        // TODO: получить input_x, input_y из joystick state
        // features::fly::on_tick(player, input_x, input_y);
    }
    if (features::spin::enabled && player) {
        features::spin::on_tick(player);
    }

    // === COMBAT ===
    if (features::killaura::enabled) {
        features::killaura::on_tick(game_state, tick_data);
    }
    if (features::autododge::enabled && player) {
        features::autododge::on_tick(game_state, tick_data, player, team);
    }
    if (features::boltmod::enabled && player) {
        features::boltmod::on_tick(game_state, player, team);
    }

    // === AUTOMATION ===
    if (features::autofarm::enabled && player) {
        features::autofarm::on_tick(game_state, player, team);
    }
    if (features::follow::enabled && player) {
        features::follow::on_tick(game_state, player, team);
    }
    if (features::spray_pin::spray_enabled || features::spray_pin::pin_enabled
        || features::spray_pin::combat_fire) {
        features::spray_pin::on_tick(game_state);
    }

    // === MISC ===
    if (features::antiafk::enabled && player) {
        features::antiafk::on_tick(player);
    }
}

// ============================================================================
// Frame Observer Hook (0xB337C4)
// Вызывается каждый кадр. Захват данных для ESP/aimbot/camera.
// ============================================================================
void hook_observer_function(void* frame_data) {
    typedef void (*orig_t)(void*);
    reinterpret_cast<orig_t>(orig_observer_func)(frame_data);

    // ESP — захват позиций
    if (features::esp::enabled) {
        features::esp::on_frame(frame_data);
    }

    // Aimbot — обновление цели
    if (features::aimbot::enabled) {
        features::aimbot::on_frame(frame_data);
    }

    // Camera mod
    if (features::camera::enabled) {
        features::camera::on_frame(frame_data);
    }

    // Respawn timer
    if (features::respawn_timer::enabled) {
        features::respawn_timer::on_frame(frame_data, 0.0f /* TODO: current_time */);
    }
}

// ============================================================================
// eglSwapBuffers Hook
// Рендер оверлея перед свопом буферов.
// ============================================================================
typedef int (*eglSwapBuffers_t)(void* display, void* surface);
int hook_eglSwapBuffers(void* display, void* surface) {
    // === РЕНДЕР ОВЕРЛЕЯ ===

    // ESP — трейсеры, имена, дистанции, хитбоксы
    if (features::esp::enabled) {
        features::esp::render_overlay();
    }

    // Хитбоксы
    if (features::hitbox::enabled) {
        features::hitbox::render_overlay();
    }

    // Тайл-грид
    if (features::tile_grid::enabled) {
        // features::tile_grid::render_overlay(cam_x, cam_y, cam_zoom, screen_w, screen_h);
    }

    // Мод-меню
    //ui::menu::render();

    // Вызвать оригинал
    return reinterpret_cast<eglSwapBuffers_t>(orig_eglSwapBuffers)(display, surface);
}

// ============================================================================
// glUseProgram Hook
// Перехват переключения шейдеров для outline/xray.
// ============================================================================
typedef void (*glUseProgram_t)(GLuint program);
void hook_glUseProgram(GLuint program) {
    reinterpret_cast<glUseProgram_t>(orig_glUseProgram)(program);

    if (features::outline::enabled) {
        features::outline::on_use_program(program);
    }
}

// ============================================================================
// glDrawElements Hook
// Перехват draw call'ов для x-ray и outline.
// ============================================================================
typedef void (*glDrawElements_t)(GLenum mode, GLsizei count, GLenum type, const void* indices);
void hook_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    GLuint current_program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&current_program));

    // X-Ray: отключить depth test для персонажей
    if (features::xray::enabled) {
        features::xray::pre_draw(current_program, count);
    }

    // Outline: stencil-based обводка
    if (features::outline::enabled &&
        features::outline::is_character_draw(current_program, count)) {
        features::outline::draw_outline_stencil(current_program, count, type, indices);
        // outline уже вызвал glDrawElements внутри
        if (features::xray::enabled) features::xray::post_draw();
        return;
    }

    // Обычная отрисовка
    reinterpret_cast<glDrawElements_t>(orig_glDrawElements)(mode, count, type, indices);

    if (features::xray::enabled) {
        features::xray::post_draw();
    }
}

// ============================================================================
// Install all hooks
// ============================================================================
bool install_game_hooks() {
    LOGI("Installing game hooks for build 69.252...");

    // === GAME LOGIC HOOKS ===

    // Тик/scheduling — speed, killaura, dodge, farm, follow, spin...
    orig_tick_func = hook::hook_by_offset(
        "libg.so", offsets::SCHEDULING_FUNC,
        reinterpret_cast<void*>(hook_tick_function));
    if (!orig_tick_func) {
        LOGE("CRITICAL: Failed to hook tick function at 0x%lX", offsets::SCHEDULING_FUNC);
        return false;
    }
    LOGI("Tick hook installed at 0x%lX", offsets::SCHEDULING_FUNC);

    // Frame observer — ESP, aimbot, camera
    orig_observer_func = hook::hook_by_offset(
        "libg.so", offsets::OBSERVER_FUNC,
        reinterpret_cast<void*>(hook_observer_function));
    if (!orig_observer_func) {
        LOGE("WARNING: Failed to hook observer at 0x%lX (ESP disabled)", offsets::OBSERVER_FUNC);
    } else {
        LOGI("Observer hook installed at 0x%lX", offsets::OBSERVER_FUNC);
    }

    // === GL HOOKS ===

    // eglSwapBuffers — оверлей и мод-меню
    orig_eglSwapBuffers = hook::hook_gl_func(
        "libEGL.so", "eglSwapBuffers",
        reinterpret_cast<void*>(hook_eglSwapBuffers));
    if (!orig_eglSwapBuffers) {
        LOGE("WARNING: Failed to hook eglSwapBuffers (overlay disabled)");
    } else {
        LOGI("eglSwapBuffers hook installed");
    }

    // glUseProgram — outline detection
    orig_glUseProgram = hook::hook_gl_func(
        "libGLESv2.so", "glUseProgram",
        reinterpret_cast<void*>(hook_glUseProgram));
    if (!orig_glUseProgram) {
        LOGE("WARNING: Failed to hook glUseProgram (outline disabled)");
    } else {
        LOGI("glUseProgram hook installed");
    }

    // glDrawElements — x-ray и outline rendering
    orig_glDrawElements = hook::hook_gl_func(
        "libGLESv2.so", "glDrawElements",
        reinterpret_cast<void*>(hook_glDrawElements));
    if (!orig_glDrawElements) {
        LOGE("WARNING: Failed to hook glDrawElements (xray/outline disabled)");
    } else {
        LOGI("glDrawElements hook installed");
    }

    LOGI("=== All hooks installed ===");
    LOGI("Features: speed, fly, spin, aimbot, killaura, autododge,");
    LOGI("  boltmod, ESP, xray, outline, hitbox, camera, autofarm,");
    LOGI("  follow, spray/pin, antiafk, tile_grid, respawn_timer");
    return true;
}
