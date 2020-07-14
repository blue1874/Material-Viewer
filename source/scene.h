#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
	std::vector<Shader> shaders;
	std::vector<Light> lights;
	// 
	std::vector<unsigned int> model_shader;
	std::string resDir = RES_DIR;


	unsigned int currentShader;

	// input helper variable
	static bool displayUI;
	static bool firstMouse;
	static float lastX ;
	static float lastY;
	static float lastFrame;
	static float deltaTime;
	static GLuint lastKeyState;

	/**
	 * input event callback function
	 * */
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	/**
	 * @brief process keyborad input, as callback provided by glfw can not handle multiple key input
	 * */
	void keyboard_process();

	static Scene* instance;

	//FBO fbo;
	//GLuint fbo;
	Scene(unsigned int screenWidth, unsigned int screenHeight);
	~Scene();
	bool initOpenGLContext();
	void initImGui();
public:
	static Camera mainCamera;
	GLFWwindow* window;
	std::vector<std::string> includeDirs;
	static Scene* getInstance(unsigned int screenWidth = SCREEN_WIDTH, unsigned int screenHeight = SCREEN_HEIGHT);
	//unsigned int addModel(const std::string &path, unsigned int shaderIndex = 0);
	unsigned int addModel(const std::string &path);
	unsigned int addLight(const Light &light);
	unsigned int addShader(const Shader &shader);
	void useShader(unsigned int shaderIndex);
	void getCurrentShader(Shader &shader);
    bool getShader(unsigned int shaderIndex, Shader &shader);
    bool getModel(unsigned int modelIndex, Model &model);
    bool getLight(unsigned int lightIndex, Light &light);
	void getLight(std::vector<Light> &_lights);
	//FBO &getFBO();
	void draw();

	void drawUniform();

};