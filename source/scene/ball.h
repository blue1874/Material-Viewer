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
#include "scene/material.h"

class BallModel
{
public:
	size_t divide_num;
	size_t index_num;
	const float radius = 5.0f;
	std::shared_ptr<Material> material = std::shared_ptr<Material>(new Material());
	GLuint VAO;
	BallModel(size_t _divide_num = 60);
	~BallModel();

	void draw(std::shared_ptr<Shader> shader, Camera& camera, glm::mat4& modelMat);
private:

};
