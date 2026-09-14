#pragma once
// ============================================================================
// Tile Grid Overlay — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Ключевые строки:
//   TOGGLE TILE GRID (UI)
//
// Рисует сетку тайлов карты для лучшего понимания позиционирования.
// Полезно для отладки wall check и dodge расчётов.
// ============================================================================

#include <GLES2/gl2.h>

namespace features {
namespace tile_grid {

inline bool enabled = false;
inline float tile_size = 1.0f; // размер тайла в game units

inline void render_overlay(float cam_x, float cam_y, float cam_zoom,
                            int screen_w, int screen_h) {
    if (!enabled) return;

    // Рассчитать видимую область тайлов
    // Нарисовать горизонтальные и вертикальные линии через GL_LINES
    // Цвет: полупрозрачный белый/серый
}

} // namespace tile_grid
} // namespace features
