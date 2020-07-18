#pragma once

#include "mesh.h"
#include "load_texture.h"
class Model
{
	public:
		/*  ����   */
		Model();
		Model(std::string &path);
		Model(std::string &&path);
		void Draw(std::shared_ptr<Shader> shader, Camera &camera, std::string type);
	private:
		/*  ģ������  */
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;
		/*  ����   */
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		Texture &loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName);
		//Texture &loadCubeMap(std::vector<std::string> &faces);
};