#pragma once


namespace open_gl_engine 
{
    struct ScreenQuad 
    {
        static unsigned int vao();

        static void destroy();


    private:

        static unsigned int vao_id;
        static unsigned int vbo_id;
        static bool initialized;
    };
}

