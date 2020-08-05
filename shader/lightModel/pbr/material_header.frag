struct Material {
	vec3 albedo;
	float metalic;
	float roughness;
	float AO;

	sampler2D albedoMap;
//	sampler2D diffuseMap;
//	sampler2D specularMap;
	sampler2D normalMap;
	sampler2D metalicMap;
	sampler2D roughnessMap;
	sampler2D AOMap;
};

uniform Material material;