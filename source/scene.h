#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "model.h"
#include "light.h"
// #include "fbo.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define RES_DIR "../../resources/"
#define SKYBOX_PATH "skybox/skybox.obj"

class Scene {
private:
	unsigned int screenWidth;
	unsigned int screenHeight;

	std::vector<Model> models;
	std::vector<Camera> cameras;
	std::vector<Shader> shaders;
	std::vector<Light> lights;
	//ģ�Ͷ�Ӧ��shader���
	std::vector<unsigned int> model_shader;
	std::string resDir = RES_DIR;


	unsigned int currentShader;
	//FBO fbo;
	//GLuint fbo;

public:
	Scene(unsigned int screenWidth = SCREEN_WIDTH, unsigned int screenHeight = SCREEN_HEIGHT);
	bool initOpenGLContext();
	//unsigned int addModel(const std::string &path, unsigned int shaderIndex = 0);
	unsigned int addModel(const std::string &path);
	unsigned int addLight(const Light &light);
	unsigned int addShader(const Shader &shader);
	unsigned int addCamera(const Camera &camera);
	void setWindowSize(unsigned int w, unsigned int h);
	void useShader(unsigned int shaderIndex);
	Shader &getCurrentShader();
	Shader &getShader(unsigned int shaderIndex);
	Model &getModel(unsigned int modelIndex);
	Light &getLight(unsigned int lightIndex);
	std::vector<Light> &getLight();
	//FBO &getFBO();
	void draw();
	GLFWwindow* window;
	std::vector<std::string> includeDirs;

};