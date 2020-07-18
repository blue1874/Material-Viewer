#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "camera.h"
#include "load_texture.h"
#include "cubemap.h"
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

class Mesh
{
	unsigned int vertexSize;
	unsigned int indexSize;
	Material material;
	unsigned int VBO, IBO, VAO;
public:
	//Mesh(std::vector<glm::vec3> &_pos, std::vector<glm::vec3> &_nor, std::vector<glm::vec2> &_tex, std::vector<unsigned int> &_index, Material &material);
	Mesh(std::shared_ptr<std::vector<glm::vec3>> _pos, std::shared_ptr<std::vector<glm::vec3>> _nor, std::shared_ptr<std::vector<glm::vec2>> _tex, std::shared_ptr<std::vector<unsigned int>> _index, Material &_material);
	void draw(std::shared_ptr<Shader> shader, Camera &camera, std::string type);
};