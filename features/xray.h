#pragma once
// ============================================================================
// X-Ray — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Модифицирует GL рендеринг чтобы объекты за стенами были видны.
// Два подхода:
// 1. Отключение depth test для персонажей (glDisable(GL_DEPTH_TEST))
// 2. Модификация шейдера — рисовать силуэт поверх всего
//
// Ключевые строки:
//   xRayEnabled, xrayTargetMode
//   nexus_sx_xray_target_mode
//   nexus_visual_gl_features_v1 — управление GL фичами
//
// Target modes:
//   0 = все враги
//   1 = только ближайший
//   2 = все (враги + союзники)
//
// GL хуки:
//   glDrawElements / glDrawArrays — перед отрисовкой персонажей
//   отключить depth test, нарисовать, включить обратно
// ============================================================================

#include <GLES2/gl2.h>

namespace features {
namespace xray {

inline bool enabled = false;
inline int target_mode = 0; // 0=enemies, 1=closest, 2=all

// Вызывается из хука glDrawElements
// game_shader_id — ID текущего шейдера игры
// Нужно определить какие draw call'ы рисуют персонажей
// (по shader program ID или по количеству вершин)
inline void pre_draw(GLuint shader_id, int vertex_count) {
    if (!enabled) return;

    // Персонажи обычно имеют характерное количество вершин
    // и используют определённый шейдер
    // Nexus определяет это через nexus_visual_gl_calibrate_v1

    // Для x-ray: отключить depth test перед отрисовкой персонажей
    glDisable(GL_DEPTH_TEST);
}

inline void post_draw() {
    if (!enabled) return;
    glEnable(GL_DEPTH_TEST);
}

} // namespace xray
} // namespace features
