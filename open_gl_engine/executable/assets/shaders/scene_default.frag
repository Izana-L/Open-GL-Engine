#version 330

struct PointLight
{
    vec3 light_position;
    vec3 light_color;
    float light_intensity;
};

struct DirectionalLight
{
    vec3 light_direction; 
    vec3 light_color;
    float light_intensity;
};

uniform PointLight point_light;
uniform DirectionalLight directional_light;
uniform bool directional_light_enabled;

uniform float ambient_intensity;
uniform float diffuse_intensity;
uniform float specular_intensity;
uniform float shininess;
uniform vec3 material_color;
uniform sampler2D sampler2d;

in vec3 pos_view;
in vec3 normal_view;
in vec2 texture_uv;

out vec4 fragment_color;

vec3 compute_lighting(vec3 surface_normal, vec3 view_direction, vec3 light_direction, vec3 light_color, float light_intensity)
{
    vec3 ambient_component = ambient_intensity * light_color;

    float diffuse_factor = max(dot(surface_normal, light_direction), 0.0);
    vec3 diffuse_component = diffuse_intensity * diffuse_factor * light_color;

    vec3 halfway_direction = normalize(light_direction + view_direction);
    float specular_factor = pow(max(dot(surface_normal, halfway_direction), 0.0), shininess);
    vec3 specular_component = specular_intensity * specular_factor * light_color;

    return (ambient_component + diffuse_component + specular_component) * light_intensity;
}

void main()
{
    vec4 texture_color = texture(sampler2d, texture_uv);
    if (texture_color.a < 0.05)
        discard;

    vec3 surface_normal = normalize(normal_view);
    vec3 view_direction = normalize(-pos_view);

    vec3 total_lighting = vec3(0.0);

    // Point light
    vec3 to_point_light = normalize(point_light.light_position - pos_view);
    total_lighting += compute_lighting(surface_normal, view_direction, to_point_light, point_light.light_color, point_light.light_intensity);
    
    // Directional light
    if (directional_light_enabled)
    {
        vec3 to_directional_light = normalize(-directional_light.light_direction);
        total_lighting += compute_lighting(surface_normal, view_direction, to_directional_light, directional_light.light_color, directional_light.light_intensity);
    }

    vec3 final_color = total_lighting * material_color * texture_color.rgb;

    fragment_color = vec4(final_color, texture_color.a);
}