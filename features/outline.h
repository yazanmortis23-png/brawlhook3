#pragma once
// ============================================================================
// Character Outline — Brawl Stars 69.252
//
// Как работает в Nexus:
// Инжекция кастомного шейдера в рендер-пайплайн игры.
// Добавляет цветную обводку вокруг персонажей.
//
// Ключевые строки:
//   characterOutlineEnabled
//   nexus_sx_outline_r, nexus_sx_outline_g, nexus_sx_outline_b
//   nexus_sx_outline_opacity
//   u_nexusOutline (uniform в шейдере)
//   v_outlineColor (varying в шейдере)
//   SHADER CHARACTER EDGE (UI)
//
// Техника:
// 1. Хукнуть glUseProgram — определить шейдер персонажей
// 2. Хукнуть glDrawElements — перед отрисовкой персонажа:
//    a. Первый проход: увеличить масштаб на 2-3%, рисовать solid цветом
//    b. Второй проход: нормальная отрисовка поверх
//    Результат — цветная обводка по краям
//
// Или через uniform injection:
//    glUniform1i(u_nexusOutline_loc, 1);
//    glUniform4f(v_outlineColor_loc, r, g, b, a);
// ============================================================================

#include <GLES2/gl2.h>

namespace features {
namespace outline {

inline bool enabled = false;
inline float color_r = 1.0f;
inline float color_g = 0.0f;
inline float color_b = 0.0f;
inline float opacity = 0.8f;

// ID шейдер-программы персонажей (нужно определить runtime)
inline GLuint character_shader_id = 0;

// Location униформов (ищутся через glGetUniformLocation)
inline GLint u_outline_loc = -1;
inline GLint u_color_loc = -1;

// Определить является ли текущий draw call отрисовкой персонажа
// Nexus делает это через nexus_visual_gl_calibrate_v1
inline bool is_character_draw(GLuint shader, int vertex_count) {
    // Эвристика: персонажи имеют определённый диапазон вершин
    // и используют конкретный шейдер
    // Точные значения нужно определить через GL trace
    return (shader == character_shader_id && vertex_count > 100);
}

// Хук glUseProgram — найти шейдер персонажей
inline void on_use_program(GLuint program) {
    if (!enabled) return;

    // Проверить есть ли у этого шейдера наши униформы
    GLint loc = glGetUniformLocation(program, "u_nexusOutline");
    if (loc >= 0) {
        // Шейдер уже модифицирован — активировать outline
        u_outline_loc = loc;
        u_color_loc = glGetUniformLocation(program, "v_outlineColor");
        character_shader_id = program;
    }
}

// Метод стенсил-обводки (не требует модификации шейдера):
// 1. Включить stencil test
// 2. Рисовать персонажа, записывая в stencil buffer
// 3. Увеличить масштаб, рисовать solid цветом где stencil != written
// 4. Выключить stencil
inline void draw_outline_stencil(GLuint shader, int count, int type,
                                  const void* indices) {
    if (!enabled) return;

    // Stencil setup
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);

    // Нормальная отрисовка (записывает в stencil)
    glDrawElements(type, count, GL_UNSIGNED_SHORT, indices);

    // Outline pass
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    // TODO: увеличить масштаб модели на 2-3%
    // и рисовать solid цветом (color_r, color_g, color_b, opacity)

    glDisable(GL_STENCIL_TEST);
}

} // namespace outline
} // namespace features
