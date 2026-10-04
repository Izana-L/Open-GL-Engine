#include "Skybox.hpp"
#include <gtc/type_ptr.hpp>
#include <cassert>
#include <Mesh.hpp>        
#include <Cubemap.hpp>
#include <glad/gl.h>
#include <Mesh_Directory.hpp>

namespace open_gl_engine {

    // Constructor: obtiene la malla del cubo, guarda referencias y localiza uniforms.
    Skybox::Skybox(unsigned int shader_program, const Cubemap* cubemap)
        : cubemap(cubemap), program_id(shader_program)
    {
        // Obtener la malla de cubo skybox del directorio
        cube_mesh = Mesh_Directory::get().create_skybox_mesh();

        assert(cube_mesh && cube_mesh->vao_id != 0);
        assert(cubemap && cubemap->id != 0);
        assert(program_id != 0);

        // Guardar las localizaciones de los uniforms para usarlas en render()
        loc_model_view = glGetUniformLocation(program_id, "model_view_matrix");
        loc_projection = glGetUniformLocation(program_id, "projection_matrix");
        loc_sampler = glGetUniformLocation(program_id, "sampler");
    }

    // Render: dibuja el cubo del skybox con la vista sin traslación.
    void Skybox::render(const glm::mat4& view, const glm::mat4& projection) const
    {
        glUseProgram(program_id);

        glDisable(GL_CULL_FACE); // El cubo se ve desde dentro

        // Eliminar la traslación de la vista para que el skybox siga a la cámara
        glm::mat4 view_no_trans = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(loc_model_view, 1, GL_FALSE, glm::value_ptr(view_no_trans));
        glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projection));

        // Enlazar el cubemap a la unidad 0
        cubemap->bind(0);
        glUniform1i(loc_sampler, 0);

        // Configurar test de profundidad para que el skybox quede al fondo
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);   // No escribir en el depth buffer

        // Dibujar la malla del cubo
        glBindVertexArray(cube_mesh->vao_id);
        glDrawElements(GL_TRIANGLES, cube_mesh->index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Restaurar estado OpenGL
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glUseProgram(0);
        glEnable(GL_CULL_FACE);
    }

} 