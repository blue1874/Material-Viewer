#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int width;
uniform int height;

uniform sampler2D fbo;
uniform sampler2DMS MSAATex;

uniform bool enableMSAA;
uniform float time;

uniform float splitIntensity;

uniform float gamma;

float randomNoise(float x, float y)
{
    return fract(sin(dot(vec2(x, y), vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 sampleInMSAATex(vec2 texCoord)
{
	vec4 ans = vec4(0, 0, 0, 0);
	for(int i = 0; i < 4; i++)
	{
		ans += texelFetch(MSAATex, ivec2(texCoord.x * width, texCoord.y * height), i);
	}
	return ans / 4.0;
}

vec4 RGB_Split()
{
    float uvOffset = splitIntensity * randomNoise(time, 2);
	if(!enableMSAA) {
		float r = texture(fbo, vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
		float g = texture(fbo, TexCoords).g;
		float b = texture(fbo, vec2(TexCoords.x - uvOffset, TexCoords.y)).b;		
		return vec4(r, g, b, 1);
	}
	else {
		float R, G, B;
		R = sampleInMSAATex(vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
		G = sampleInMSAATex(TexCoords).g;
		B = sampleInMSAATex(vec2(TexCoords.x - uvOffset, TexCoords.y)).b;
		return vec4(R, G, B, 1);
	}
}

vec4 gammaCorrection(vec4 color)
{
	return vec4(pow(color.rgb, vec3(1.0 / gamma)), 1.0);	
}

void main()
{ 
    // vec4 srcColor = texture(fbo, TexCoords);
    FragColor = gammaCorrection(RGB_Split());
}