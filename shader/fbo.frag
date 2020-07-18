#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D fbo;
uniform float time;

uniform float splitIntensity;

float randomNoise(float x, float y)
{
    return fract(sin(dot(vec2(x, y), vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 RGB_Split()
{
    float uvOffset = splitIntensity * randomNoise(time, 2);
    float r = texture(fbo, vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
    float g = texture(fbo, TexCoords).g;
    float b = texture(fbo, vec2(TexCoords.x - uvOffset, TexCoords.y)).b;
    return vec4(r, g, b, 1);
}
void main()
{ 
    vec4 srcColor = texture(fbo, TexCoords);
    FragColor = RGB_Split();
}