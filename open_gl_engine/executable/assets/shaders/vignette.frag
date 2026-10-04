#version 330
uniform sampler2D screenTexture;
uniform float vignette_intensity;   

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
 
    vec2 uv = TexCoords * 2.0 - 1.0;           
    float dist = length(uv);                     
    float vignette = 1.0 - dist * vignette_intensity;
    vignette = clamp(vignette, 0.0, 1.0);

    FragColor = vec4(color * vignette, 1.0);
}