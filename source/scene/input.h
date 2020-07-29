#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Input {
	// input helper variable
	inline bool displayUI = false;
	inline bool firstMouse = true;
	inline float lastX = 0.0f;
	inline float lastY = 0.0f;
	inline GLuint lastKeyState = GLFW_RELEASE;

	/**
	 * input event callback function
	 * */
	inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	};
	//inline auto getMouseCallback(glm::mat4 modelMat) -> void(*)(GLFWwindow *window, double xpos, double ypos) {
	//	if (displayUI) return;
	//	if (firstMouse)
	//	{
	//		lastX = xpos;
	//		lastY = ypos;
	//		firstMouse = false;
	//	}

	//	float xoffset = xpos - lastX;
	//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	//	return void(*)(GLFWwindow *window, double xpos, double ypos) {};
	//};
	inline void mouse_callback(GLFWwindow *window, double xpos, double ypos)
	{

	};

	/**
	 * @brief process keyborad input, as callback provided by glfw can not handle multiple key input
	 * */
	inline void keyboard_process(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (lastKeyState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
			displayUI = displayUI ? false : true;
		lastKeyState = glfwGetKey(window, GLFW_KEY_TAB);
	};
}

