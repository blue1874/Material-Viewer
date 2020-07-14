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
	// build and compile our shader program
	// ------------------------------------
	unsigned int modelShaderIndex = myApp->addShader(Shader(myApp->includeDirs, "./shader/shader.vert", "./shader/shader.frag"));
	Shader modelShader;
	myApp->getShader(modelShaderIndex, modelShader);
	unsigned int lightShaderIndex = myApp->addShader(Shader(myApp->includeDirs, "./shader/light.vert", "./shader/light.frag"));
	Shader lightShader;
	myApp->getShader(lightShaderIndex, lightShader);
	unsigned int cubemapShaderIndex = myApp->addShader(Shader(myApp->includeDirs, "./shader/cubemap.vert", "./shader/cubemap.frag"));
	Shader cubemapShader;
	myApp->getShader(cubemapShaderIndex, cubemapShader);
	Shader fboShader;
	myApp->getShader(myApp->addShader(Shader(myApp->includeDirs, "./shader/fbo.vert", "./shader/fbo.frag")), fboShader);
	//Shader debugShader("debug.vert", "debug.frag");
	//Shader rimShader(includeDirs, "rim.vert", "rim.frag");

	//Model myModel("./resources/objects/SwordMinecraft/SwordMinecraft.obj");
	//Model myModel = myApp->getModel(myApp->addModel("./resources/objects/nanosuit/nanosuit.obj", modelShaderIndex));
	//Model dotlight = myApp->getModel(myApp->addModel("./resources/objects/cube/cube.obj", lightShaderIndex));
	Model myModel;
	myApp->getModel(myApp->addModel("./resources/objects/nanosuit/nanosuit.obj"), myModel);
	Model dotlight;
	myApp->getModel(myApp->addModel("./resources/objects/cube/cube.fbx"), dotlight);

	unsigned int pointLight = myApp->addLight(Light(point));

	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 rimColor = ImVec4(0.04f, 0.28f, 0.26f, 1.00f);
	float shininess = 2.0f;

	std::vector<std::string> faces
	{
		"./resources/textures/skybox/right.jpg",
		"./resources/textures/skybox/left.jpg",
		"./resources/textures/skybox/top.jpg",
		"./resources/textures/skybox/bottom.jpg",
		"./resources/textures/skybox/front.jpg",
		"./resources/textures/skybox/back.jpg"
	};
	GLuint cubemap = load_cubemap(faces);

	FBO fbo(SCREEN_WIDTH, SCREEN_HEIGHT);
	// FBO fbo;
	// fbo = FBO(SCREEN_WIDTH, SCREEN_HEIGHT);

	float uvOffset = 0.01f;
	while (!glfwWindowShouldClose(myApp->window))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

		//glEnable(GL_STENCIL_TEST);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		//glStencilMask(0x00);
		// input
		// -----
		myApp->draw();

		// render
		// ------

		//lights[0].position.x = 3.0f * sin(glfwGetTime());
		//lights[0].position.z = 3.0f * cos(glfwGetTime());

		// render the triangle

		modelShader.use();

		//Shader s1 = myApp->getCurrentShader();
		// create transformations
		glm::mat4 model(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glm::mat4 view = myApp->mainCamera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(myApp->mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
		// get matrix's uniform location and set matrix
		
		modelShader.updateUniform("model", model);
		modelShader.updateUniform("view", view);
		modelShader.updateUniform("projection", projection);
		std::vector<Light> l1;
		myApp->getLight(l1);
		modelShader.updateUniform("lightNum", (int)l1.size());
		for (int i = 0; i < l1.size(); i++) l1[i].setLightUniform(modelShader, i);
		modelShader.updateUniform("material.shininess", shininess);


		modelShader.updateUniform("viewPos", myApp->mainCamera.Position);
			//glStencilMask(0xFF);
			//glStencilFunc(GL_ALWAYS, 1, 0xFF);

		myModel.Draw(modelShader);


		//dotlight.Draw(modelShader);
		lightShader.use();

		// create transformations
		model = glm::mat4(1.0f);
		model = glm::translate(model, l1[1].position);
		model = glm::scale(model, glm::vec3(0.1f));

		lightShader.updateUniform("model", model);
		lightShader.updateUniform("view", view);
		lightShader.updateUniform("projection", projection);
		lightShader.updateUniform("lightColor", l1[1].color);
		
		dotlight.Draw(lightShader);
		glCullFace(GL_FRONT);

		cubemapShader.use();
		cubemapShader.updateUniform("view", glm::mat4(glm::mat3(view)));
		cubemapShader.updateUniform("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		cubemapShader.updateUniform("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

		dotlight.Draw(cubemapShader);
		glCullFace(GL_BACK);
		myApp->drawUniform();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
		fboShader.use();
		fboShader.updateUniform("uvOffset", uvOffset);
		fbo.draw(fboShader);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(myApp->window);
		glfwPollEvents();
	}
	return 0;
}
