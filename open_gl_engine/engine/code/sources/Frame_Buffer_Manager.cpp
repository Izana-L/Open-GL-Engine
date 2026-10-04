#include "Frame_Buffer_Manager.hpp"
#include <cassert>
#include <glad/gl.h>
namespace open_gl_engine
{
    Frame_Buffer_Manager& Frame_Buffer_Manager::get()
    {
        static Frame_Buffer_Manager instance;
        return instance;
    }

    Frame_Buffer* Frame_Buffer_Manager::create_framebuffer(const std::string& name, int width, int height, bool with_depth)
    {
        if (frame_buffers.count(name))
            return frame_buffers.at(name).get();

        auto frame_buffer = std::make_unique<Frame_Buffer>(name, width, height, with_depth);
        auto* ptr = frame_buffer.get();
        frame_buffers[name] = std::move(frame_buffer);
        return ptr;
    }

    Frame_Buffer* Frame_Buffer_Manager::get_framebuffer(const std::string& name) const
    {
        auto it = frame_buffers.find(name);
        return (it != frame_buffers.end()) ? it->second.get() : nullptr;
    }

    
    void Frame_Buffer_Manager::resize_all(int new_width, int new_height)
    {
        for (auto& [name, frame_buffer] : frame_buffers)
        {
			assert(frame_buffer && "Frame_Buffer no válido");
			assert(frame_buffer->texture_id != 0 && "Frame_Buffer sin textura válida");
            frame_buffer->resize(new_width, new_height);
        }
    }

    void Frame_Buffer_Manager::remove_framebuffer(const std::string& name)
    {
        auto it = frame_buffers.find(name);
        if (it == frame_buffers.end()) return;

        Frame_Buffer* fbo = it->second.get();

        // Verificar si está actualmente bindeado
        GLint current_fbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fbo);
        if (fbo->framebuffer_object_id != 0 && static_cast<GLuint>(current_fbo) == fbo->framebuffer_object_id) 
        {
            // Está activo: forzar desvinculación
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            assert(false && "Eliminando un FBO que está bindeado. Se ha forzado unbind.");
          
        }

        frame_buffers.erase(it);
    }

    
}