#pragma once
// ============================================================================
// Auto-Farm — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Как работает в Nexus:
// Автоматический геймплей — бот двигается, атакует, собирает предметы.
// Комбинирует aimbot + movement AI:
// 1. Находит ближайшего врага или ресурс (gems, bolts, stars)
// 2. Двигается к цели
// 3. Атакует когда в радиусе атаки
// 4. Уклоняется от проектайлов (если autododge включён)
//
// Ключевые строки:
//   autofarmEnabled
//   nexus_autofarm_enabled
//   nexus_autofarm_attack_enemies
//   TOGGLE AUTO PLAY AGAIN
//   SMOOTH HIGH-SPEED AUTOPILOT
//
// Зависимости: использует aimbot для наведения, speed для движения
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace autofarm {

inline bool enabled = false;
inline bool attack_enemies = true;
inline bool collect_items = true;
inline bool auto_play_again = false;

struct Vec2 {
    float x, y;
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const {
        float l = length();
        return l > 0 ? Vec2{x / l, y / l} : Vec2{0, 0};
    }
};

inline void on_tick(void* game_state, void* player_entity, int player_team) {
    if (!enabled || !game_state || !player_entity) return;

    auto base = reinterpret_cast<uintptr_t>(player_entity);
    Vec2 player_pos;
    memcpy(&player_pos.x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), 4);
    memcpy(&player_pos.y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), 4);

    // Логика:
    // 1. Итерация entity list (через 0xDE8 entity manager)
    // 2. Для каждой сущности — определить тип:
    //    - Враг (team != player_team, alive)
    //    - Ресурс (gem, bolt, star — по entity type ID)
    // 3. Найти ближайшую цель
    // 4. Если цель — враг и в радиусе атаки:
    //    - Установить attack direction в сторону врага
    //    - Установить attack button = pressed
    // 5. Если цель далеко:
    //    - Установить movement direction к цели
    // 6. Если auto_play_again и матч окончен:
    //    - Нажать кнопку "play again"
}

} // namespace autofarm
} // namespace features
