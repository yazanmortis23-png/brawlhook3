#pragma once
// ============================================================================
// Follow Mode — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Автоматическое следование за целью (союзником или врагом).
//
// Ключевые строки:
//   followModeEnemy, followModeTeam
//   nexus_sx_follow_closest_ally
//   nexus_sx_follow_closest_enemy
//   FOLLOW (UI)
//
// Режимы:
//   - Следовать за ближайшим союзником
//   - Следовать за ближайшим врагом
//   - Следовать за конкретным игроком (по ID)
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace follow {

inline bool enabled = false;
inline int mode = 0; // 0=closest_ally, 1=closest_enemy, 2=specific

struct Vec2 {
    float x, y;
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const {
        float l = length();
        return l > 0 ? Vec2{x / l, y / l} : Vec2{0, 0};
    }
};

inline float follow_distance = 2.0f; // держать дистанцию
inline float follow_speed = 2.0f;

inline void on_tick(void* game_state, void* player_entity, int player_team) {
    if (!enabled || !game_state || !player_entity) return;

    auto base = reinterpret_cast<uintptr_t>(player_entity);
    Vec2 player_pos;
    memcpy(&player_pos.x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), 4);
    memcpy(&player_pos.y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), 4);

    // 1. Итерация entity list
    // 2. Фильтр по mode:
    //    mode=0: team == player_team && entity != player
    //    mode=1: team != player_team
    // 3. Найти ближайшего
    // 4. Если дистанция > follow_distance:
    //    - Записать movement input в направлении цели
    // 5. Если дистанция <= follow_distance:
    //    - Остановиться

    // Vec2 target_pos = find_closest_target(game_state, player_pos, player_team, mode);
    // Vec2 delta = target_pos - player_pos;
    // float dist = delta.length();
    //
    // if (dist > follow_distance) {
    //     Vec2 dir = delta.normalized();
    //     Vec2 new_pos = player_pos + dir * follow_speed;
    //     memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_X), &new_pos.x, 4);
    //     memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), &new_pos.y, 4);
    // }
}

} // namespace follow
} // namespace features
