#include "scene.h"

Scene* Scene::instance = 0;
Camera Scene::mainCamera = Camera();
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
	//addModel(resDir + "skybox.obj");
	addLight(Light(direct));
	Shader skyboxShader(includeDirs, "./shader/skybox.vert", "./shader/skybox.frag");
	addShader(skyboxShader);
	mainCamera = Camera();
	currentShader = 0;
	//fbo = FBO(screenWidth, screenHeight);
	includeDirs.push_back("");
	includeDirs.push_back("./shader/");
	
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

void Scene::drawUniform()
{
	if(displayUI) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		if(ImGui::CollapsingHeader("shaders", ImGuiTreeNodeFlags_None)) {
			for (auto i : shaders) {
				if(ImGui::TreeNode(("ID: " + std::to_string(i.ID)).c_str())) {
					if(ImGui::TreeNode("uniforms")) {
						for(auto j : i.uniforms.members) {
							if(std::holds_alternative<int>(j.second))
								ImGui::InputInt(j.first.c_str(), &std::get<int>(j.second));
							else if(std::holds_alternative<float>(j.second))
								ImGui::InputFloat(j.first.c_str(), &std::get<float>(j.second));
							else if(std::holds_alternative<glm::vec3>(j.second)){
								float *f = (float *) &std::get<glm::vec3>(j.second);
								ImGui::InputFloat3(j.first.c_str(), f);
							}
						}
						ImGui::TreePop();							
					}
					ImGui::TreePop();							
				}
			}
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

//unsigned int Scene::addModel(const std::string & path, unsigned int shaderIndex)
//{
//	std::string _path = path;
//	models.push_back(Model(_path));
//	//model_shader.push_back(shaderIndex);
//	return models.size() - 1;
//}

unsigned int Scene::addModel(const std::string & path)
{
	std::string _path = path;
	models.push_back(Model(_path));
	return models.size() - 1;
}

unsigned int Scene::addLight(const Light & light)
{
	lights.push_back(light);
	return lights.size() - 1;

}

unsigned int Scene::addShader(const Shader & shader)
{
	shaders.push_back(shader);
	return shaders.size() - 1;

}

void Scene::useShader(unsigned int shaderIndex)
{
	if (shaderIndex < 0 || shaderIndex > shaders.size() - 1) return;
	else shaders[shaderIndex].use();
	currentShader = shaderIndex;
}

void Scene::getCurrentShader(Shader &shader)
{
	shader = shaders[currentShader];
}

bool Scene::getShader(unsigned int shaderIndex, Shader &shader)
{
	Shader res;
	if (shaderIndex < 0 || shaderIndex > shaders.size() - 1) return false;
	else {
		shader = shaders[shaderIndex];
		return true;
	}
}

bool Scene::getModel(unsigned int modelIndex, Model &model)
{
	if (modelIndex < 0 || modelIndex > models.size() - 1) return false;
	else {
		model = models[modelIndex];
		return true;
	}
}

bool Scene::getLight(unsigned int lightIndex, Light &light)
{
	if (lightIndex < 0 || lightIndex > lights.size() - 1) return false;
	else {
		light = lights[lightIndex];
		return true;
	}
}

void Scene::getLight(std::vector<Light> &_lights)
{
	_lights = lights;
}

// FBO & Scene::getFBO()
// {
// 	return fbo;
// }


void Scene::draw()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	keyboard_process();
}
