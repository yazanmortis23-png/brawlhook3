#pragma once
// ============================================================================
// Auto-Dodge — Brawl Stars 69.252
// tg by @dre6d @HernBrawl
// Реальные оффсеты из реверса Nexus
//
// Как работает в Nexus:
// 1. Каждый тик перехватываются все проектайлы через ng_projectiles_v1
// 2. Классификация угроз по типу проектайла (weapon database, 248 классов)
// 3. Для каждого проектайла рассчитывается:
//    - Траектория (позиция + вектор скорости)
//    - Время до столкновения с игроком
//    - Зона поражения (радиус, конус, линия)
// 4. Если проектайл попадёт в игрока — рассчитывается вектор уклонения
// 5. Вектор уклонения записывается как movement input
//
// Ключевые экспорты из libNexusEvasion69252.so:
//   nexus_autododge_set_enabled       — вкл/выкл
//   nexus_autododge_set_replay_block  — блокировка реплея (антиоткат)
//   nexus_evasion_dodge_profile       — профили уклонения
//   nexus_evasion_segment_distance    — расчёт дистанции до сегмента
//
// Ключевые экспорты из libNexusEvasionRuntime69252.so:
//   ng_projectiles_v1                 — перехват проектайлов
//   ng_observe_registers_v1           — захват регистров
//
// Ключевые строки из реверса:
//   autododgeEnabled
//   nexus_autododge_version
//   dodge_profile
//   segment_distance
//   projectile_count, friendly_projectiles
//   worker_target, threat classification
//   combatDodge, v2Dodge, v3Dodge, v4Dodge, v5Dodge
//   AUTO DODGE (UI)
//
// Оффсеты проектайла (из entity walk в ng_prepare_observer_v1):
//   0x20, 0x24     — позиция X, Y
//   0x70, 0x74     — скорость X, Y (вектор направления)
//   0x78, 0x7C     — доп. скорость (ускорение?)
//   0x9C           — флаги состояния (активен, team, тип)
//   0xA4, 0xA8     — данные команды
//
// Оффсеты dodge-системы в GOT (libNexusEvasion69252.so):
//   0x0DD0         — game context
//   0x0DE8         — entity manager (итерация проектайлов)
//   0x0D60         — список активных сущностей
// ============================================================================

#include "../../game/offsets_69252.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace autododge {

inline bool enabled = false;
inline int version = 5;           // v2Dodge..v5Dodge — версия алгоритма
inline bool replay_block = true;  // блокировка серверного отката

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
    Vec2 perpendicular() const { return {-y, x}; } // 90 градусов
};

struct Projectile {
    Vec2 position;
    Vec2 velocity;
    float radius;       // зона поражения
    int team;           // команда владельца
    uint32_t flags;     // из STATE_FLAGS (0x9C)
    bool active;
};

// Чтение данных проектайла из entity struct
inline Projectile read_projectile(void* entity) {
    Projectile p = {};
    auto base = reinterpret_cast<uintptr_t>(entity);

    memcpy(&p.position.x, reinterpret_cast<void*>(base + offsets::entity::POSITION_X), 4);
    memcpy(&p.position.y, reinterpret_cast<void*>(base + offsets::entity::POSITION_Y), 4);
    memcpy(&p.velocity.x, reinterpret_cast<void*>(base + offsets::entity::VELOCITY_X), 4);
    memcpy(&p.velocity.y, reinterpret_cast<void*>(base + offsets::entity::VELOCITY_Y), 4);
    memcpy(&p.flags,      reinterpret_cast<void*>(base + offsets::entity::STATE_FLAGS), 4);
    memcpy(&p.team,       reinterpret_cast<void*>(base + offsets::entity::TEAM_DATA_1), 4);
    p.active = true;

    return p;
}

