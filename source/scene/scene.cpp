#include "scene.h"

Scene* Scene::instance = 0;
Camera Scene::mainCamera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
bool Scene::displayUI = false;
bool Scene::firstMouse = true;
float Scene::lastX = SCREEN_HEIGHT / 2;
float Scene::lastY = SCREEN_WIDTH / 2;
float Scene::lastFrame = 0.0f;
float Scene::deltaTime = 0.0f;;
GLuint Scene::lastKeyState = GLFW_RELEASE;

Scene* Scene::getInstance(unsigned int screenWidth, unsigned int screenHeight)
{
	if(instance == 0) {
		instance = new Scene(screenWidth, screenHeight);
	}
	return instance;
}
Scene::Scene(unsigned int screenWidth, unsigned int screenHeight)
{

	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	initOpenGLContext();
	initImGui();

	includeDirs.push_back("");
	includeDirs.push_back("./shader/");

	{
		/**
		 * add default cube
		 * using default cubemap model, texture ,shader
		 * */
		cubeModel = std::make_shared<Model>(Cubemap::MODEL_PATH);
		cubeTexture = load_cubemap(Cubemap::faces);
		// shader() arg 1 should be moved
		cubemapShader = std::make_shared<Shader>(Shader(includeDirs, Cubemap::VERT_PATH, Cubemap::FRAG_PATH));

		userModel = std::make_shared<Model>("./resources/objects/head/head.obj");
		// shader() arg 1 should be moved
		userShader = std::make_shared<Shader>(Shader(includeDirs, "./shader/shader.vert", "./shader/shader.frag"));
		fboShader = std::make_shared<Shader>(Shader(includeDirs, "./shader/fbo.vert", "./shader/fbo.frag"));
	}
	//
	addLight(std::make_shared<DirectLight>(DirectLight()));
	mainCamera = Camera(screenWidth, screenHeight);
	fbo = std::shared_ptr<FBO>(new FBO(screenWidth, screenHeight));
	currentShader = 0;
	//fbo = FBO(screenWidth, screenHeight);
	
	lastX = screenWidth / 2;
	lastY = screenHeight / 2;
}

Scene::~Scene()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);

	// need to be done
	// Scene::screenWidth = width;
	// Scene::screenHeight = height;
}
void Scene::keyboard_process()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (lastKeyState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		displayUI = displayUI ? false : true;	
	lastKeyState = glfwGetKey(window, GLFW_KEY_TAB);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(RIGHT, deltaTime);
}
void Scene::mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

	mainCamera.ProcessMouseMovement(xoffset, yoffset);	
}

bool Scene::initOpenGLContext()
{
	// glfw: initialize and configure
// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);

	// glfw window creation
	// --------------------
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// binding callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}

void Scene::initImGui()
{
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
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Scene::drawImGui()
{
	if(displayUI) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		ImGui::Begin("console");                          // Create a window called "Hello, world!" and append into it.
		if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("shader"))
			{
				if(ImGui::TreeNode("user shader")) {
					for(auto j : userShader->uniforms.members) {
						if(std::holds_alternative<int>(j.second)) {
							auto a = std::get<int>(j.second);
							ImGui::InputInt(j.first.c_str(), &a);
							userShader->updateUniform(j.first, a);
						}
						else if(std::holds_alternative<size_t>(j.second))
						{
							int a = std::get<size_t>(j.second);
							ImGui::InputInt(j.first.c_str(), &a);
							userShader->updateUniform(j.first, a);
						}
						else if(std::holds_alternative<float>(j.second)) {
							auto a = std::get<float>(j.second);
							ImGui::InputFloat(j.first.c_str(), &a);
							userShader->updateUniform(j.first, a);
						}
						else if(std::holds_alternative<glm::vec3>(j.second)){
							auto a = std::get<glm::vec3>(j.second);
							ImGui::InputFloat3(j.first.c_str(), (float *)&a);
							userShader->updateUniform(j.first, a);
						}
					}
					ImGui::TreePop();				
				}
				if(ImGui::TreeNode("fbo shader")) {
					for(auto j : fboShader->uniforms.members) {
						if(std::holds_alternative<int>(j.second)) {
							auto a = std::get<int>(j.second); 
							ImGui::SliderInt(j.first.c_str(), &a, 0, 32);
							fboShader->updateUniform(j.first, a);
						}
						else if(std::holds_alternative<size_t>(j.second))
						{
							int a = std::get<size_t>(j.second); 
							ImGui::SliderInt(j.first.c_str(), &a, 0, 32);
							fboShader->updateUniform(j.first, a);
						}
						else if(std::holds_alternative<float>(j.second)) {
							auto a = std::get<float>(j.second); 
							ImGui::SliderFloat(j.first.c_str(), &a, 0, 5);
							fboShader->updateUniform(j.first, a);
						}
						// else if(std::holds_alternative<glm::vec3>(j.second)){
						// 	float *f = (float *) &std::get<glm::vec3>(j.second);
						// 	ImGui::InputFloat3(j.first.c_str(), f);
						// }
					}
					ImGui::TreePop();				
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("model"))
			{
				ImGui::Text("user model");
				ImGui::Text("%d meshes, %d vertices, %d faces, %d textures", userModel->mesh_num, userModel->vertices_num, userModel->face_num, userModel->texture_num);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("render"))
			{
				ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}


void Scene::updateModel(std::string && path)
{
	userModel = std::make_shared<Model>(path);
}

void Scene::addLight(std::shared_ptr<Light> light)
{
	lights.push_back(light);
}


std::shared_ptr<Model> Scene::getModel()
{
	return userModel;
}

// bool Scene::getLight(unsigned int lightIndex, Light &light)
// {
// 	if (lightIndex < 0 || lightIndex > lights.size() - 1) return false;
// 	else {
// 		light = lights[lightIndex];
// 		return true;
// 	}
// }

// void Scene::getLight(std::vector<Light> &_lights)
// {
// 	_lights = lights;
// }

// FBO & Scene::getFBO()
// {
// 	return fbo;
// }


void Scene::draw()
{
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 rimColor = ImVec4(0.04f, 0.28f, 0.26f, 1.00f);
	float shininess = 2.0f;

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	keyboard_process();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	userShader->use();
	userShader->updateUniform("lightNum", (int)lights.size());
	for (int i = 0; i < lights.size(); i++) lights[i]->setLightUniform(userShader, i);
	userShader->updateUniform("material.shininess", shininess);
	userShader->updateUniform("viewPos", mainCamera.Position);
	userModel->Draw(userShader, mainCamera, "");

	glCullFace(GL_FRONT);
	if(drawCubemap) {
		glActiveTexture(GL_TEXTURE0);
		cubemapShader->use();
		cubemapShader->updateUniform("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
		cubeModel->Draw(cubemapShader, mainCamera, "cubemap");
	}
		

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0x00);

	//glStencilMask(0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	fbo->draw(fboShader);
	drawImGui();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}
