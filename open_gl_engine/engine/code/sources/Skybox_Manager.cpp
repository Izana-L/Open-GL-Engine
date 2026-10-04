#include "Skybox_Manager.hpp"
#include "ShaderCompiler.hpp"      // tu función compile_shaders(código_vert, código_frag)
#include "Texture_Manager.hpp"     // para load_cubemap
#include <cassert>

namespace open_gl_engine {

    // Singleton clásico.
    Skybox_Manager& Skybox_Manager::get() {
        static Skybox_Manager instance;
        return instance;
    }

    // Constructor: compila el shader de skybox y lo guarda como compartido.
    Skybox_Manager::Skybox_Manager() {
        shared_shader = load_and_compile_shaders("sky.vert", "sky.frag");
        assert(shared_shader != 0);
    }

    // Destructor: libera el programa de shader compartido.
    Skybox_Manager::~Skybox_Manager() {
        if (shared_shader) {
            glDeleteProgram(shared_shader);
        }
    }

    // Añade un skybox: carga el cubemap mediante el Texture_Manager, crea el objeto Skybox y lo almacena.
    bool Skybox_Manager::add_skybox(const std::string& name, const std::vector<std::string>& face_filenames) {
        // Cargar el cubemap a partir de las 6 caras
        const Cubemap* cubemap = Texture_Manager::get().load_cubemap(face_filenames);
        if (!cubemap || cubemap->id == 0) {
            return false; // fallo al cargar
        }

        // Crear el Skybox con el shader compartido y el cubemap
        auto sky = std::make_unique<Skybox>(shared_shader, cubemap);
        skyboxes[name] = std::move(sky);
        return true;
    }

    // Activa un skybox por nombre.
    void Skybox_Manager::set_active(const std::string& name) {
        auto it = skyboxes.find(name);
        if (it != skyboxes.end()) {
            active_skybox = it->second.get(); // apunta al skybox almacenado
        }
        else {
            active_skybox = nullptr; // no existe ese nombre
        }
    }

    // Retorna el skybox actualmente activo.
    const Skybox* Skybox_Manager::get_active() const {
        return active_skybox;
    }

} 