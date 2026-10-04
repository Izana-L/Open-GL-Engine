#include <Input_Manager.hpp>
#include <Mesh_Directory.hpp>
#include <Camera_Component.hpp>
#include <Mesh_Component.hpp>
#include <Transform_Component.hpp>
#include <Texture_Manager.hpp>
#include <Texture_Component.hpp>
#include <Movement_Component.hpp>
#include <Model_Loader.hpp>
#include <Scene.hpp>
#include <Id.hpp>
#include <Window.hpp>
#include <Heightmap.hpp>
#include <Skybox_Manager.hpp>
#include <random>
#include "Entity_Creator.hpp"
using namespace open_gl_engine;
//utilizo el mismo namespace pero tengo pensado hacer como namespace anidados como open_gl_engine::tasks 
// en el main solo se ejecuta el kernel


int main(int, char* [])
{
    unsigned viewport_width = 1024;
    unsigned viewport_height = 576;
    Window::OpenGL_Context_Settings settings{ 3,3 };

    Window window("Open GL engine", viewport_width, viewport_height, settings);
    Input_Manager::initialize(window);
    Scene scene(window);
    std::vector<std::string> faces =
    {
        "sky-cube-map-0.png",
        "sky-cube-map-1.png",
        "sky-cube-map-2.png",
        "sky-cube-map-3.png",
        "sky-cube-map-4.png",
        "sky-cube-map-5.png"
    };
	create_skybox(scene, "day", faces);
	create_camera(scene, glm::vec3(0.0f, 0.0f, 5.0f));
	Id terrain_id = create_terrain(scene, "height-map.png", 10.0f, 50.0f);
    scene.add_component<Texture_Component>(terrain_id);
    auto tex_opt = scene.get_component<Texture_Component>(terrain_id);
    tex_opt->get().texture = Texture_Manager::get().load_texture("Moon_d.jpg");
    
    Id cube_id = create_cube(scene, glm::vec3(-2.0f, 0.0f, -11.0f), INVALID_ID, "uv-checker.png");
    scene.add_component<Movement_Component>(cube_id,glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(glm::radians(30.0f), glm::radians(45.0f), glm::radians(60.0f)));
    Id cylinder_id = create_cylinder(scene, glm::vec3(-9.0f, 0.0f, -11.0f), INVALID_ID, "wood.png");
 
   

    scene.run();

    return 0;
}