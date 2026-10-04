#version 330

layout (location = 0) in vec3 vertex_coordinates;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texture_uv;

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat4 normal_matrix;

out vec3 pos_view;
out vec3 normal_view;
out vec2 texture_uv;

void main()
{
    vec4 view_pos = model_view_matrix * vec4(vertex_coordinates, 1.0);
    pos_view = view_pos.xyz;
    normal_view = normalize((normal_matrix * vec4(vertex_normal, 0.0)).xyz);
    texture_uv = vertex_texture_uv;
    gl_Position = projection_matrix * view_pos;
}