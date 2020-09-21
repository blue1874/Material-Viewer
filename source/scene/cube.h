#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cassert>

#include "mgl/shader.h"
#include "scene/camera.h"
#include "tool/path.h"
#include "io/textureIO.h"

class CubeModel
{
public:
	size_t index_num;
	GLuint VAO;
	GLuint texture;
	CubeModel(glm::vec3 offset = glm::vec3(0.0f));
	~CubeModel();

	// draw light cube
	void draw();

	//draw skybox cube
	void draw(std::shared_ptr<Shader> shader, Camera& camera);
private:

};
