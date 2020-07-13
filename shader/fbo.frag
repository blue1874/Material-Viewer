#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D fbo;
uniform float uvOffset;

void main()
{ 
    vec4 srcColor = texture(fbo, TexCoords);
    if(gl_FragCoord.x < 300) FragColor = vec4(1.0 - vec3(srcColor), 1.0);
    else if(gl_FragCoord.x < 600) FragColor = vec4(srcColor.r, 0, 0, 1);
    else if(gl_FragCoord.x < 900) {
        float r = texture(fbo, vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
        float g = srcColor.g;
        float b = texture(fbo, vec2(TexCoords.x - uvOffset, TexCoords.y)).b;
        FragColor = vec4(r, g, b, 1);
    }
    else FragColor = srcColor;
}