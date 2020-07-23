#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mgl/shader.h"
#include "scene/camera.h"
#include "io/load_texture.h"
#include "scene/cubemap.h"
#define MAX_TEXTURE_NUM 16
enum TextureType {normal, ambient, diffuse, specular};

/**
 * use ptr to manage data
 * store ptr and data size
 * */
// struct Vertex
// {
// 	std::shared_ptr<std::vector<glm::vec3>> positionPtr; 
// 	std::shared_ptr<std::vector<glm::vec3>> normalPtr;
// 	std::shared_ptr<std::vector<glm::vec2>> texCoordPtr;
// 	Vertex() {
// 		positionPtr = std::make_shared<std::vector<glm::vec3>>(); 
// 		normalPtr = std::make_shared<std::vector<glm::vec3>>();
// 		texCoordPtr = std::make_shared<std::vector<glm::vec2>>();
// 	};
// 	Vertex(	std::shared_ptr<std::vector<glm::vec3>> pP,std::shared_ptr<std::vector<glm::vec3>> nP,std::shared_ptr<std::vector<glm::vec2>> tP) : 
// 	positionPtr(pP), normalPtr(nP), texCoordPtr(tP) {};
// 	// ~Vertex() {
// 	// 	positionPtr.reset();
// 	// 	normalPtr.reset();
// 	// 	texCoordPtr.reset();
// 	// }
// };

// struct Index
// {
// 	std::shared_ptr<std::vector<unsigned int>> indexPtr;
// 	Index() {
// 		indexPtr = std::make_shared<std::vector<unsigned int>>(); 
// 	};
// 	Index(std::shared_ptr<std::vector<unsigned int>> iP) : indexPtr(iP) {};
// 	// ~Index() {
// 	// 	indexPtr.reset();
// 	// }
// };

struct Texture
{
	int id;
	TextureType type;
	//texture path, used to avoid duplicate texture load
	aiString path;  

	Texture(): id(), type(), path("") {};
};

struct Material
{
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;

	Texture map_Ka;
	Texture map_Kd;
	Texture map_Ks;
	Texture map_Kn;
};

/**
 * @brief store mesh data pointer and size in order to transport them to openGL
 * */
struct Vertex 
{
	void *position;
	// size in memory, not nums of data
	size_t position_size;
	void *normal;
	size_t normal_size;
	void *texCoord;
	size_t texCoord_size;
	void *index;
	size_t index_size;
};

class Mesh
{
	size_t vertexSize;
	size_t indexSize;
	Material material;
	unsigned int VBO, IBO, VAO;
public:
	/**
	 * @brief construct openGL buffer and transport data to them by using Veretx and Material
	 * @param vertex position, normal, texCoord, index
	 * */
	Mesh(Vertex vertex, Material &_material);
	void draw(std::shared_ptr<Shader> shader, Camera &camera, std::string type);
};