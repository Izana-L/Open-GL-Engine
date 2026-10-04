#include <ShaderCompiler.hpp>
#include <glad/gl.h> 
#include <string>
#include <iostream>
#include <fstream>      // para ifstream
#include <sstream>  
#include <cassert>

namespace open_gl_engine
{
    // ---- Función interna anónima para leer un archivo a string ----
    namespace
    {
        /// @brief Lee todo el contenido de un archivo de texto.
        /// @param filepath Ruta completa al archivo.
        /// @return Contenido del archivo en un std::string.
        std::string read_file(const std::string& filepath)
        {
            std::ifstream file(filepath, std::ios::in | std::ios::binary);
            assert(file.is_open() && "No se pudo abrir el archivo: ");
            std::stringstream buffer;
            buffer << file.rdbuf();      // vuelca todo el flujo
            return buffer.str();
        }
    }

    // ---- Implementaciones de las funciones públicas ----

    // Carga los shaders desde archivos y los compila/enlaza.
    GLuint load_and_compile_shaders(const std::string& vertex_filename, const std::string& fragment_filename)
    {
        const std::string base_path = "../assets/shaders/";

        // Construir rutas completas
        std::string vert_path = base_path + vertex_filename;
        std::string frag_path = base_path + fragment_filename;

        // Leer código fuente desde disco
        std::string vert_source = read_file(vert_path);
        std::string frag_source = read_file(frag_path);

        // Compilar y enlazar con las cadenas obtenidas
        return compile_shaders(vert_source, frag_source);
    }

    // Compila shaders dados sus códigos fuente.
    GLuint compile_shaders(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
    {
        GLint succeeded = GL_FALSE;

        // Crear objetos shader vacíos
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Preparar las cadenas y longitudes para glShaderSource
        const char* vertex_shaders_code[] = { vertex_shader_code.c_str() };
        const char* fragment_shaders_code[] = { fragment_shader_code.c_str() };
        const GLint vertex_shaders_size[] = { (GLint)vertex_shader_code.size() };
        const GLint fragment_shaders_size[] = { (GLint)fragment_shader_code.size() };

        // Asignar código fuente a los shaders
        glShaderSource(vertex_shader_id, 1, vertex_shaders_code, vertex_shaders_size);
        glShaderSource(fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

        // Compilar ambos shaders
        glCompileShader(vertex_shader_id);
        glCompileShader(fragment_shader_id);

        // Verificar errores de compilación del vertex shader
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(vertex_shader_id);

        // Verificar errores de compilación del fragment shader
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(fragment_shader_id);

        // Crear programa y adjuntar los shaders compilados
        GLuint program_id = glCreateProgram();
        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);

        // Enlazar el programa
        glLinkProgram(program_id);

        // Verificar errores de enlace
        glGetProgramiv(program_id, GL_LINK_STATUS, &succeeded);
        if (!succeeded) show_linkage_error(program_id);

        // Los shaders individuales ya no son necesarios una vez enlazados
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        return program_id;
    }

    // Muestra el log de compilación de un shader y termina con assert.
    void show_compilation_error(unsigned int shader_id)
    {
        std::string info_log;
        GLint info_log_length;

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetShaderInfoLog(shader_id, info_log_length, NULL, &info_log.front());

        std::cerr << info_log.c_str() << std::endl;
        assert(false && "Error de compilación de shader");
    }

    // Muestra el log de enlace de un programa y termina con assert.
    void show_linkage_error(unsigned int program_id)
    {
        std::string info_log;
        GLint info_log_length;

        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetProgramInfoLog(program_id, info_log_length, NULL, &info_log.front());

        std::cerr << info_log.c_str() << std::endl;
        assert(false && "Error de enlace de programa");
    }
}