// Расчёт дистанции от точки до сегмента (линии проектайла)
// Nexus экспортирует это как nexus_evasion_segment_distance
inline float segment_distance(Vec2 point, Vec2 seg_start, Vec2 seg_end) {
    Vec2 d = seg_end - seg_start;
    Vec2 f = seg_start - point;

    float a = d.x * d.x + d.y * d.y;
    float b = 2.0f * (f.x * d.x + f.y * d.y);
    float c = f.x * f.x + f.y * f.y;

    if (a < 0.0001f) return f.length(); // сегмент нулевой длины

    float t = -(b / (2.0f * a));
    t = t < 0 ? 0 : (t > 1 ? 1 : t); // clamp [0, 1]

    Vec2 closest = {seg_start.x + t * d.x, seg_start.y + t * d.y};
    return (point - closest).length();
}

// Предсказание позиции проектайла через dt секунд
inline Vec2 predict_projectile(const Projectile& proj, float dt) {
    return proj.position + proj.velocity * dt;
}

// Рассчитать вектор уклонения
// Находит перпендикуляр к линии проектайла — убегает в сторону
inline Vec2 calculate_dodge_vector(Vec2 player_pos, const Projectile& proj,
                                    float dodge_distance = 3.0f) {
    // Вектор от проектайла к игроку
    Vec2 to_player = player_pos - proj.position;

    // Перпендикуляр к направлению полёта проектайла
    Vec2 perp = proj.velocity.perpendicular().normalized();

    // Выбрать сторону — уклоняемся в ту сторону куда ближе
    float dot = to_player.x * perp.x + to_player.y * perp.y;
    if (dot < 0) {
        perp = perp * (-1.0f); // перевернуть направление
    }

    return perp * dodge_distance;
}

// Основная функция — вызывается каждый тик
inline void on_tick(void* game_state, void* tick_data,
                     void* player_entity, int player_team) {
    if (!enabled || !game_state || !player_entity) return;

    Vec2 player_pos;
    memcpy(&player_pos.x,
           reinterpret_cast<void*>(
               reinterpret_cast<uintptr_t>(player_entity) + offsets::entity::POSITION_X), 4);
    memcpy(&player_pos.y,
           reinterpret_cast<void*>(
               reinterpret_cast<uintptr_t>(player_entity) + offsets::entity::POSITION_Y), 4);

    // Итерация проектайлов:
    // game_state -> [0xDE8] (entity manager) -> iterate entities
    // Фильтр: только вражеские проектайлы (team != player_team)
    //
    // Для каждого проектайла:
    // 1. Предсказать позицию через N тиков
    // 2. Посчитать дистанцию до игрока (segment_distance)
    // 3. Если дистанция < порог — рассчитать dodge вектор
    // 4. Выбрать самую опасную угрозу
    // 5. Записать movement input в направлении dodge вектора

    // Пример (псевдокод):
    //
    // float min_dist = 999.0f;
    // Vec2 best_dodge = {0, 0};
    // bool should_dodge = false;
    //
    // for (auto* entity : iterate_entities(game_state)) {
    //     Projectile proj = read_projectile(entity);
    //     if (proj.team == player_team) continue;  // свой проектайл
    //     if (!proj.active) continue;
    //
    //     // Предсказать позицию через 0.5 секунды
    //     Vec2 future_pos = predict_projectile(proj, 0.5f);
    //
    //     // Дистанция от игрока до линии полёта
    //     float dist = segment_distance(player_pos, proj.position, future_pos);
    //
    //     // Порог уклонения (зависит от радиуса поражения)
    //     float threshold = 2.0f + proj.radius;
    //
    //     if (dist < threshold && dist < min_dist) {
    //         min_dist = dist;
    //         best_dodge = calculate_dodge_vector(player_pos, proj);
    //         should_dodge = true;
    //     }
    // }
    //
    // if (should_dodge) {
    //     // Записать dodge вектор как movement input
    //     write_movement_input(player_entity, best_dodge);
    // }
}

} // namespace autododge
} // namespace features
