#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int width;
uniform int height;

uniform sampler2D fbo;
uniform sampler2DMS MSAATex;
uniform int sampleNum;

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
	for(int i = 0; i < sampleNum; i++)
	{
		ans += texelFetch(MSAATex, ivec2(texCoord.x * width, texCoord.y * height), i);
	}
	return ans / float(sampleNum);
}

vec3 RGB_Split()
{
    float uvOffset = splitIntensity * randomNoise(time, 2);
	if(sampleNum == 1) {
		float r = texture(fbo, vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
		float g = texture(fbo, TexCoords).g;
		float b = texture(fbo, vec2(TexCoords.x - uvOffset, TexCoords.y)).b;		
		return vec3(r, g, b);
	}
	else {
		float R, G, B;
		R = sampleInMSAATex(vec2(TexCoords.x + uvOffset, TexCoords.y)).r;
		G = sampleInMSAATex(TexCoords).g;
		B = sampleInMSAATex(vec2(TexCoords.x - uvOffset, TexCoords.y)).b;
		return vec3(R, G, B);
	}
}

vec3 gammaCorrection(vec3 color)
{
	return pow(color, vec3(1.0 / gamma));	
}

void main()
{ 
    // vec4 srcColor = texture(fbo, TexCoords);
    vec3 result = RGB_Split();
//	result = result / (result + vec3(1.0));
//	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
}