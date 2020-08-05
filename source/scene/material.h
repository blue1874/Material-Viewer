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

	fileGui albedoFile = fileGui("albedoFile");
	//fileGui diffuseFile = fileGui("diffuseFile");
	//fileGui specularFile = fileGui("specularFile");
	fileGui normalFile = fileGui("normalFile");
	fileGui metalicFile = fileGui("metalicFile");
	fileGui roughnessFile = fileGui("roughnessFile");
	fileGui AOFile = fileGui("AOFile");
	std::map<std::shared_ptr<GLuint>, std::shared_ptr<fileGui>> iter;
	Material();
	void drawGUi();
};