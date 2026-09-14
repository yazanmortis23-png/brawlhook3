#pragma once
// ============================================================================
// Aimbot — Brawl Stars
// by tg @dre6d
// How Nexus does it:
// 1. Observer hook (0xB337C4) captures entity positions each frame
// 2. Iterate all enemy entities, find closest/best target
// 3. Calculate aim angle from player to target
// 4. For predictive aim: factor in target velocity + projectile speed
// 5. Set attack input direction to calculated angle
//
// Key strings from reverse:
//   aopAimEnabled, aopAimTargetMode, holdToShootAim
//   nexus_sx_aop_projectile_speed
//   nexus_sx_hold_delay_ms, nexus_sx_hold_range_check
//   worker_target, final_target
//   SMART AIM, HOLD AIM, TARGET MODE
//
// Target modes (from UI strings):
//   - Closest enemy
//   - Lowest HP enemy
//   - Specific brawler types
//
// The weapon database (248 classes in weapons.txt) determines
// projectile speed and attack pattern per brawler.
// ============================================================================

#include <cmath>

namespace features {
namespace aimbot {

inline bool enabled = false;
inline int target_mode = 0;    // 0=closest, 1=lowest_hp, etc.
inline bool hold_aim = false;
inline float hold_delay_ms = 100.0f;
inline bool range_check = true;

struct Vec2 {
    float x, y;

    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const { float l = length(); return l > 0 ? Vec2{x/l, y/l} : Vec2{0,0}; }
};

struct EntityInfo {
    Vec2 position;
    Vec2 velocity;
    float health;
    int team;
    bool alive;
    // weapon class name for per-brawler logic
    // matched against weapons.txt database
};

// Predict where target will be when projectile arrives
inline Vec2 predict_position(Vec2 target_pos, Vec2 target_vel,
                              Vec2 player_pos, float projectile_speed) {
    float dist = (target_pos - player_pos).length();
    float time_to_hit = dist / projectile_speed;
    return target_pos + target_vel * time_to_hit;
}

inline void on_frame(void* frame_data) {
    if (!enabled || !frame_data) return;

    // TODO: Parse frame_data to extract:
    // - Player position
    // - All entity positions, teams, health
    // - Current weapon type (for projectile speed lookup)
    //
    // Then calculate aim direction and write to input state
}

} // namespace aimbot
} // namespace features
