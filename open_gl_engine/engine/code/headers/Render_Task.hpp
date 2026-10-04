#pragma once

#include <memory>
#include <vector>
#include <glm.hpp>
#include <Task.hpp>
#include <Texture.hpp>         

namespace open_gl_engine
{
    class   Kernel;
    struct  Mesh_Component;
    struct  Transform_Component;
    class   Scene;

    /// @brief Tarea que renderiza la escena.
    /// Separa objetos opacos y translúcidos, configura iluminación, materiales y
    /// texturas, y soporta renderizado a un framebuffer intermedio para postprocesado.
    class Render_Task : public Task
    {
       
    private:
        struct General_Light_Config
        {
            float ambient_intensity = 0.1f;
            float diffuse_intensity = 0.9f;
            float specular_intensity = 0.5f;
            float shininess = 32.0f;
        };

        struct Point_Light_Config
        {
            glm::vec3 world_position = glm::vec3(-3.0f, 0.0f, -20.0f);
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            float     intensity = 0.8f;
        };

        struct Directional_Light_Config
        {
            glm::vec3 world_direction = glm::vec3(-0.5f, -1.0f, -0.3f); // dirección en que viaja la luz
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            float     intensity = 0.6f;
            bool      enabled = true;
        };
        std::unique_ptr<Kernel> kernel;                  ///< Copia local del Kernel (acceso a shaders y uniforms).
        glm::mat4 view_matrix;                           ///< Matriz de vista actual.
        glm::mat4 projection_matrix;                     ///< Matriz de proyección actual.
        std::vector<Mesh_Component*> opaque_meshes;      ///< Mallas opacas a renderizar.
        std::vector<Mesh_Component*> translucent_meshes; ///< Mallas translúcidas (ordenadas por distancia).
        
        /// @brief Configura los uniforms de material (color y textura) en el shader dado.
        /// @param program_id ID del programa GL.
        /// @param color Color base del material.
        /// @param texture Puntero a la textura (o nullptr para usar blanca).
        void configure_material(unsigned int program_id, const glm::vec3& color, const Texture* texture);

        /// @brief Establece los uniforms de iluminación (posición, colores, factores).
        /// @param program_id ID del programa GL.
		void configure_General_light();
        void configure_Point_light();
		void configure_Directional_light();
        /// @brief Dibuja una malla individual con su transformación y material.
        /// @param meshComp Puntero al Mesh_Component.
        /// @param texture Textura a aplicar.
        void draw_mesh(const Mesh_Component* meshComp, const Texture* texture);

        /// @brief Dibuja todos los objetos opacos (depth write activo).
        /// @param meshes Vector de Mesh_Component opacos.
        /// @param default_tex Textura por defecto si el objeto no tiene una propia.
        void render_opaque_objects(const std::vector<Mesh_Component*>& meshes, const Texture* default_tex);

        /// @brief Dibuja los objetos translúcidos ordenados por distancia a la cámara (depth write desactivado).
        /// @param meshes Vector de Mesh_Component translúcidos.
        /// @param default_tex Textura por defecto.
        /// @param camPos Posición de la cámara en el mundo.
        void render_translucent_objects(const std::vector<Mesh_Component*>& meshes, const Texture* default_tex, const glm::vec3& camPos);

    public:
        General_Light_Config      general_light;
        Point_Light_Config        point_light;
        Directional_Light_Config  directional_light;
        
        /// @brief Constructor. Copia el kernel de la escena y pre-reserva espacio para las mallas.
        /// @param my_scene Puntero a la Scene propietaria.
        Render_Task(Scene* my_scene);

        /// @brief Ejecuta la lógica de renderizado del frame.
        void execute() override;

        /// @brief Destructor virtual.
        ~Render_Task() { scene = nullptr; }
    };
}