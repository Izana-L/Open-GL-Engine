#version 330

uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec3 vertex_coordinates;

out vec3 texture_coordinates;

void main()
{
    
    texture_coordinates = vec3(vertex_coordinates.x, -vertex_coordinates.y, vertex_coordinates.z);
    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);
}