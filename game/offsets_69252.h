#pragma once

// ============================================================================
// Brawl Stars Offsets — Build 69.252
// tg by @dre6d @HernBrawl
// Target: libg.so (ARM64)
// Extracted from Nexus Brawl cheat reverse engineering
// WARNING: Version-specific. Update after game patches.
// ============================================================================

namespace offsets {

// =========================================================================
// HOOK TARGETS (RVA в libg.so)
// Функции игры которые нужно хукнуть
// =========================================================================
constexpr uintptr_t SCHEDULING_FUNC     = 0x005921A0; // тик/scheduling loop
constexpr uintptr_t OBSERVER_FUNC       = 0x00B337C4; // обсервер фреймов
constexpr uintptr_t POST_ENTRY_FUNC     = 0x00B2F780; // пост-обработка фрейма

// =========================================================================
// GAME DATA (RVA в libg.so data section)
// Указатели на структуры данных игры
// =========================================================================
constexpr uintptr_t GAME_STATE_VTABLE   = 0x012EB9F0; // vtable игрового состояния
constexpr uintptr_t BATTLE_STATE        = 0x011ABAD8; // структура battle state

// Дополнительные data section RVAs (из UI модуля):
constexpr uintptr_t DATA_011A0000       = 0x011A0000; // (7 ссылок в UI)
constexpr uintptr_t DATA_011C0000       = 0x011C0000; // (4 ссылки в UI)
constexpr uintptr_t DATA_01210000       = 0x01210000; // (Runtime)
constexpr uintptr_t DATA_01220000       = 0x01220000; // (Runtime)
constexpr uintptr_t DATA_01230000       = 0x01230000; // (UI)
constexpr uintptr_t DATA_012E0000       = 0x012E0000; // (UI, 3 ссылки)
constexpr uintptr_t DATA_01340000       = 0x01340000; // (UI + Runtime)

// =========================================================================
// CHEAT MODULE GOT OFFSETS
// Оффсеты в GOT секции чит-модулей (не libg.so)
// Используются для хранения глобального состояния
// =========================================================================

// libNexusEvasion69252.so GOT:
constexpr uintptr_t GOT_GAME_CONTEXT   = 0x0DD0; // основной контекст игры (35 ссылок!)
constexpr uintptr_t GOT_ENTITY_MANAGER = 0x0DE8; // менеджер сущностей (37 ссылок!)
constexpr uintptr_t GOT_ENTITY_LIST    = 0x0D60; // список активных сущностей (31 ссылка)
constexpr uintptr_t GOT_SECONDARY_STATE= 0x0DC0; // вторичное состояние
constexpr uintptr_t GOT_FEATURE_CONFIG = 0x06B0; // конфигурация фич
constexpr uintptr_t GOT_FUNC_TABLE     = 0x06C8; // таблица указателей на функции
constexpr uintptr_t GOT_FUNC_TABLE_FLAG= 0x06D0; // флаг готовности таблицы

// libNexusEvasionRuntime69252.so GOT:
constexpr uintptr_t RT_GOT_STATE       = 0x0CA8; // глобальное состояние runtime
constexpr uintptr_t RT_GOT_EGL_HANDLE  = 0x0A00; // handle libEGL.so
constexpr uintptr_t RT_GOT_GLES_HANDLE = 0x0A08; // handle libGLESv2.so

// =========================================================================
// ENTITY STRUCT LAYOUT
// Оффсеты полей внутри сущности игрока/врага
// Извлечены из ng_prepare_observer_v1 (entity walk на x21)
// =========================================================================
namespace entity {
    // Позиция и трансформ
    constexpr size_t POSITION_X     = 0x20;  // float
    constexpr size_t POSITION_Y     = 0x24;  // float
    constexpr size_t POSITION_Z     = 0x28;  // float
    constexpr size_t ROTATION       = 0x2C;  // float (угол)

    // Статы
    constexpr size_t PRIMARY_STAT   = 0x60;  // (HP?)
    constexpr size_t SECONDARY_STAT = 0x68;  // 

