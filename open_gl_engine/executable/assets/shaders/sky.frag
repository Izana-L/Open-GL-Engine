#version 330

in vec3 texture_coordinates;
out vec4 fragment_color;

uniform samplerCube sampler;

void main()
{
    fragment_color = texture(sampler, texture_coordinates);
}