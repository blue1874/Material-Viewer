#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "debug.h"
#include "scene.h"
#include "fbo.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
// #ifdef WINDOWS
//     #include <direct.h>
//     #define GetCurrentDir _getcwd
// #else
//     #include <unistd.h>
//     #define GetCurrentDir getcwd
//  #endif

//void DisplayFramebufferTexture(GLuint textureID)
//{
//	if (!notInitialized)
//	{
//		// 在屏幕右上角，使用NDC顶点坐标初始化着色器和VAO
//		glGenVertexArrays(1, &vaoDebugTexturedRect);
//		glGenBuffers(1, &VBO);
//
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
//
//
//		// position attribute
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//		glEnableVertexAttribArray(0);
//
//		//tex attribute
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texCoord));
//		glEnableVertexAttribArray(2);
//
//		glBindVertexArray(0);
//	}
//
//	glActiveTexture(GL_TEXTURE0);
//	glUseProgram(shaderDisplayFBOOutput);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	glBindVertexArray(vaoDebugTexturedRect);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//	glBindVertexArray(0);
//	glUseProgram(0);
//};
// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	// char programPath[FILENAME_MAX];
	// _getcwd(programPath, FILENAME_MAX);
	// std::string s1(programPath);
	// std::cout << s1 << "\n";
	auto myApp = Scene::getInstance();

	//Shader debugShader("debug.vert", "debug.frag");
	//Shader rimShader(includeDirs, "rim.vert", "rim.frag");

	//Model myModel("./resources/objects/SwordMinecraft/SwordMinecraft.obj");
	//Model myModel = myApp->getModel(myApp->addModel("./resources/objects/nanosuit/nanosuit.obj", modelShaderIndex));
	//Model dotlight = myApp->getModel(myApp->addModel("./resources/objects/cube/cube.obj", lightShaderIndex));

	unsigned int pointLight = myApp->addLight(Light(point));


	while (!glfwWindowShouldClose(myApp->window))
	{
		myApp->draw();
	}
	return 0;
}
