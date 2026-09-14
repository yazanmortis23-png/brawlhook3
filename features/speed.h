#pragma once
// ============================================================================
// Speed Exploit — Brawl Stars 69.252
// Реальные оффсеты из реверса Nexus
// by tg @dre6d
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>

namespace features {
namespace speed {

inline bool enabled = false;
inline int level = 1;          // 1-4
inline bool local_move = false;
inline bool spin_enabled = false;
inline float spin_speed = 5.0f;

// Множители скорости по уровням
inline float level_multipliers[] = {1.0f, 1.5f, 2.0f, 3.0f, 5.0f};

// Чтение позиции из entity struct
inline void read_position(void* entity, float& x, float& y) {
    auto base = reinterpret_cast<uintptr_t>(entity);
    memcpy(&x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), sizeof(float));
    memcpy(&y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), sizeof(float));
}

// Чтение скорости из entity struct
inline void read_velocity(void* entity, float& vx, float& vy) {
    auto base = reinterpret_cast<uintptr_t>(entity);
    memcpy(&vx, reinterpret_cast<void*>(base + offsets::entity::VELOCITY_X), sizeof(float));
    memcpy(&vy, reinterpret_cast<void*>(base + offsets::entity::VELOCITY_Y), sizeof(float));
}

// Запись позиции в entity struct
// Nexus делает это с верификацией: пишет, потом читает обратно
// и проверяет что значение принялось (readback_verified)
inline bool write_position(void* entity, float x, float y) {
    auto base = reinterpret_cast<uintptr_t>(entity);

    // proposed_raw_x/y — записываем желаемую позицию
    memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_X), &x, sizeof(float));
    memcpy(reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), &y, sizeof(float));

    // readback — проверяем что игра приняла новые координаты
    float rx, ry;
    memcpy(&rx, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), sizeof(float));
    memcpy(&ry, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), sizeof(float));

    return (rx == x && ry == y);
}

// Вызывается каждый тик из хука scheduling func (0x5921A0)
inline void on_tick(void* game_state, void* tick_data) {
    if (!enabled || !game_state) return;

    float mult = level_multipliers[level];

    // Получить entity локального игрока:
    // game_state -> [GOT_GAME_CONTEXT (0xDD0)] -> [GOT_ENTITY_LIST (0xD60)] -> entity[0]
    // Точная цепочка зависит от того как game_state передаётся в tick func.
    //
    // Из реверса:
    // 1. Базовый указатель хранится в GOT чит-модуля по оффсету 0xDD0
    // 2. Из него читается список сущностей по оффсету 0xD60
    // 3. Первый элемент (или по индексу) — локальный игрок
    //
    // Для standalone чита нужно:
    // a) Найти указатель на game state из аргументов хукнутой функции
    //    (tick_data или game_state — один из них содержит entity list)
    // b) Или найти глобальный указатель через сигнатуру в libg.so

    // void* player_entity = get_local_player(game_state);
    // if (!player_entity) return;
    //
    // float x, y, vx, vy;
    // read_position(player_entity, x, y);
    // read_velocity(player_entity, vx, vy);
    //
    // // Применить множитель к движению
    // float new_x = x + vx * mult;
    // float new_y = y + vy * mult;
    //
    // // Записать с верификацией
    // if (!write_position(player_entity, new_x, new_y)) {
    //     // xy_write_refused — игра отклонила запись
    //     // movement_restore_unverified — откатить
    // }
}

} // namespace speed
} // namespace features
