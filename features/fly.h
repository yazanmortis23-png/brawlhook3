#pragma once
// ============================================================================
// Fly — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Fly это по сути speed exploit без привязки к movement input.
// Игрок может двигаться в любом направлении независимо от коллизий.
// Реализуется через прямую запись координат, игнорируя проверки стен/воды.
//
// Ключевые строки:
//   flyEnabled
//   movement_enabled, movement_restore_unverified
//   xy_writes, committed_raw_x, committed_raw_y
//
// Оффсеты:
//   Entity position: 0x20 (X), 0x24 (Y)
//   Entity velocity: 0x70 (VX), 0x74 (VY)
//   State flags: 0x9C (проверка коллизий?)
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace fly {

inline bool enabled = false;
inline float fly_speed = 3.0f;

// Направление полёта задаётся через joystick input
// Nexus перехватывает movement input и применяет напрямую
// к координатам, обходя коллизии

inline void on_tick(void* player_entity, float input_x, float input_y) {
    if (!enabled || !player_entity) return;

    auto base = reinterpret_cast<uintptr_t>(player_entity);

    float cur_x, cur_y;
    memcpy(&cur_x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), 4);
    memcpy(&cur_y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), 4);

    // Нормализация input
    float len = sqrtf(input_x * input_x + input_y * input_y);
    if (len > 0.01f) {
        input_x /= len;
        input_y /= len;
    }

    // Прямая запись новой позиции (обходит коллизии)
    float new_x = cur_x + input_x * fly_speed;
    float new_y = cur_y + input_y * fly_speed;

    memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_X), &new_x, 4);
    memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), &new_y, 4);
}

} // namespace fly
} // namespace features
