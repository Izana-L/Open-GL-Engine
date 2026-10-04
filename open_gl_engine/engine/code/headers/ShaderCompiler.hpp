#pragma once
#include <string>

/// @brief Funciones de compilación y enlace de shaders OpenGL.
/// Proporciona utilidades para cargar shaders desde archivos, compilarlos,
/// enlazarlos en un programa y mostrar errores de compilación o enlace.
namespace open_gl_engine
{
    /// @brief Muestra por consola el log de error de compilación de un shader y aborta.
    /// @param shader_id ID del shader que falló la compilación.
    void show_compilation_error(unsigned int shader_id);

    /// @brief Muestra por consola el log de error de enlace de un programa y aborta.
    /// @param program_id ID del programa que falló el enlace.
    void show_linkage_error(unsigned int program_id);

    /// @brief Carga los archivos fuente de los shaders (vertex y fragment) desde el disco,
    /// los compila y enlaza en un programa OpenGL.
    /// @param vertex_filename Nombre del archivo del vertex shader (relativo a "../assets/shaders/").
    /// @param fragment_filename Nombre del archivo del fragment shader.
    /// @return ID del programa OpenGL resultante.
    unsigned int load_and_compile_shaders(const std::string& vertex_filename, const std::string& fragment_filename);

    /// @brief Compila y enlaza shaders directamente desde cadenas de código fuente.
    /// @param vertex_shader_code Código fuente del vertex shader.
    /// @param fragment_shader_code Código fuente del fragment shader.
    /// @return ID del programa OpenGL resultante.
    unsigned int compile_shaders(const std::string& vertex_shader_code, const std::string& fragment_shader_code);
}