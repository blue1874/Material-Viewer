struct Material 
{
	sampler2D map_a;
	sampler2D map_d;
    sampler2D map_s;   
	sampler2D map_n;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float shininess;

}; 

uniform Material material;
