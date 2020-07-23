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
#include "cubemap.h"
#include "mgl/fbo.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900
#define RES_DIR "../../resources/"

class Scene {
private:
	unsigned int screenWidth;
	unsigned int screenHeight;

	// used by user / cubemap
	std::shared_ptr<Model> userModel, cubeModel;
	GLuint cubeTexture;
	std::shared_ptr<Shader> userShader, cubemapShader, fboShader;
	std::vector<std::shared_ptr<Light>> lights;
	//std::vector<Light> lights;
	// 
	std::string resDir = RES_DIR;


	unsigned int currentShader;
	// scene options
	bool drawCubemap = true;

	// render optitions

	// input helper variable
	static bool displayUI;
	static bool firstMouse;
	static float lastX ;
	static float lastY;
	static float lastFrame;
	static float deltaTime;
	static GLuint lastKeyState;

	/**
	 * @brief input event callback function
	 * */
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	/**
	 * @brief process keyborad input, as callback provided by glfw can not handle multiple key input
	 * */
	void keyboard_process();

	static Scene* instance;

	std::shared_ptr<FBO> fbo;
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
	/**
	 * @brief change user model
	 * @param path model absolute path
	 * */
	void updateModel(std::string &&path);
	void addLight(std::shared_ptr<Light> light);
	/**
	 * @brief change user shader
	 * @param path shader absolute path
	 * */
	void updateShader(std::string &&path);
	/**
	 * @return a shared pointer point to user model
	 * */
    std::shared_ptr<Model> getModel();
    // bool getLight(unsigned int lightIndex, Light &light);
	// void getLight(std::vector<Light> &_lights);
	//FBO &getFBO();
	void draw();

	/**
	 * @brief draw imGUI menu
	 * including shader data, render option
	 * */
	void drawImGui();

};