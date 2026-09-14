#pragma once
// ============================================================================
// Anti-AFK — Brawl Stars 69.252
//
// Как работает в Nexus:
// Периодически отправляет минимальный input чтобы игра не кикнула за AFK.
// Маленькое движение или нажатие каждые N секунд.
//
// Ключевые строки:
//   antiAfkEnabled
//   PREVENTS IDLE DISCONNECT (UI)
//
// Простейшая реализация — каждые 10 секунд микро-движение
// ============================================================================

#include <cstdint>
#include <cstring>
#include <cmath>

namespace features {
namespace antiafk {

inline bool enabled = false;
inline int interval_ticks = 300; // ~10 секунд при 30 тиках/сек
inline int tick_counter = 0;

inline void on_tick(void* player_entity) {
    if (!enabled || !player_entity) return;

    tick_counter++;
    if (tick_counter < interval_ticks) return;
    tick_counter = 0;

    // Микро-движение: сдвинуть позицию на 0.01 и обратно
    auto base = reinterpret_cast<uintptr_t>(player_entity);
    float x;
    memcpy(&x, reinterpret_cast<void*>(base + 0x20), 4);
    float nudge = x + 0.01f;
    memcpy(reinterpret_cast<void*>(base + 0x20), &nudge, 4);
    // На следующем тике игра сама скорректирует — но AFK таймер сбросится
}

} // namespace antiafk
} // namespace features
