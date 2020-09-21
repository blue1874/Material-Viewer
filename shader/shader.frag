#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal; 

uniform vec3 viewPos;
uniform float time;
uniform float borderWidth;

#include "lightModel/pbr/material.frag"
#include "lightModel/pbr/light.frag"

#include "math/noise/noise.frag"

void main()
{
	vec2 uv = TexCoord * vec2(10);
	float ans = simplex_noise_2d(uv);
	
	float border = sin(time);
	vec3 color = vec3(0.0);
	color = calLight();
	//if(ans < border) color = calLight();
	//else if(ans < border + borderWidth) color = vec3(1.0, 0.0, 0.0);
	FragColor = vec4(color, 1);
//    float depth = LinearizeDepth(gl_FragCoord.z) / far; // far
//    FragColor = vec4(vec3(depth), 1.0);
} 