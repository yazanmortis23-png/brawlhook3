#pragma once
// ============================================================================
// Spray & Pin Spam — Brawl Stars 69.252
//
// Как работает в Nexus:
// Автоматическая отправка спреев и пинов с заданным интервалом.
//
// Ключевые строки:
//   sprayEnabled, pinEnabled
//   nexus_sx_spray_interval, nexus_sx_pin_interval
//   SENDS PINS, SENDS SPRAYS (UI)
//   combatFire, combatFireInterval
//
// Реализация:
//   Каждые N тиков отправить game event для spray/pin
//   Через игровой API или прямую запись в event queue
// ============================================================================

#include <cstdint>

namespace features {
namespace spray_pin {

inline bool spray_enabled = false;
inline bool pin_enabled = false;
inline int spray_interval = 60;  // тиков между спреями (~2 сек)
inline int pin_interval = 45;    // тиков между пинами (~1.5 сек)

inline int spray_counter = 0;
inline int pin_counter = 0;

// Combat fire — автоматическая стрельба с интервалом
inline bool combat_fire = false;
inline int combat_fire_interval = 5; // тиков

inline int fire_counter = 0;

inline void on_tick(void* game_state) {
    if (!game_state) return;

    if (spray_enabled) {
        spray_counter++;
        if (spray_counter >= spray_interval) {
            spray_counter = 0;
            // TODO: Отправить spray event через game event system
            // Нужно найти указатель на event queue в game_state
            // и записать spray event
        }
    }

    if (pin_enabled) {
        pin_counter++;
        if (pin_counter >= pin_interval) {
            pin_counter = 0;
            // TODO: Отправить pin event
        }
    }

    if (combat_fire) {
        fire_counter++;
        if (fire_counter >= combat_fire_interval) {
            fire_counter = 0;
            // TODO: Записать attack input = pressed
        }
    }
}

} // namespace spray_pin
} // namespace features
