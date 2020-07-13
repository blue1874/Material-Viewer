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
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xOffset, double yOffset);
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


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

bool displayUI = false;
int lastKeyState = GLFW_RELEASE;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	// char programPath[FILENAME_MAX];
	// _getcwd(programPath, FILENAME_MAX);
	// std::string s1(programPath);
	// std::cout << s1 << "\n";
	Scene myApp;
	glfwSetFramebufferSizeCallback(myApp.window, framebuffer_size_callback);
	glfwSetCursorPosCallback(myApp.window, mouse_callback);
	glfwSetInputMode(myApp.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// build and compile our shader program
	// ------------------------------------
	unsigned int modelShaderIndex = myApp.addShader(Shader(myApp.includeDirs, "./shader/shader.vert", "./shader/shader.frag"));
	Shader modelShader = myApp.getShader(modelShaderIndex);
	unsigned int lightShaderIndex = myApp.addShader(Shader(myApp.includeDirs, "./shader/light.vert", "./shader/light.frag"));
	Shader lightShader = myApp.getShader(lightShaderIndex);
	unsigned int cubemapShaderIndex = myApp.addShader(Shader(myApp.includeDirs, "./shader/cubemap.vert", "./shader/cubemap.frag"));
	Shader cubemapShader = myApp.getShader(cubemapShaderIndex);
	Shader fboShader = myApp.getShader(myApp.addShader(Shader(myApp.includeDirs, "./shader/fbo.vert", "./shader/fbo.frag")));
	//Shader debugShader("debug.vert", "debug.frag");
	//Shader rimShader(includeDirs, "rim.vert", "rim.frag");

	//Model myModel("./resources/objects/SwordMinecraft/SwordMinecraft.obj");
	//Model myModel = myApp.getModel(myApp.addModel("./resources/objects/nanosuit/nanosuit.obj", modelShaderIndex));
	//Model dotlight = myApp.getModel(myApp.addModel("./resources/objects/cube/cube.obj", lightShaderIndex));
	Model myModel = myApp.getModel(myApp.addModel("./resources/objects/nanosuit/nanosuit.obj"));
	Model dotlight = myApp.getModel(myApp.addModel("./resources/objects/cube/cube.fbx"));

	unsigned int pointLight = myApp.addLight(Light(point));

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(myApp.window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

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
	while (!glfwWindowShouldClose(myApp.window))
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
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(myApp.window);

		// render
		// ------

		//lights[0].position.x = 3.0f * sin(glfwGetTime());
		//lights[0].position.z = 3.0f * cos(glfwGetTime());

		// render the triangle

		modelShader.use();

		//Shader s1 = myApp.getCurrentShader();
		// create transformations
		glm::mat4 model(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
		// get matrix's uniform location and set matrix
		
		modelShader.updateUniform("model", model);
		modelShader.updateUniform("view", view);
		modelShader.updateUniform("projection", projection);
		std::vector<Light> l1 = myApp.getLight();
		modelShader.updateUniform("lightNum", (int)l1.size());
		for (int i = 0; i < l1.size(); i++) l1[i].setLightUniform(modelShader, i);
		modelShader.updateUniform("material.shininess", shininess);


		modelShader.updateUniform("viewPos", camera.Position);
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
		if (displayUI)
		{
			glfwSetInputMode(myApp.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstMouse = true;
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{


				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				//ImGui::ShowDemoWindow();
				//ImGui::SliderFloat("linear", &linear, 0.0f, 1.0f);
				//ImGui::SliderFloat("rimScale", &rimScale, 1.0f, 1.1f);
				ImGui::SliderFloat("shininess", &shininess, 0.0f, 10.0f);
				ImGui::SliderFloat("uvOffset", &uvOffset, 0.0f, 0.2f);
				
				ImGui::ColorEdit3("rimColor", (float*)&rimColor);

				//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				//	counter++;
				//ImGui::SameLine();
				//ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(myApp.window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		else glfwSetInputMode(myApp.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		glfwSwapBuffers(myApp.window);
		glfwPollEvents();
	}


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (lastKeyState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		displayUI = displayUI ? false : true;	
	lastKeyState = glfwGetKey(window, GLFW_KEY_TAB);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (displayUI) return;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
