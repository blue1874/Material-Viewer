#include "scene.h"

Scene* Scene::instance = 0;
Camera Scene::mainCamera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
bool Scene::displayUI = false;
bool Scene::firstMouse = true;
bool Scene::mouse_left = false;

double Scene::lastX = SCREEN_HEIGHT / 2;
double Scene::lastY = SCREEN_WIDTH / 2;
glm::mat4 Scene::modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
glm::mat4 Scene::lastModelMat = modelMat;

Scene* Scene::getInstance(size_t screenWidth, size_t screenHeight)
{
	if(instance == 0) {
		instance = new Scene(screenWidth, screenHeight);
	}
	return instance;
}

Scene::Scene(size_t _screenWidth, size_t _screenHeight) : screenHeight(_screenHeight), screenWidth(_screenWidth)
{

	initOpenGLContext();
	initImGui();

	{
		/**
		 * add default cube
		 * using default cubemap model, texture ,shader
		 * */
		// shader() arg 1 should be moved
		cubemapShader = std::make_shared<Shader>(Cubemap::VERT_PATH, Cubemap::FRAG_PATH);
		// shader() arg 1 should be moved
		userShader = std::make_shared<Shader>(DefaultWorkFlow::VERT_PATH, DefaultWorkFlow::FRAG_PATH);
		lightShader = std::make_shared<Shader>("shader/tool/light.vert", "shader/tool/light.frag");
		fboShader = std::make_shared<Shader>("shader/postprocess/fbo.vert", "shader/postprocess/fbo.frag");
		debugShader = std::make_shared<Shader>("shader/tool/debug.vert", "shader/tool/debug.frag", "shader/tool/debug.geom");
		IBLShader = std::make_shared<Shader>("shader/lightModel/pbr/IBL_diffuse.vert", "shader/lightModel/pbr/IBL_diffuse.frag");
	}
	//
	addLight(std::make_shared<PointLight>(PointLight(0)));
	//addLight(std::make_shared<DirectLight>(DirectLight(1)));
	mainCamera = Camera(screenWidth, screenHeight);
	fbo = std::make_shared<ppFBO>(screenWidth, screenHeight, fboShader);
	cubeMapFbo = std::make_shared<cubeMapFBO>(screenWidth, screenHeight, IBLShader);
	cube = std::make_shared<CubeModel>();
	ball = std::make_shared<BallModel>();
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

void Scene::keyboard_process()
{
	static GLuint lastKeyState = GLFW_RELEASE;
	static GLuint lastMouseKeyState = GLFW_RELEASE;

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
}

bool Scene::initOpenGLContext()
{
	// glfw: initialize and configure
// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, GLFW_DONT_CARE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	//glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GL_TRUE);



	window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	glfwSetWindowAspectRatio(window, screenWidth, screenHeight);

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
	glfwSetFramebufferSizeCallback(window, [] (GLFWwindow * window, int width, int height) {
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
		glfwSetWindowSize(window, width, height);
		int fw, fh;
		glfwGetFramebufferSize(window, &fw, &fh);
		if (instance) {
			//instance->fbo = std::shared_ptr<ppFBO>(new ppFBO(width, height));
			instance->mainCamera.screenWidth = instance->screenWidth = width;
			instance->mainCamera.screenHeight = instance->screenHeight = height;
			instance->fbo = std::make_shared<ppFBO>(instance->screenWidth, instance->screenHeight, instance->fboShader);
			instance->cubeMapFbo = std::make_shared<cubeMapFBO>(instance->screenWidth, instance->screenHeight, instance->IBLShader);
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			if (displayUI) return;
			if (mouse_left)
			{
				modelMat = glm::rotate(lastModelMat, glm::radians(float(xpos - lastX) / 10), glm::vec3(0.0f, 1.0f, 0.0f));
				modelMat = glm::rotate(modelMat, glm::radians(float(ypos - lastY) / 10), glm::vec3(1.0f, 0.0f, 0.0f));
			}
		});

	glfwSetScrollCallback(window, [] (GLFWwindow* window, double xoffset, double yoffset) {
			if (displayUI) return;
			auto scale_para = 1 + (yoffset / 10);
			modelMat = glm::scale(modelMat, glm::vec3(scale_para)); 
		});

	glfwSetErrorCallback([](int errCode, const char* errString) { std::cout << errCode << "\n" << errString << "\n"; });
	glfwShowWindow(window);
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
				if (ImGui::TreeNode("light")) {
					for (auto light : lights)
					{
						light->drawGui();
					}
					ImGui::TreePop();
				}
				if(ImGui::TreeNode("user shader")) {
					for(auto [name, value] : userShader->uniforms.members) {
						std::visit(hookfunction(userShader, name), value);
					}
					ImGui::TreePop();				
				}
				if(ImGui::TreeNode("fbo shader")) {
					for(auto [name, value] : fboShader->uniforms.members) {
						std::visit(hookfunction(fboShader, name), value);
					}
					ImGui::TreePop();				
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("material"))
			{
				ImGui::Text("user shader");
				ball->material->drawGUi();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("render"))
			{
				ImGui::Text("Render options");
				RenderOption::debugDraw.toGUi();

				RenderOption::polygonMode.toGui();
				RenderOption::drawCubeMap.toGUi();

				RenderOption::depthTest.toGUi();
				ImGui::SameLine(150);
				RenderOption::depthMask.toGUi();
				RenderOption::depthFunc.toGui();

				RenderOption::clearColor.toGui();

				RenderOption::cullFace.toGui();
				RenderOption::cullFaceFunc.toGui();

				fbo->drawGui();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("window")) {
				WindowOption::opacity.toGui();
				glfwSetWindowOpacity(window, WindowOption::opacity.value);
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

void Scene::addLight(std::shared_ptr<Light> light)
{
	lights.push_back(light);
}

void Scene::draw()
{
	[&]() {
		static int nFrames = 0;
		static float lastTime = 0;
		static float deltaTime = 0;

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		nFrames++;
		if (deltaTime > 1.0) {
			glfwSetWindowTitle(window, ("LearnOpenGL FPS: " + std::to_string(nFrames)).c_str());
			lastTime = currentTime;
			nFrames = 0;
		}
	} ();
	keyboard_process();
	float shininess = 2.0f;


	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	RenderOption::preRenderOption();

	userShader->use();
	userShader->updateUniform("lightNum", (int)lights.size());
	for (auto light : lights) light->setLightUniform(userShader);
	//userShader->updateUniform("material.shininess", shininess);
	userShader->updateUniform("viewPos", mainCamera.Position);
	userShader->updateUniform("time", float(glfwGetTime()));
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, cubeMapFbo->IBLmap);
	userShader->updateUniform("IBL", 5);
	ball->draw(userShader, mainCamera, modelMat);
	
	for (auto light : lights) light->drawModel(lightShader, mainCamera.GetViewMatrix(), mainCamera.GetProjectionMatrix());
	// debug
	if (RenderOption::debugDraw.value) {
		debugShader->use();
		debugShader->updateUniform("projection", mainCamera.GetProjectionMatrix());
		debugShader->updateUniform("view", mainCamera.GetViewMatrix());
		debugShader->updateUniform("model", modelMat);
		debugShader->setAllUniforms();
		glBindVertexArray(ball->VAO);
		glDrawElements(GL_TRIANGLES, ball->index_num, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}



	if(RenderOption::drawCubeMap.value) {
		// glCullFace(GL_FRONT);
		cube->draw(cubemapShader, mainCamera);
		cubeMapFbo->cubemap = cube->texture;
		cubeMapFbo->draw();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
		// glCullFace(GL_BACK);
	}
		

	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0x00);

	//glStencilMask(0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);

	RenderOption::fboRenderOption();
	fbo->draw();
	drawImGui();

	RenderOption::postRenderOption();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}
