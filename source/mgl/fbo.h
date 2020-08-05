#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"
struct FBO
{
	size_t width;
	size_t height;
	GLuint id;
	GLuint texture;
	GLuint rbo;

	GLuint MSAATex;
	GLuint MSAARbo;
	size_t sample_num;
	bool enableMSAA;

	float gamma = 1.0f;
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	float quadVertices[24] = { 
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
	FBO(size_t _width, size_t _height, bool enable_msaa = true, size_t _sample_num = 4);
	void switchMSAA();
	~FBO();
	void draw(std::shared_ptr<Shader> shader);
};