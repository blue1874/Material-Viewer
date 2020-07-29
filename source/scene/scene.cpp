#include "scene.h"

Scene* Scene::instance = 0;
Camera Scene::mainCamera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
bool Scene::displayUI = false;
bool Scene::firstMouse = true;
double Scene::lastX = SCREEN_HEIGHT / 2;
double Scene::lastY = SCREEN_WIDTH / 2;
float Scene::lastFrame = 0.0f;
float Scene::deltaTime = 0.0f;;
GLuint Scene::lastKeyState = GLFW_RELEASE;
glm::mat4 Scene::modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
bool Scene::mouse_left = false;
GLuint Scene::lastMouseKeyState = GLFW_RELEASE;
glm::mat4 Scene::lastModelMat = modelMat;

Scene* Scene::getInstance(size_t screenWidth, size_t screenHeight)
{
	if(instance == 0) {
		instance = new Scene(screenWidth, screenHeight);
	}
	return instance;
}
Scene::Scene(size_t screenWidth, size_t screenHeight)
{

	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	initOpenGLContext();
	initImGui();

	{
		/**
		 * add default cube
		 * using default cubemap model, texture ,shader
		 * */
		cubeModel = std::make_shared<Model>(Cubemap::MODEL_PATH);
		cubeTexture = load_cubemap(Cubemap::faces);
		// shader() arg 1 should be moved
		cubemapShader = std::make_shared<Shader>(Shader(Cubemap::VERT_PATH, Cubemap::FRAG_PATH));

		userModel = std::make_shared<Model>(DefaultWorkFlow::MODEL_PATH);
		// shader() arg 1 should be moved
		userShader = std::make_shared<Shader>(Shader(DefaultWorkFlow::VERT_PATH, DefaultWorkFlow::FRAG_PATH));
		fboShader = std::make_shared<Shader>(Shader("./shader/postprocess/fbo.vert", "./shader/postprocess/fbo.frag"));
	}
	//
	addLight(std::make_shared<DirectLight>(DirectLight()));
	mainCamera = Camera(screenWidth, screenHeight);
	fbo = std::shared_ptr<FBO>(new FBO(screenWidth, screenHeight));
	
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
	if(instance) {
		instance->fbo = std::shared_ptr<FBO>(new FBO(width, height));
		instance->mainCamera.screenWidth = instance->screenWidth = width;
		instance->mainCamera.screenHeight = instance->screenHeight = height;
	}
	// need to be done
	// Scene::screenWidth = width;
	// Scene::screenHeight = height;
}
//void Scene::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
//{
//	if (button == GLFW_MOUSE_BUTTON_LEFT)
//	{
//		if(action == GLFW_PRESS) mouse_left = true;
//		else mouse_left = false;
//		lastMouseKeyState = action;
//	}
//}
void Scene::keyboard_process()
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (lastKeyState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
		displayUI = displayUI ? false : true;	
	lastKeyState = glfwGetKey(window, GLFW_KEY_TAB);

	mouse_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? true : false;
	if (lastMouseKeyState == GLFW_RELEASE && mouse_left == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &lastX, &lastY);
		lastModelMat = modelMat;
	}
	lastMouseKeyState = mouse_left;
	// FPS camera style
	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//	mainCamera.ProcessKeyboard(FORWARD, deltaTime);
	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//	mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	//	mainCamera.ProcessKeyboard(LEFT, deltaTime);
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//	mainCamera.ProcessKeyboard(RIGHT, deltaTime);
}
void Scene::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	// rotate model while mouse_ledt button is pressed
	if (mouse_left)
	{
		modelMat = glm::rotate(lastModelMat, glm::radians(float(xpos - lastX) / 10), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, glm::radians(float(ypos - lastY) / 10), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	// fps camera style
	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//float xoffset = xpos - lastX;
	//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	//lastX = xpos;
	//lastY = ypos;

	//mainCamera.ProcessMouseMovement(xoffset, yoffset);	
}

void Scene::mouse_ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	auto scale_para = 1 + (yoffset / 10);
	modelMat = glm::scale(modelMat, glm::vec3(scale_para));
}

bool Scene::initOpenGLContext()
{
	// glfw: initialize and configure
// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

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
	glfwSetScrollCallback(window, mouse_ScrollCallback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
					for(auto &j : userShader->uniforms.members) {
						std::visit(hookfunction(userShader, j.first), j.second);
					}
					ImGui::TreePop();				
				}
				if(ImGui::TreeNode("fbo shader")) {
					for(auto &j : fboShader->uniforms.members) {
						std::visit(hookfunction(fboShader, j.first), j.second);
					}
					ImGui::TreePop();				
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("model"))
			{
				ImGui::Text("user model");
				ImGui::Text("%d meshes, %d vertices, %d faces, %d textures", userModel->mesh_num, userModel->vertices_num, userModel->face_num, userModel->texture_num);
				RenderOption::modelFile.toGUi();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("render"))
			{
				ImGui::Text("Render options");
				RenderOption::polygonMode.toGui();
				RenderOption::drawCubeMap.toGUi();

				RenderOption::depthTest.toGUi();
				ImGui::SameLine(150);
				RenderOption::depthMask.toGUi();
				RenderOption::depthFunc.toGui();

				RenderOption::clearColor.toGui();

				RenderOption::cullFace.toGui();
				RenderOption::cullFaceFunc.toGui();

				RenderOption::enableMSAA.toGUi();
				RenderOption::enableMSAA.value != fbo->enableMSAA ? fbo->switchMSAA() : 1;

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
	else
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

}


void Scene::updateModel(std::string path)
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


void Scene::draw()
{


	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	keyboard_process();
	float shininess = 2.0f;


	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	RenderOption::handleRenderOption();
	if (RenderOption::modelFile.needReload) {
		updateModel(RenderOption::modelFile.filePath);
		RenderOption::modelFile.needReload = false;
	}
	userShader->use();
	userShader->updateUniform("lightNum", (int)lights.size());
	for (int i = 0; i < lights.size(); i++) lights[i]->setLightUniform(userShader, i);
	userShader->updateUniform("material.shininess", shininess);
	userShader->updateUniform("viewPos", mainCamera.Position);
	userModel->Draw(userShader, mainCamera, modelMat, "");

	if(RenderOption::drawCubeMap.value) {
		glCullFace(GL_FRONT);
		glActiveTexture(GL_TEXTURE0);
		cubemapShader->use();
		cubemapShader->updateUniform("skybox", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
		cubeModel->Draw(cubemapShader, mainCamera, modelMat, "cubemap");
		glCullFace(GL_BACK);
	}
		

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0x00);

	//glStencilMask(0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);
	fbo->draw(fboShader);
	drawImGui();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}