    // Скорость
    constexpr size_t VELOCITY_X     = 0x70;  // float
    constexpr size_t VELOCITY_Y     = 0x74;  // float
    constexpr size_t VELOCITY_AUX_X = 0x78;  // float (доп. вектор скорости)
    constexpr size_t VELOCITY_AUX_Y = 0x7C;  // float

    // Состояние
    constexpr size_t STATE_FLAGS    = 0x9C;  // uint32_t
    constexpr size_t TEAM_DATA_1    = 0xA4;  // 
    constexpr size_t TEAM_DATA_2    = 0xA8;  // 

    // Расширенные поля (из глубокого entity walk)
    constexpr size_t FIELD_C0       = 0xC0;
    constexpr size_t FIELD_C4       = 0xC4;
    constexpr size_t FIELD_C8       = 0xC8;
    constexpr size_t FIELD_CC       = 0xCC;
    constexpr size_t FIELD_D0       = 0xD0;
    constexpr size_t FIELD_D4       = 0xD4;
    constexpr size_t FIELD_D8       = 0xD8;
    constexpr size_t FIELD_E0       = 0xE0;
    constexpr size_t FIELD_F0       = 0xF0;
    constexpr size_t FIELD_F8       = 0xF8;
    constexpr size_t FIELD_FC       = 0xFC;
    constexpr size_t FIELD_100      = 0x100;
    constexpr size_t FIELD_108      = 0x108;
    constexpr size_t FIELD_10C      = 0x10C;
    constexpr size_t FIELD_114      = 0x114;
    constexpr size_t FIELD_118      = 0x118;
    constexpr size_t FIELD_12C      = 0x12C;
    constexpr size_t FIELD_138      = 0x138;
    constexpr size_t FIELD_140      = 0x140;
    constexpr size_t FIELD_180      = 0x180;
    constexpr size_t FIELD_184      = 0x184;
    constexpr size_t FIELD_188      = 0x188;
    constexpr size_t FIELD_18C      = 0x18C;
    constexpr size_t FIELD_194      = 0x194;
    constexpr size_t FIELD_198      = 0x198;
    constexpr size_t FIELD_1A0      = 0x1A0;
    constexpr size_t FIELD_1A8      = 0x1A8;
    constexpr size_t FIELD_1B0      = 0x1B0;
    constexpr size_t FIELD_1B8      = 0x1B8;
}

// =========================================================================
// AURA/COMBAT STRUCT
// Оффсеты для kill aura системы
// Из nexus_evasion_runtime_aura_route_v1
// =========================================================================
namespace aura {
    constexpr size_t FIELD_120      = 0x120;
    constexpr size_t FIELD_128      = 0x128;
    constexpr size_t FIELD_240      = 0x240;
    constexpr size_t FIELD_264      = 0x264;
    constexpr size_t FIELD_26C      = 0x26C;
    constexpr size_t PHASE_370      = 0x370; // aura_phase
    constexpr size_t FIELD_378      = 0x378;
    constexpr size_t FIELD_37C      = 0x37C;
    constexpr size_t FIELD_380      = 0x380;
    constexpr size_t FIELD_388      = 0x388;
    constexpr size_t FIELD_390      = 0x390;
    constexpr size_t FIELD_398      = 0x398;
    constexpr size_t FIELD_3A0      = 0x3A0;
}

// =========================================================================
// ESP FILTER STRUCT
// Оффсеты для ESP фильтрации
// Из nexus_evasion_handler_status_v1
// =========================================================================
namespace esp_filter {
    constexpr size_t FILTER_BASE    = 0x300;
    constexpr size_t FILTER_SEC     = 0x318;
    constexpr size_t COMPONENT_PTR  = 0x430;
    constexpr size_t FILTER_STATE   = 0x490;
    constexpr size_t TEAM_FILTER_0  = 0x500; // espTeamFilterEnemies
    constexpr size_t TEAM_FILTER_1  = 0x508; // espTeamFilterSelf
    constexpr size_t TEAM_FILTER_2  = 0x510; // espTeamFilterTeammates
    constexpr size_t TEAM_FILTER_3  = 0x518;
}

// =========================================================================
// UI/MENU STRUCT
// Оффсеты для мод-меню
// Из nexus_menu_init и nexus_menu_dispatch
// =========================================================================
namespace menu {
    constexpr size_t STATE_FLAG     = 0x2FC;
    constexpr size_t RENDER_STATE   = 0x304;
    constexpr size_t FIELD_314      = 0x314;
    constexpr size_t RENDER_PIPE_1  = 0x6F0;
    constexpr size_t RENDER_PIPE_2  = 0x6F8;
    constexpr size_t RENDER_PIPE_3  = 0x700;
    constexpr size_t FIELD_758      = 0x758;
    constexpr size_t SCREEN_W       = 0xA90;
    constexpr size_t SCREEN_H       = 0xAA8;
    constexpr size_t UI_CONTEXT     = 0xEF8;
    constexpr size_t RENDER_CTX     = 0xF08;
    constexpr size_t DIMENSION_1    = 0x1E4;
    constexpr size_t DIMENSION_2    = 0x1E8;
    constexpr size_t BUTTON_SIZE    = 0x260; // размер структуры кнопки
}

// =========================================================================
// RENDER/STAGE STRUCT
// Оффсеты для рендер-системы (rich_render)
// =========================================================================
namespace render {
    constexpr size_t STAGE_ROOT     = 0x270;
    constexpr size_t IDENTITY_PTR   = 0x278;
    constexpr size_t TRANSFORM_1    = 0x2B8;
    constexpr size_t TRANSFORM_2    = 0x2C0;
    constexpr size_t RENDER_NODE    = 0x2D0;
    constexpr size_t TRANSFORM_MTX  = 0x2E0;
    constexpr size_t POSITION_1     = 0x2F0;
    constexpr size_t POSITION_2     = 0x2F8;
}

// =========================================================================
// RUNTIME CAPTURE STRUCT
// =========================================================================
namespace capture {
    constexpr size_t FIELD_164      = 0x164;
    constexpr size_t FIELD_168      = 0x168;
}

// =========================================================================
// GL FUNCTIONS (хукаются через dlsym)
// =========================================================================
// eglSwapBuffers       — главный хук для оверлея
// eglGetCurrentDisplay — получение текущего дисплея
// eglGetCurrentSurface — получение текущей поверхности
// eglGetCurrentContext — получение текущего контекста
// eglQueryAPI          — проверка API
// eglQuerySurface      — параметры поверхности
// glGetIntegerv        — получение параметров GL
// glGetFloatv, glGetBooleanv, glIsEnabled
// glEnable, glDisable, glScissor
// glClearColor, glClear

// =========================================================================
// SHADER UNIFORMS (для ESP outline)
// Инжектятся в фрагментный шейдер игры
// =========================================================================
// "u_nexusOutline"  — uniform bool, включает обводку
// "v_outlineColor"  — vec4 RGBA цвет обводки

// =========================================================================
// INTEGRITY HASHES
// Проверяются при bind — для своего чита нужно обойти или подменить
// =========================================================================
// SHA256: "a10aeb6b4085fb2a15d969a130cd9608231998b5269a40aed41129d99624ede3"
// SHA1:   "fdf834103d333f9f8a1947b3a405b32da6ebb651"

// =========================================================================
// ARM64 HOOK TRAMPOLINE
// Nexus использует 0xd65f0000 как опкод RET для построения трамплинов
// =========================================================================
constexpr uint32_t ARM64_RET_OPCODE = 0xD65F03C0;
constexpr uint32_t ARM64_LDR_X16_PC8 = 0x58000050; // LDR X16, [PC, #8]
constexpr uint32_t ARM64_BR_X16 = 0xD61F0200;       // BR X16

} // namespace offsets
