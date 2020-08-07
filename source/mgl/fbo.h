#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "shader.h"
#include "tool/path.h"
#include "io/load_texture.h"
#include "scene/console.h"

class FBO
{
public:
	size_t width;
	size_t height;
	GLuint id;
	GLuint texture;
	GLuint rbo;

	GLuint quadVAO;

	std::shared_ptr<Shader> shader;
	FBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader);
	~FBO();

	virtual void draw();
private:

};

class ppFBO : public FBO
{
public:

	GLuint MSAATex;
	GLuint MSAARbo;

	ppFBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader);
	~ppFBO();
	void draw();
	void drawGui();

private:
	void switchMSAA();

	// MSAA enbale
	enumToGui<int> MSAA = enumToGui("MSAA", std::vector<std::string>{"none(x1)", "x4", "x8", "x16"}, std::vector<int>{1, 4, 8, 16}, 1);
	size_t last_index = MSAA.selected_index;

	enumToGui<float> gammaCorrection = enumToGui("gammaCorretion", std::vector<std::string>{"none(1)", "2.2", "4"}, 
		std::vector<float>{1.0, 2.2, 4.0}, 1);
};

class cubeMapFBO : public FBO
{
public:
	GLuint cubemap;
	GLuint IBLmap;
	GLuint cubeVAO = 0;
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	cubeMapFBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader);
	~cubeMapFBO();
	void draw();
};