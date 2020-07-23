#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// input helper variable
bool displayUI = false;
bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
GLuint lastKeyState = GLFW_RELEASE;

/**
 * input event callback function
 * */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);	
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

}
/**
 * @brief process keyborad input, as callback provided by glfw can not handle multiple key input
 * */
void keyboard_process();