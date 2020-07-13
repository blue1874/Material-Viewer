#include "scene.h"

Scene::Scene(unsigned int screenWidth, unsigned int screenHeight)
{

	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	initOpenGLContext();

	//addModel(resDir + "skybox.obj");
	addLight(Light(direct));
	Shader skyboxShader(includeDirs, "./shader/skybox.vert", "./shader/skybox.frag");
	addShader(skyboxShader);
	addCamera(Camera(glm::vec3(0.0f, 0.0f, 3.0f)));

	currentShader = 0;
	//fbo = FBO(screenWidth, screenHeight);
	includeDirs.push_back("");
	includeDirs.push_back("./shader/");
	1;
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

unsigned int Scene::addCamera(const Camera & camera)
{
	cameras.push_back(camera);
	return cameras.size() - 1;
}

void Scene::setWindowSize(unsigned int w, unsigned int h)
{
	screenWidth = w;
	screenHeight = h;
}

void Scene::useShader(unsigned int shaderIndex)
{
	if (shaderIndex < 0 || shaderIndex > shaders.size() - 1) return;
	else shaders[shaderIndex].use();
	currentShader = shaderIndex;
}

Shader & Scene::getCurrentShader()
{
	return shaders[currentShader];
}

Shader & Scene::getShader(unsigned int shaderIndex)
{
	Shader res;
	if (shaderIndex < 0 || shaderIndex > shaders.size() - 1) return res;
	else return shaders[shaderIndex];
}

Model & Scene::getModel(unsigned int modelIndex)
{
	Model res;
	if (modelIndex < 0 || modelIndex > models.size() - 1) return res;
	else return models[modelIndex];
}

Light & Scene::getLight(unsigned int lightIndex)
{
	Light res;
	if (lightIndex < 0 || lightIndex > lights.size() - 1) return res;
	else return lights[lightIndex];
}

std::vector<Light>& Scene::getLight()
{
	return lights;
}

// FBO & Scene::getFBO()
// {
// 	return fbo;
// }


void Scene::draw()
{
	for (auto i : model_shader) {
		shaders[i].use();

	}
}
