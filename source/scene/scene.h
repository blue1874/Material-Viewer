#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "console.h"
#include "camera.h"
#include "material.h"
#include "light.h"
#include "tool/path.h"
#include "mgl/fbo.h"
#include "ball.h"
#include "cube.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

class Scene {
private:
	size_t screenWidth;
	size_t screenHeight;

	// used by user / cubemap
	std::shared_ptr<BallModel> ball;
	std::shared_ptr<CubeModel> cube;
	std::shared_ptr<Shader> userShader, cubemapShader, fboShader, debugShader, lightShader;
	std::vector<std::shared_ptr<Light>> lights;
	static glm::mat4 modelMat;

	// input helper variable
	static bool displayUI;
	static bool firstMouse;
	static double lastX ;
	static double lastY;
	static float lastFrame;
	static float deltaTime;
	static GLuint lastKeyState;
	static bool mouse_left;
	static GLuint lastMouseKeyState;
	static glm::mat4 lastModelMat;

	/**
	 * @brief input event callback function
	 * */
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	// static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	static void mouse_ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	/**
	 * @brief process keyborad input, as callback provided by glfw can not handle multiple key input
	 * */
	void keyboard_process();

	static Scene* instance;

	std::shared_ptr<FBO> fbo;
	//GLuint fbo;
	Scene(size_t screenWidth, size_t screenHeight);
	~Scene();
	bool initOpenGLContext();
	void initImGui();
public:
	static Camera mainCamera;
	GLFWwindow* window;
	static Scene* getInstance(size_t screenWidth = SCREEN_WIDTH, size_t screenHeight = SCREEN_HEIGHT);

	void addLight(std::shared_ptr<Light> light);
	/**
	 * @brief change user shader
	 * @param path shader absolute path
	 * */
	void updateShader(std::string &&path);
	void draw();

	/**
	 * @brief draw imGUI menu
	 * including shader data, render option
	 * */
	void drawImGui();

};