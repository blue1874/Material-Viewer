const int maxLightNum = 8;

const int point = 0;
const int direct = 1;
const int flash = 2;

struct Light {
	int type;
	//point light arg
    vec3 position;
	float constant;
    float linear;
    float quadratic;

	//direct light arg
	vec3 direction;

	//common
	vec3 color;
	float intensity;
};


uniform int lightNum;
uniform Light lights[maxLightNum];


//vec3 calAttenuation(float constant, float linear, float quadratic, float dis)
//{
//	return 1.0f / (constant + linear * dis + pow(quadratic, 2) * dis);
//}

vec3 BlingPhong(Material material, vec2 TexCoord, Light light, vec3 Normal, vec3 LightDir, vec3 ViewDir, float Attenuation)
{
	vec3 ambientSample = vec3(texture(material.map_a, TexCoord));
	vec3 diffuseSample = vec3(texture(material.map_d, TexCoord));
	vec3 specularSample = vec3(texture(material.map_s, TexCoord));
	vec3 normalSample = vec3(texture(material.map_n, TexCoord));

	vec3 normal = normalSample == vec3(0, 0, 0) ? Normal : normalSample;

	//ambient
	vec3 ambient = material.ambient * ambientSample;
			
	//diffuse
	float diff = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diff * (material.diffuse * diffuseSample);

	// specular

	// bling-phong
	vec3 halfDir = normalize(LightDir + ViewDir);
	float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);
	// phong
//	vec3 reflectDir = reflect(-LightDir, normal);  
//	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * (material.specular * specularSample);

	return ((ambient + diffuse + specular) * light.color * light.intensity) * Attenuation;

}
vec3 calLight(Material material, vec2 TexCoord, vec3 Normal, vec3 FragPos, vec3 viewPos)
{
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < lightNum; i++)
	{
		switch (lights[i].type)
		{
		case point:
			{
				float dis = distance(lights[i].position, FragPos);
				float attenuation = 1.0f / (lights[i].constant + lights[i].linear * dis + pow(lights[i].quadratic, 2) * dis);
				result += BlingPhong(material, TexCoord, lights[i], Normal, normalize(lights[i].position - FragPos), normalize(viewPos - FragPos), attenuation);
			}
			break;
		case direct:
			{
				result += BlingPhong(material, TexCoord, lights[i], Normal, lights[i].direction, normalize(viewPos - FragPos), 1.0);
			}
			break;
		case flash:
			break;
		default:
			break;
		}
	}
	return result;
}