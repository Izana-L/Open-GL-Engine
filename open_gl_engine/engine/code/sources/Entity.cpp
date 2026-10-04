#include <Entity.hpp>
#include <Scene.hpp>

namespace open_gl_engine
{
    /// @brief Constructor de Entity.
    /// La inicialización se realiza desde fuera (normalmente desde Scene) para romper la dependencia circular:
    /// Entity necesita un puntero a Scene para consultar sus componentes, pero Scene incluye Entity en su
    /// definición. Al recibir el puntero desde el exterior, Entity.hpp solo necesita una declaración adelantada
    /// de Scene y no se produce una inclusión cíclica de cabeceras.
    /// @param my_scene Puntero a la Scene que contiene la entidad.
    /// @param my_id Identificador único asignado por la Scene.
    Entity::Entity(Scene* my_scene, Id my_id) : scene(my_scene), id(my_id)
    {
        // No se realiza ninguna operación adicional; la máscara de componentes
        // se inicializa a 0 en la declaración de la estructura.
    }
}