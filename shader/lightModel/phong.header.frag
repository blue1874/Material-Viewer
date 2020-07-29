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

vec3 calLight(Material material)
{
	vec3 ambientSample = vec3(texture(material.map_a, TexCoord));
	vec3 diffuseSample = vec3(texture(material.map_d, TexCoord));
	vec3 speculartSample = vec3(texture(material.map_s, TexCoord));
	vec3 normalSample = vec3(texture(material.map_n, TexCoord));

	vec3 normal = normalSample == vec3(0, 0, 0) ? normalize(Normal) : normalize(normalSample);
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < lightNum; i++)
	{
		switch (lights[i].type)
		{
		case point:
			{
				float dis = distance(lights[i].position, FragPos);
				float attenuation = 1.0f / (lights[i].constant + lights[i].linear * dis + pow(lights[i].quadratic, 2) * dis);
			
				//ambient
				vec3 ambient = material.ambient * ambientSample;
			
				//diffuse
				vec3 lightDir = normalize(lights[i].position - FragPos);
				float diff = max(dot(normal, lightDir), 0.0);
				vec3 diffuse = diff * (material.diffuse * diffuseSample);

				// specular
				vec3 viewDir = normalize(viewPos - FragPos);
				vec3 reflectDir = reflect(-lightDir, normal);  
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
				vec3 specular = spec * (material.specular * speculartSample);
				result += (ambient + diffuse + specular) * lights[i].color * attenuation * lights[i].intensity;
			}
			break;
		case direct:
			{
				//ambient
				vec3 ambient = material.ambient * ambientSample;
			
				//diffuse
				float diff = max(dot(normal, lights[i].direction), 0.0);
				vec3 diffuse = diff * (material.diffuse * diffuseSample);

				// specular
				vec3 viewDir = normalize(viewPos - FragPos);
				vec3 reflectDir = reflect(-lights[i].direction, normal);  
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
				vec3 specular = spec * (material.specular * speculartSample);
				result += (ambient + diffuse + specular) * lights[i].color * lights[i].intensity;
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