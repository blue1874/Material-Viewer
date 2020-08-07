#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include "io/load_texture.h"
#include "console.h"

class Material
{
public:
	glm::vec3 albedo = glm::vec3(1.0f);
	float metalic = 1.0f;
	float roughness = 1.0f;
	float AO = 0.0f;

	GLuint albedoMap;
	//GLuint diffuseMap;
	//GLuint specularMap;
	GLuint normalMap;
	GLuint metalicMap;
	GLuint roughnessMap;
	GLuint AOMap;

	fileGui albedoFile = fileGui("albedoFile", DefaultWorkFlow::ALBEDO);
	//fileGui diffuseFile = fileGui("diffuseFile");
	//fileGui specularFile = fileGui("specularFile");
	fileGui normalFile = fileGui("normalFile", DefaultWorkFlow::NORMAL);
	fileGui metalicFile = fileGui("metalicFile", DefaultWorkFlow::METALLIC);
	fileGui roughnessFile = fileGui("roughnessFile", DefaultWorkFlow::ROUGHNESS);
	fileGui AOFile = fileGui("AOFile", DefaultWorkFlow::AO);
	std::map<std::shared_ptr<GLuint>, std::shared_ptr<fileGui>> iter;
	Material();
	void drawGUi();
};