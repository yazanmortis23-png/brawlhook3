#pragma once
// ============================================================================
// Ally Respawn Timer — Brawl Stars 69.252
//
// Ключевые строки:
//   allyRespawnTimer
//   nexus_vis_ally_respawn
//   TOGGLE TEAMMATE HP (UI)
//
// Показывает таймер респавна убитых союзников на экране
// ============================================================================

namespace features {
namespace respawn_timer {

inline bool enabled = false;
inline bool show_teammate_hp = false;

// Хранение времени смерти для каждого тиммейта
inline float death_time[3] = {0, 0, 0};
inline bool is_dead[3] = {false, false, false};
inline float respawn_duration = 5.0f; // секунд (зависит от режима)

inline void on_frame(void* game_state, float current_time) {
    if (!enabled || !game_state) return;

    // 1. Итерация entity list, фильтр по team == player_team
    // 2. Для каждого тиммейта:
    //    - Если HP <= 0 и не был мёртв: записать death_time
    //    - Если HP > 0 и был мёртв: сбросить
    // 3. Для мёртвых: рассчитать оставшееся время = respawn_duration - (now - death_time)
    // 4. Рисовать таймер через GL overlay
}

} // namespace respawn_timer
} // namespace features
