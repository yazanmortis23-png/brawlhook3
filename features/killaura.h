#pragma once
// ============================================================================
// Kill Aura — Brawl Stars
// tg by @dre6d @HernBrawl
// How Nexus does it:
// 1. Each tick, scan for enemies within attack range
// 2. Determine current weapon type via weapon class database (248 classes)
// 3. Check line-of-sight (wall check if killauraNoWall enabled)
// 4. Auto-fire main attack / super / gadget at nearest valid target
// 5. Optional: skip if target has ball (killauraNoBall)
// 6. Optional: disable below health threshold
//
// Key strings from reverse:
//   killauraEnabled, killauraMainAttack, killauraSuper, killauraGadget
//   killauraNoBall, killauraNoWall
//   nexus_sx_aura_health_disable
//   nexus_evasion_aura_snapshot_v1
//   nexus_evasion_runtime_aura_route_v1
//   worker_target, final_target, weapon_bypass
//   AURA MAIN, AURA PARAMS
//
// The aura routes through nexus_evasion_event_routes_v2 which maps
// game events to cheat actions. The weapon database determines
// attack range, projectile count, and pattern for each brawler.
//
// Weapon examples (from weapons.txt — 248 total):
//   CrowWeapon (poison daggers, short range, spread)
//   BullDudeWeapon (shotgun, close range, cone)
//   CactusWeapon (spike grenades, medium range, aoe)
//   ShieldTankWeapon (shield bash, melee range)
//   ArcadeWeapon (joystick throw, medium range)
//   BeamerWeapon (beam, long range, continuous)
// ============================================================================

namespace features {
namespace killaura {

inline bool enabled = false;
inline bool main_attack = true;
inline bool use_super = false;
inline bool use_gadget = false;
inline bool no_ball = true;     // don't attack ball carrier in brawl ball
inline bool wall_check = true;  // check line-of-sight
inline float health_threshold = 0.0f; // disable below this HP %

inline void on_tick(void* game_state, void* tick_data) {
    if (!enabled || !game_state) return;

    // TODO: Implementation steps:
    //
    // 1. Get local player position + weapon info from game_state
    // 2. Get weapon attack range from weapon class lookup
    // 3. Iterate all entities, find enemies in range
    // 4. If wall_check: cast ray from player to enemy, skip if blocked
    // 5. If no_ball: skip enemies carrying the ball
    // 6. Pick best target (closest, or lowest HP)
    // 7. Calculate attack direction
    // 8. Write attack input to game state:
    //    - Set attack direction vector
    //    - Set attack button pressed
    //    - For super: set super button pressed
    //    - For gadget: set gadget button pressed
}

} // namespace killaura
} // namespace features
