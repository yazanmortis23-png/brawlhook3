#pragma once
// ============================================================================
// Bolt Mod — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Модификация поведения болтов (снарядов) бравлера.
// Автоматическая атака, предикшен попадания, безопасный выход.
//
// Ключевые строки:
//   boltModEnabled, boltAutoAttackEnabled
//   boltPredictionEnabled, boltSafeExitEnabled
//   nexus_sx_bolt_mod
//   nexus_sx_bolt_wall_avoidance
//   nexus_sx_bolt_wall_lookahead
//   BOLT MOD, BOLT MODS (UI section)
//   BOLT WALL LOOKAHEAD
//   WALL AVOID, WALL CHECK
//
// Bolt auto-attack:
//   Автоматически стреляет когда враг входит в зону поражения
//   с учётом предикшена движения врага
//
// Bolt prediction:
//   Рассчитывает точку упреждения на основе скорости проектайла
//   и скорости/направления цели
//
// Bolt safe exit:
//   Автоматически прекращает атаку и отступает при низком HP
//
// Wall avoidance/lookahead:
//   Проверяет что снаряд не попадёт в стену
//   Lookahead — предсказание траектории на N тиков вперёд
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace boltmod {

inline bool enabled = false;
inline bool auto_attack = false;
inline bool prediction = false;
inline bool safe_exit = false;
inline bool wall_avoidance = true;
inline int wall_lookahead = 5; // тиков вперёд
inline float safe_exit_hp_threshold = 0.2f; // 20% HP

struct Vec2 {
    float x, y;
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const {
        float l = length();
        return l > 0 ? Vec2{x / l, y / l} : Vec2{0, 0};
    }
};

// Предикшен точки попадания
inline Vec2 predict_hit_point(Vec2 player_pos, Vec2 target_pos,
                               Vec2 target_vel, float projectile_speed) {
    float dist = (target_pos - player_pos).length();
    if (projectile_speed < 0.01f) return target_pos;
    float t = dist / projectile_speed;
    return target_pos + target_vel * t;
}

// Проверка стены на линии (ray cast)
// Нужно реализовать через чтение tile map из game state
inline bool is_wall_between(Vec2 from, Vec2 to, void* game_state) {
    // TODO: Читать tile map из game_state
    // Итерировать по линии from->to, проверять каждый тайл
    // Nexus использует WALL CHECK логику из nexus_evasion_event_routes_v2
    return false;
}

inline void on_tick(void* game_state, void* player_entity, int player_team) {
    if (!enabled || !game_state || !player_entity) return;

    auto base = reinterpret_cast<uintptr_t>(player_entity);
    Vec2 player_pos;
    memcpy(&player_pos.x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), 4);
    memcpy(&player_pos.y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), 4);

    // Safe exit check
    if (safe_exit) {
        float hp;
        memcpy(&hp, reinterpret_cast<void*>(base + offsets::entity::PRIMARY_STAT), 4);
        // TODO: нужен max_hp для расчёта процента
        // if (hp / max_hp < safe_exit_hp_threshold) {
        //     // Отступить от ближайшего врага
        //     return;
        // }
    }

    // Auto attack с предикшеном
    if (auto_attack) {
        // 1. Найти ближайшего врага
        // 2. Если prediction — рассчитать упреждение
        // 3. Если wall_avoidance — проверить стены
        // 4. Если всё ок — записать attack input
    }
}

} // namespace boltmod
} // namespace features
