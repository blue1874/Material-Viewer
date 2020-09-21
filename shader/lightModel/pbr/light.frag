const int maxLightNum = 4;

const int point = 0;
const int direct = 1;
const int flash = 2;

float PI = 3.14159265359;

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

vec3 albedoSample = vec3(texture(material.albedoMap, TexCoord));
vec3 albedo = albedoSample == vec3(0, 0, 0) ? material.albedo : albedoSample;
//	vec3 diffuseSample = vec3(texture(material.diffuseMap, TexCoord));
//	vec3 specularSample = vec3(texture(material.specularMap, TexCoord));
float metalicSample = vec3(texture(material.metalicMap, TexCoord)).r;
float metalic = metalicSample == 0.0 ? material.metalic : metalicSample;

float roughnessSample = vec3(texture(material.roughnessMap, TexCoord)).r;
float roughness = roughnessSample == 0.0 ? material.roughness : roughnessSample;

float AOSample = vec3(texture(material.AOMap, TexCoord)).r;
float AO = AOSample == 0.0 ? material.AO : AOSample;

vec3 getNormalFromMap()
{
	vec3 sampleTex = vec3(texture(material.normalMap, TexCoord));
	if(sampleTex == vec3(0, 0, 0)) return sampleTex;
    vec3 tangentNormal = sampleTex * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 normalSample = getNormalFromMap();
vec3 normal = normalSample == vec3(0, 0, 0) ? normalize(Normal) : normalSample;
// Normal Distribution Function
// Trowbridge-Reitz GGX
float NDF(float roughness, vec3 halfVector)
{
	float r2 = roughness * roughness;
	float theta = max(dot(normal, halfVector), 0.0);
	float tmp = theta * theta * (r2 - 1) + 1;
	float n = PI * tmp * tmp;
	return r2 / n;
}

// Geometry Function
// Schlick-GGX
float GeometrySchlickGGX(float NV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

	return NV / (NV * (1.0 - k) + k);
}

float GeometrySmith(vec3 viewDir, vec3 lightDir, float roughness)
{
	float NV = max(dot(normal, viewDir), 0.0);
	float NL = max(dot(normal, lightDir), 0.0);
	return GeometrySchlickGGX(NV, roughness) * GeometrySchlickGGX(NL, roughness);
}

// Fresnel-Schlick
vec3 FSchlick(vec3 halfVector, vec3 viewDir, vec3 F0)
{
	float theta = max(dot(halfVector, viewDir), 0.0);
	return F0 + (1 - F0) * pow(1 - theta, 5);
}
uniform samplerCube IBL;

vec3 pbrLight(vec3 lightDir, vec3 viewDir)
{

	vec3 halfVector = normalize(viewDir + lightDir);

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metalic);

	vec3 ks = FSchlick(halfVector, viewDir, F0);
	vec3 ka = vec3(1.0) - ks;
	// ka *= (1.0 - metalic);
	// diffuse
	// vec3 diffuse = ka * albedo / PI;
	vec3 diffuse = ka * albedo * vec3(texture(IBL, normal)) / PI;

	// specular

	vec3 NGF = NDF(roughness, halfVector) * GeometrySmith(viewDir, lightDir, roughness) * ks;
	vec3 specular = NGF / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.001);
	return diffuse + specular;
}


vec3 calLight()
{
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < lightNum; i++)
	{
		vec3 ambient = vec3(0.03) * albedo * AO;
		vec3 viewDir = normalize(viewPos - FragPos);
		switch (lights[i].type)
		{
		case point:
			{
				// Li * Wi * normal (ambient in phong model)
				//vec3 wi = normalize(FragPos - lights[i].position);
				vec3 wi = normalize(lights[i].position - FragPos);
				float dis = distance(lights[i].position, FragPos);
				float attenuation = 1.0f / (lights[i].constant + lights[i].linear * dis + pow(lights[i].quadratic, 2) * dis);
				vec3 LightIntensity = lights[i].intensity * lights[i].color * attenuation * max(dot(wi, normal), 0.0);

				// ambient
				result += pbrLight(wi, viewDir) * LightIntensity + ambient;
			}
			break;
		case direct:
			{
				vec3 wi = lights[i].direction;
				vec3 LightIntensity = lights[i].intensity * lights[i].color * max(dot(wi, normal), 0.0);

				result += pbrLight(wi, viewDir) * LightIntensity + ambient;
			}
			break;
		case flash:
			break;
		default:
			break;
		}
	}
	result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));
	return result;
}