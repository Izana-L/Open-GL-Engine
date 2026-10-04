#version 330
layout (location = 0) in vec3 pos;       
layout (location = 2) in vec2 uv;        
out vec2 TexCoords;
void main() 
{
    gl_Position = vec4(pos, 1.0);
    TexCoords = uv;
}