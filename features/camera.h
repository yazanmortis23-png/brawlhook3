#pragma once
// ============================================================================
// Camera Mod — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Модифицирует параметры камеры: зум и смещение.
// Перезаписывает view/projection матрицу или параметры камеры в game state.
//
// Ключевые строки:
//   cameraEnabled, cameraZoom, cameraOffsetX, cameraOffsetY
//   nexus_sx_camera_zoom, nexus_sx_camera_offset_x, nexus_sx_camera_offset_y
//   LOCAL SPECTATOR CAMERA MODE
//   CAMERA CONFIG (UI section)
//
// Из offsets_69252.h — menu struct:
//   0xA90 — screen width
//   0xAA8 — screen height
//
// Zoom реализуется через масштабирование projection матрицы
// Offset — через сдвиг view матрицы
// ============================================================================

namespace features {
namespace camera {

inline bool enabled = false;
inline float zoom = 1.0f;       // 1.0 = default, <1 = zoom out, >1 = zoom in
inline float offset_x = 0.0f;   // горизонтальный сдвиг
inline float offset_y = 0.0f;   // вертикальный сдвиг
inline bool spectator_mode = false;

// Вызывается из observer hook (0xB337C4)
// Нужно найти указатель на камеру в game state
// и модифицировать zoom/offset параметры
inline void on_frame(void* frame_data) {
    if (!enabled || !frame_data) return;

    // Nexus делает это через nexus_visual_gl_calibrate_v1
    // которая калибрует screen coords к world coords
    //
    // Для zoom: модифицировать projection matrix scale
    // Для offset: модифицировать view matrix translation
    //
    // Альтернатива: перехватить glUniformMatrix4fv и
    // домножить projection матрицу на наш scale/translate
}

} // namespace camera
} // namespace features
