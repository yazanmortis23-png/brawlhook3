#pragma once
// ============================================================================
// Hitbox Renderer — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Рисует хитбоксы персонажей и проектайлов через GL overlay.
//
// Ключевые строки:
//   hitboxRenderer
//   espShowHitbox
//   nexus_vis_attack_range
//   HITBOXES, ATTACK RANGE (UI)
//
// Техника: в eglSwapBuffers хуке рисовать круги/прямоугольники
// в позициях entity с радиусом хитбокса
// ============================================================================

#include "../../game/offsets_69252.h"
#include <GLES2/gl2.h>
#include <cmath>

namespace features {
namespace hitbox {

inline bool enabled = false;
inline bool show_attack_range = false;

// Рисовать круг через GL_LINE_LOOP
inline void draw_circle_gl(float cx, float cy, float radius, int segments = 32) {
    // Вершины круга
    float vertices[64 * 2]; // max 64 segments
    if (segments > 64) segments = 64;

    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * (float)i / (float)segments;
        vertices[i * 2]     = cx + cosf(angle) * radius;
        vertices[i * 2 + 1] = cy + sinf(angle) * radius;
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_LINE_LOOP, 0, segments);
    glDisableVertexAttribArray(0);
}

inline void render_overlay(/* entity list, camera projection */) {
    if (!enabled) return;

    // Для каждого entity:
    // 1. World-to-screen проекция позиции
    // 2. Масштабировать радиус хитбокса по zoom камеры
    // 3. draw_circle_gl(screen_x, screen_y, hitbox_radius_screen)
    //
    // Если show_attack_range:
    //   Нарисовать второй круг с радиусом атаки текущего бравлера
}

} // namespace hitbox
} // namespace features
