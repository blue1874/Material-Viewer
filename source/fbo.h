#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
struct FBO
{
	GLuint id;
	GLuint texture;
	GLuint rbo;
	float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
			};

	GLuint quadVAO, quadVBO;
	FBO();
	FBO(unsigned int width, unsigned int height);
	~FBO();
	void draw(std::shared_ptr<Shader> shader);
};