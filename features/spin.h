#pragma once
// ============================================================================
// Spin Attack — Brawl Stars 69.252
//
// Как работает в Nexus:
// Автоматическое вращение персонажа с заданной скоростью.
// Используется для BM (bad manners) или для некоторых бравлеров
// у которых атака зависит от направления взгляда.
//
// Ключевые строки:
//   spinSpeed, isSpinEnabled
//   nexus_sx_spin_speed
//   nexus_evasion_spin_register
//   nexus_evasion_spin_post
//   nexus_evasion_spin_validate
//   nexus_evasion_spin_recheck
//   nexus_evasion_spin_snapshot
//   nexus_evasion_spin_lease
//
// Реализация:
//   Каждый тик увеличивать угол rotation на spin_speed
//   Записывать в entity::ROTATION (0x2C)
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace features {
namespace spin {

inline bool enabled = false;
inline float speed = 0.3f; // радиан за тик
inline float current_angle = 0.0f;

inline void on_tick(void* player_entity) {
    if (!enabled || !player_entity) return;

    current_angle += speed;
    if (current_angle > 2.0f * M_PI) current_angle -= 2.0f * M_PI;

    auto base = reinterpret_cast<uintptr_t>(player_entity);
    memcpy(reinterpret_cast<void*>(base + offsets::entity::ROTATION),
           &current_angle, sizeof(float));
}

} // namespace spin
} // namespace features
