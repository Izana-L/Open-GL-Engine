#include <Scene.hpp>
#include <Window.hpp>
#include <Render_Task.hpp>
#include <Camera_Control_Task.hpp>
#include <PostProcess_Task.hpp>
#include "Transform_Update_Task.hpp"

namespace open_gl_engine
{
    /// @brief Constructor de Scene. Crea y registra las tareas fundamentales del motor.
    /// Las tareas se añaden al kernel en orden inverso de ejecución
    /// (la última añadida se ejecuta primero cada frame).
    Scene::Scene(Window& my_window) : window(my_window)
    {
        // Crear las tareas como shared_ptr para que el kernel las mantenga vivas
        auto render = std::make_shared<Render_Task>(this);           // Renderizado de la escena
        auto transform = std::make_shared<Transform_Update_Task>(this); // Actualización de físicas/posiciones
        auto camera = std::make_shared<Camera_Control_Task>(this);   // Control de cámara por teclado/ratón
        auto post = std::make_shared<PostProcess_Task>(this);      // Postprocesado (viñeta, FXAA...)

        // Orden de ejecución (el kernel las recorre de principio a fin):
        // post -> render -> transform -> camera
        kernel.add_task(post);
        kernel.add_task(render);
        kernel.add_task(transform);
        kernel.add_task(camera);
    }
}